#include <errno.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include <zephyr/device.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/spi.h>
#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>

#include "tphip.h"

#if !DT_NODE_HAS_STATUS(DT_NODELABEL(spi2), okay)
#error "spi2 must be enabled as the DWM host-interface SPIS endpoint"
#endif

#if !DT_NODE_EXISTS(DT_ALIAS(tphost_ready))
#error "Missing devicetree alias tphost-ready"
#endif

#define SPIS_FREQUENCY_HZ 1000000U
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

static const struct device *const spis_dev = DEVICE_DT_GET(DT_NODELABEL(spi2));
static const struct gpio_dt_spec host_ready =
    GPIO_DT_SPEC_GET(DT_ALIAS(tphost_ready), gpios);

static struct guarded_request_buffer request_rx;
static struct guarded_request_buffer request_idle_tx;
static struct guarded_response_buffer response_tx;
static struct guarded_response_buffer response_discard_rx;

static volatile int response_result;
K_SEM_DEFINE(response_done, 0, 1);

static const struct spi_config spis_config = {
    .frequency = SPIS_FREQUENCY_HZ,
    .operation = SPI_OP_MODE_SLAVE |
                 SPI_WORD_SET(8) |
                 SPI_TRANSFER_MSB,
    .slave = 0,
};

static void initialise_guards(void)
{
    request_rx.pre_guard = GUARD_PRE;
    request_rx.post_guard = GUARD_POST;
    request_idle_tx.pre_guard = GUARD_PRE;
    request_idle_tx.post_guard = GUARD_POST;
    response_tx.pre_guard = GUARD_PRE;
    response_tx.post_guard = GUARD_POST;
    response_discard_rx.pre_guard = GUARD_PRE;
    response_discard_rx.post_guard = GUARD_POST;
}

