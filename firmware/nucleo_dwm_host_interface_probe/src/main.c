#include <errno.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include <zephyr/device.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/spi.h>
#include <zephyr/kernel.h>
#include <zephyr/sys/byteorder.h>
#include <zephyr/sys/printk.h>

#include "tphip.h"

#define SPI5_FREQUENCY_HZ 1000000U
#define CS_SETUP_US 10U
#define CS_HOLD_US 10U
#define RESET_LOW_MS 10U
#define RESET_SETTLE_MS 1000U
#define READY_TIMEOUT_MS 250U
#define INTER_TRANSACTION_MS 50U
#define VALID_EXCHANGE_COUNT 10U

#define GUARD_PRE 0x13579BDFu
#define GUARD_POST 0x2468ACE0u

struct guarded_request_buffer {
    uint32_t pre_guard;
    uint8_t bytes[TPHIP_REQUEST_TRANSACTION_LEN];
    uint32_t post_guard;
};

struct guarded_response_buffer {
    uint32_t pre_guard;
    uint8_t bytes[TPHIP_RESPONSE_TRANSACTION_LEN];
    uint32_t post_guard;
};

static const struct device *const spi5_dev = DEVICE_DT_GET(DT_NODELABEL(spi5));
static const struct gpio_dt_spec dwm_reset =
    GPIO_DT_SPEC_GET(DT_ALIAS(dwm_reset), gpios);
static const struct gpio_dt_spec dwm_cs =
    GPIO_DT_SPEC_GET(DT_ALIAS(dwm_cs), gpios);
static const struct gpio_dt_spec dwm_ready =
    GPIO_DT_SPEC_GET(DT_ALIAS(dwm_ready), gpios);

static struct guarded_request_buffer request_tx;
static struct guarded_request_buffer request_discard_rx;
static struct guarded_response_buffer response_clock_tx;
static struct guarded_response_buffer response_rx;

static const struct spi_config spi5_config = {
    .frequency = SPI5_FREQUENCY_HZ,
    .operation = SPI_OP_MODE_MASTER |
                 SPI_WORD_SET(8) |
                 SPI_TRANSFER_MSB,
    .slave = 0,
};

static void initialise_guards(void)
{
    request_tx.pre_guard = GUARD_PRE;
    request_tx.post_guard = GUARD_POST;
    request_discard_rx.pre_guard = GUARD_PRE;
    request_discard_rx.post_guard = GUARD_POST;
    response_clock_tx.pre_guard = GUARD_PRE;
    response_clock_tx.post_guard = GUARD_POST;
    response_rx.pre_guard = GUARD_PRE;
    response_rx.post_guard = GUARD_POST;
}

static int guards_ok(void)
{
    return request_tx.pre_guard == GUARD_PRE &&
           request_tx.post_guard == GUARD_POST &&
           request_discard_rx.pre_guard == GUARD_PRE &&
           request_discard_rx.post_guard == GUARD_POST &&
           response_clock_tx.pre_guard == GUARD_PRE &&
           response_clock_tx.post_guard == GUARD_POST &&
           response_rx.pre_guard == GUARD_PRE &&
           response_rx.post_guard == GUARD_POST;
}

static void print_bytes(const char *label, const uint8_t *data, size_t len)
{
    size_t i;

    printk("%s", label);
    for (i = 0u; i < len; i++) {
        printk("%02x", data[i]);
        if ((i + 1u) < len) {
            printk(" ");
        }
    }
    printk("\n");
}

static int check_gpio_ready(const struct gpio_dt_spec *spec, const char *name)
{
    if (!gpio_is_ready_dt(spec)) {
        printk("ERROR: %s GPIO is not ready\n", name);
        return -ENODEV;
    }

    printk("gpio_ready name=%s controller=%s pin=%u\n",
           name,
           spec->port->name,
           (unsigned int)spec->pin);
    return 0;
}

static int reset_dwm(void)
{
    int ret;

    printk("dwm_reset phase=drive_low duration_ms=%u\n", RESET_LOW_MS);

    ret = gpio_pin_configure(dwm_reset.port, dwm_reset.pin, GPIO_OUTPUT);
    if (ret != 0) {
        return ret;
    }

    gpio_pin_set_raw(dwm_reset.port, dwm_reset.pin, 0);
    k_msleep(RESET_LOW_MS);

    ret = gpio_pin_configure(dwm_reset.port, dwm_reset.pin, GPIO_INPUT);
    if (ret != 0) {
        return ret;
    }

    printk("dwm_reset phase=released_high_z settle_ms=%u\n", RESET_SETTLE_MS);
    k_msleep(RESET_SETTLE_MS);
    return 0;
}

static int wait_ready_level(int expected, uint32_t timeout_ms)
{
    int64_t deadline = k_uptime_get() + timeout_ms;

    do {
        int level = gpio_pin_get_raw(dwm_ready.port, dwm_ready.pin);

        if (level < 0) {
            return level;
        }

        if (level == expected) {
            return 0;
        }

        k_msleep(1);
    } while (k_uptime_get() < deadline);

    return -ETIMEDOUT;
}

static int spi5_manual_cs_transfer(
    const uint8_t *tx,
    uint8_t *rx,
    size_t len)
{
    struct spi_buf tx_buffer = {
        .buf = (void *)tx,
        .len = len,
    };
    struct spi_buf rx_buffer = {
        .buf = rx,
        .len = len,
    };
    const struct spi_buf_set tx_set = {
        .buffers = &tx_buffer,
        .count = 1,
    };
    const struct spi_buf_set rx_set = {
        .buffers = &rx_buffer,
        .count = 1,
    };
    int ret;

    gpio_pin_set_raw(dwm_cs.port, dwm_cs.pin, 0);
    k_usleep(CS_SETUP_US);

    ret = spi_transceive(spi5_dev, &spi5_config, &tx_set, &rx_set);

    k_usleep(CS_HOLD_US);
    gpio_pin_set_raw(dwm_cs.port, dwm_cs.pin, 1);

    return ret;
}

static int parser_sequence_mismatch_self_test(void)
{
    struct tphip_capabilities capabilities;
    struct tphip_response_view response;
    uint8_t frame[TPHIP_RESPONSE_TRANSACTION_LEN];
    enum tphip_validation_result validation;

    tphip_default_capabilities(&capabilities);
    (void)tphip_build_response(
        frame,
        TPHIP_OP_GET_CAPABILITIES,
        2u,
        TPHIP_STATUS_OK,
        &capabilities);

    validation = tphip_validate_response(
        frame,
        sizeof(frame),
        TPHIP_RSP_GET_CAPABILITIES,
        1u,
        &response);

    printk("self_test=sequence_mismatch expected=BAD_SEQUENCE actual=%s result=%s\n",
           tphip_validation_name(validation),
           validation == TPHIP_VALIDATE_BAD_SEQUENCE ? "PASS" : "FAIL");

    return validation == TPHIP_VALIDATE_BAD_SEQUENCE ? 0 : -EINVAL;
}