static int guards_ok(void)
{
    return request_rx.pre_guard == GUARD_PRE &&
           request_rx.post_guard == GUARD_POST &&
           request_idle_tx.pre_guard == GUARD_PRE &&
           request_idle_tx.post_guard == GUARD_POST &&
           response_tx.pre_guard == GUARD_PRE &&
           response_tx.post_guard == GUARD_POST &&
           response_discard_rx.pre_guard == GUARD_PRE &&
           response_discard_rx.post_guard == GUARD_POST;
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

static void response_complete_callback(
    const struct device *dev,
    int result,
    void *userdata)
{
    ARG_UNUSED(dev);
    ARG_UNUSED(userdata);

    response_result = result;
    k_sem_give(&response_done);
}

static int receive_request(void)
{
    struct spi_buf tx_buffer = {
        .buf = request_idle_tx.bytes,
        .len = sizeof(request_idle_tx.bytes),
    };
    struct spi_buf rx_buffer = {
        .buf = request_rx.bytes,
        .len = sizeof(request_rx.bytes),
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

    memset(request_rx.bytes, 0, sizeof(request_rx.bytes));
    memset(request_idle_tx.bytes, 0xff, sizeof(request_idle_tx.bytes));

    printk("tphip_dwm request_armed len=%u\n",
           (unsigned int)sizeof(request_rx.bytes));

    ret = spi_transceive(spis_dev, &spis_config, &tx_set, &rx_set);
    if (ret < 0) {
        printk("tphip_dwm request_transfer_error ret=%d\n", ret);
        return ret;
    }

    if (ret != TPHIP_REQUEST_TRANSACTION_LEN) {
        printk("tphip_dwm request_short expected=%u actual=%d\n",
               TPHIP_REQUEST_TRANSACTION_LEN,
               ret);
        return -EMSGSIZE;
    }

    return 0;
}

static int send_response(void)
{
    struct spi_buf tx_buffer = {
        .buf = response_tx.bytes,
        .len = sizeof(response_tx.bytes),
    };
    struct spi_buf rx_buffer = {
        .buf = response_discard_rx.bytes,
        .len = sizeof(response_discard_rx.bytes),
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

    memset(response_discard_rx.bytes, 0, sizeof(response_discard_rx.bytes));
    response_result = -EINPROGRESS;
    k_sem_reset(&response_done);

    ret = spi_transceive_cb(
        spis_dev,
        &spis_config,
        &tx_set,
        &rx_set,
        response_complete_callback,
        NULL);
    if (ret < 0) {
        printk("tphip_dwm response_arm_error ret=%d\n", ret);
        return ret;
    }

    ret = gpio_pin_set_dt(&host_ready, 1);
    if (ret != 0) {
        printk("tphip_dwm ready_assert_error ret=%d\n", ret);
        return ret;
    }

    printk("tphip_dwm ready=1 response_armed len=%u\n",
           TPHIP_RESPONSE_TRANSACTION_LEN);

    ret = k_sem_take(&response_done, K_FOREVER);
    (void)gpio_pin_set_dt(&host_ready, 0);
    printk("tphip_dwm ready=0 response_callback_result=%d sem_ret=%d\n",
           response_result,
           ret);

    if (ret != 0) {
        return ret;
    }

    if (response_result != TPHIP_RESPONSE_TRANSACTION_LEN) {
        return response_result < 0 ? response_result : -EMSGSIZE;
    }

    return 0;
}

int main(void)
{
    struct tphip_capabilities capabilities;
    uint32_t transaction_count = 0u;
    int ret;

    printk("\nThin-Pod DWM3001-CDK TPHIP SPIS endpoint\n");
    printk("board=decawave_dwm3001cdk role=gateway_initiator backend=stub\n");
    printk("interface=spi2 mode=0 request_len=%u response_len=%u\n",
           TPHIP_REQUEST_TRANSACTION_LEN,
           TPHIP_RESPONSE_TRANSACTION_LEN);
    printk("pin_map sck=P0.31 copi=P0.27 cipo=P0.07 cs=P0.30 ready=P0.28\n");
    printk("build_id=0x%08x\n", TPHIP_INTERFACE_BUILD_ID);

    if (!device_is_ready(spis_dev)) {
        printk("ERROR: spi2 SPIS device is not ready\n");
        return -ENODEV;
    }

    if (!gpio_is_ready_dt(&host_ready)) {
        printk("ERROR: HOST_READY GPIO is not ready\n");
        return -ENODEV;
    }

    ret = gpio_pin_configure_dt(&host_ready, GPIO_OUTPUT_INACTIVE);
    if (ret != 0) {
        printk("ERROR: HOST_READY configure ret=%d\n", ret);
        return ret;
    }

    initialise_guards();
    tphip_default_capabilities(&capabilities);

    while (1) {
        struct tphip_request_view request;
        enum tphip_validation_result validation;
        enum tphip_status status;
        uint8_t request_opcode;
        uint16_t sequence;
        size_t logical_response_len;

        ret = receive_request();
        if (ret != 0) {
            k_msleep(10);
            continue;
        }

        transaction_count++;
        request_opcode = request_rx.bytes[TPHIP_OFFSET_OPCODE];
        sequence = (uint16_t)request_rx.bytes[TPHIP_OFFSET_SEQUENCE] |
                   ((uint16_t)request_rx.bytes[TPHIP_OFFSET_SEQUENCE + 1u] << 8);

        validation = tphip_validate_request(
            request_rx.bytes,
            sizeof(request_rx.bytes),
            &request);
        status = tphip_status_from_validation(validation);

        printk("tphip_dwm request_rx count=%u sequence=%u opcode=0x%02x validation=%s status=%s guard=%s\n",
               (unsigned int)transaction_count,
               (unsigned int)sequence,
               (unsigned int)request_opcode,
               tphip_validation_name(validation),
               tphip_status_name(status),
               guards_ok() ? "OK" : "FAIL");
        print_bytes("request=", request_rx.bytes, sizeof(request_rx.bytes));

        if (!guards_ok()) {
            status = TPHIP_STATUS_INTERNAL_ERROR;
        }

        logical_response_len = tphip_build_response(
            response_tx.bytes,
            request_opcode,
            sequence,
            status,
            &capabilities);

        printk("tphip_dwm response_ready sequence=%u opcode=0x%02x status=%s logical_len=%u physical_len=%u\n",
               (unsigned int)sequence,
               (unsigned int)response_tx.bytes[TPHIP_OFFSET_OPCODE],
               tphip_status_name(status),
               (unsigned int)logical_response_len,
               TPHIP_RESPONSE_TRANSACTION_LEN);
        print_bytes("response=", response_tx.bytes, sizeof(response_tx.bytes));

        ret = send_response();
        printk("tphip_dwm response_complete ret=%d guard=%s\n",
               ret,
               guards_ok() ? "OK" : "FAIL");

        if (!guards_ok()) {
            printk("FATAL: buffer guard failure\n");
            return -EFAULT;
        }
    }

    return 0;
}