static int exchange_request(
    const char *test_name,
    enum tphip_status expected_status,
    uint8_t expected_response_opcode,
    uint16_t expected_sequence,
    int require_capabilities)
{
    struct tphip_response_view response;
    struct tphip_capabilities capabilities;
    enum tphip_validation_result validation;
    int ret;

    memset(request_discard_rx.bytes, 0, sizeof(request_discard_rx.bytes));
    memset(response_clock_tx.bytes, 0, sizeof(response_clock_tx.bytes));
    memset(response_rx.bytes, 0, sizeof(response_rx.bytes));

    if (!guards_ok()) {
        printk("test=%s result=FAIL reason=guard_before_request\n", test_name);
        return -EFAULT;
    }

    ret = spi5_manual_cs_transfer(
        request_tx.bytes,
        request_discard_rx.bytes,
        sizeof(request_tx.bytes));
    if (ret != 0) {
        printk("test=%s result=FAIL phase=request_spi ret=%d\n", test_name, ret);
        return ret;
    }

    ret = wait_ready_level(1, READY_TIMEOUT_MS);
    if (ret != 0) {
        printk("test=%s result=FAIL phase=ready_high ret=%d\n", test_name, ret);
        return ret;
    }

    ret = spi5_manual_cs_transfer(
        response_clock_tx.bytes,
        response_rx.bytes,
        sizeof(response_rx.bytes));
    if (ret != 0) {
        printk("test=%s result=FAIL phase=response_spi ret=%d\n", test_name, ret);
        return ret;
    }

    ret = wait_ready_level(0, READY_TIMEOUT_MS);
    if (ret != 0) {
        printk("test=%s result=FAIL phase=ready_low ret=%d\n", test_name, ret);
        return ret;
    }

    validation = tphip_validate_response(
        response_rx.bytes,
        sizeof(response_rx.bytes),
        expected_response_opcode,
        expected_sequence,
        &response);

    if (validation != TPHIP_VALIDATE_OK) {
        printk("test=%s result=FAIL validation=%s\n",
               test_name,
               tphip_validation_name(validation));
        print_bytes("request=", request_tx.bytes, sizeof(request_tx.bytes));
        print_bytes("response=", response_rx.bytes, sizeof(response_rx.bytes));
        return -EBADMSG;
    }

    if (response.status != expected_status) {
        printk("test=%s expected=%s actual=%s result=FAIL\n",
               test_name,
               tphip_status_name(expected_status),
               tphip_status_name(response.status));
        return -EPROTO;
    }

    if (require_capabilities) {
        ret = tphip_decode_capabilities(
            response_rx.bytes,
            &response,
            &capabilities);
        if (ret != 0) {
            printk("test=%s result=FAIL reason=capabilities_decode\n", test_name);
            return ret;
        }

        if (capabilities.protocol_major != 1u ||
            capabilities.protocol_minor != 0u ||
            capabilities.capability_flags !=
                (TPHIP_CAP_IRQ_SUPPORTED | TPHIP_CAP_CRC_SUPPORTED) ||
            capabilities.max_payload_bytes != TPHIP_MAX_PAYLOAD_BYTES ||
            capabilities.max_record_bytes != 0u ||
            capabilities.interface_build_id != TPHIP_INTERFACE_BUILD_ID) {
            printk("test=%s result=FAIL reason=capabilities_value_mismatch\n",
                   test_name);
            return -EPROTO;
        }

        printk("host_if_probe=GET_CAPABILITIES sequence=%u status=OK crc=OK protocol=%u.%u flags=0x%04x max_payload=%u max_record=%u build_id=0x%08x guard=%s result=PASS\n",
               (unsigned int)expected_sequence,
               (unsigned int)capabilities.protocol_major,
               (unsigned int)capabilities.protocol_minor,
               (unsigned int)capabilities.capability_flags,
               (unsigned int)capabilities.max_payload_bytes,
               (unsigned int)capabilities.max_record_bytes,
               (unsigned int)capabilities.interface_build_id,
               guards_ok() ? "OK" : "FAIL");
    } else {
        printk("test=%s expected=%s actual=%s sequence=%u crc=OK guard=%s result=%s\n",
               test_name,
               tphip_status_name(expected_status),
               tphip_status_name(response.status),
               (unsigned int)expected_sequence,
               guards_ok() ? "OK" : "FAIL",
               guards_ok() ? "PASS" : "FAIL");
    }

    if (!guards_ok()) {
        return -EFAULT;
    }

    k_msleep(INTER_TRANSACTION_MS);
    return 0;
}

static int run_valid_exchange(uint16_t sequence)
{
    int ret = tphip_build_get_capabilities_request(request_tx.bytes, sequence);

    if (ret != 0) {
        return ret;
    }

    return exchange_request(
        "valid_get_capabilities",
        TPHIP_STATUS_OK,
        TPHIP_RSP_GET_CAPABILITIES,
        sequence,
        1);
}

static int run_unknown_version_test(uint16_t sequence)
{
    (void)tphip_build_get_capabilities_request(request_tx.bytes, sequence);
    request_tx.bytes[TPHIP_OFFSET_VERSION] = 2u;
    (void)tphip_refresh_crc(request_tx.bytes, sizeof(request_tx.bytes));

    return exchange_request(
        "unknown_version",
        TPHIP_STATUS_BAD_VERSION,
        TPHIP_RSP_GET_CAPABILITIES,
        sequence,
        0);
}

static int run_reserved_flags_test(uint16_t sequence)
{
    (void)tphip_build_get_capabilities_request(request_tx.bytes, sequence);
    request_tx.bytes[TPHIP_OFFSET_FLAGS] = 0x04u;
    (void)tphip_refresh_crc(request_tx.bytes, sizeof(request_tx.bytes));

    return exchange_request(
        "reserved_flags",
        TPHIP_STATUS_BAD_FLAGS,
        TPHIP_RSP_GET_CAPABILITIES,
        sequence,
        0);
}

static int run_oversized_payload_test(uint16_t sequence)
{
    (void)tphip_build_get_capabilities_request(request_tx.bytes, sequence);
    sys_put_le16(
        TPHIP_MAX_PAYLOAD_BYTES + 1u,
        &request_tx.bytes[TPHIP_OFFSET_PAYLOAD_LEN]);
    (void)tphip_refresh_crc(request_tx.bytes, sizeof(request_tx.bytes));

    return exchange_request(
        "oversized_payload",
        TPHIP_STATUS_BAD_LENGTH,
        TPHIP_RSP_GET_CAPABILITIES,
        sequence,
        0);
}

static int run_bad_crc_test(uint16_t sequence)
{
    (void)tphip_build_get_capabilities_request(request_tx.bytes, sequence);
    request_tx.bytes[TPHIP_OFFSET_CRC16] ^= 0x01u;

    return exchange_request(
        "bad_crc",
        TPHIP_STATUS_BAD_CRC,
        TPHIP_RSP_GET_CAPABILITIES,
        sequence,
        0);
}

static int run_unknown_opcode_test(uint16_t sequence)
{
    (void)tphip_build_get_capabilities_request(request_tx.bytes, sequence);
    request_tx.bytes[TPHIP_OFFSET_OPCODE] = 0x7fu;
    (void)tphip_refresh_crc(request_tx.bytes, sizeof(request_tx.bytes));

    return exchange_request(
        "unknown_opcode",
        TPHIP_STATUS_UNKNOWN_OPCODE,
        0xffu,
        sequence,
        0);
}

int main(void)
{
    unsigned int passes = 0u;
    unsigned int failures = 0u;
    uint16_t sequence;
    int ret;

    printk("\nThin-Pod NUCLEO DWM TPHIP GET_CAPABILITIES probe\n");
    printk("board=nucleo_n657x0_q spi=SPI5 frequency=%u mode=0\n",
           SPI5_FREQUENCY_HZ);
    printk("request_len=%u response_len=%u valid_exchanges=%u\n",
           TPHIP_REQUEST_TRANSACTION_LEN,
           TPHIP_RESPONSE_TRANSACTION_LEN,
           VALID_EXCHANGE_COUNT);
    printk("pin_map sck=PE15 copi=PG2 cipo=PG1 cs=PA3 ready=PB9 reset=PD0\n");

    if (!device_is_ready(spi5_dev)) {
        printk("ERROR: SPI5 device is not ready\n");
        return -ENODEV;
    }

    ret = check_gpio_ready(&dwm_reset, "DWM_RESET");
    if (ret != 0) {
        return ret;
    }
    ret = check_gpio_ready(&dwm_cs, "DWM_CS");
    if (ret != 0) {
        return ret;
    }
    ret = check_gpio_ready(&dwm_ready, "DWM_READY");
    if (ret != 0) {
        return ret;
    }

    ret = gpio_pin_configure(dwm_cs.port, dwm_cs.pin, GPIO_OUTPUT);
    if (ret != 0) {
        return ret;
    }
    gpio_pin_set_raw(dwm_cs.port, dwm_cs.pin, 1);

    ret = gpio_pin_configure(dwm_ready.port, dwm_ready.pin, GPIO_INPUT);
    if (ret != 0) {
        return ret;
    }

    initialise_guards();

    if (parser_sequence_mismatch_self_test() == 0) {
        passes++;
    } else {
        failures++;
    }

    ret = reset_dwm();
    if (ret != 0) {
        printk("ERROR: DWM reset failed ret=%d\n", ret);
        return ret;
    }

    ret = wait_ready_level(0, READY_TIMEOUT_MS);
    if (ret != 0) {
        printk("ERROR: READY did not settle low ret=%d\n", ret);
        return ret;
    }

    for (sequence = 1u; sequence <= VALID_EXCHANGE_COUNT; sequence++) {
        if (run_valid_exchange(sequence) == 0) {
            passes++;
        } else {
            failures++;
        }
    }

    if (run_unknown_version_test(1001u) == 0) {
        passes++;
    } else {
        failures++;
    }

    if (run_reserved_flags_test(1002u) == 0) {
        passes++;
    } else {
        failures++;
    }

    if (run_oversized_payload_test(1003u) == 0) {
        passes++;
    } else {
        failures++;
    }

    if (run_bad_crc_test(1004u) == 0) {
        passes++;
    } else {
        failures++;
    }

    if (run_unknown_opcode_test(1005u) == 0) {
        passes++;
    } else {
        failures++;
    }

    printk("suite=GET_CAPABILITIES passes=%u failures=%u guard=%s result=%s\n",
           passes,
           failures,
           guards_ok() ? "OK" : "FAIL",
           failures == 0u && guards_ok() ? "PASS" : "FAIL");

    while (1) {
        int ready_level = gpio_pin_get_raw(dwm_ready.port, dwm_ready.pin);
        printk("host_if_probe idle ready=%d suite_result=%s\n",
               ready_level,
               failures == 0u && guards_ok() ? "PASS" : "FAIL");
        k_msleep(5000);
    }

    return 0;
}