#include "thinpod_dw3000_rf_proof.h"

#include <errno.h>
#include <stdbool.h>
#include <stddef.h>
#include <string.h>

#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>

#include "deca_device_api.h"

#if __has_include("deca_regs.h")
#include "deca_regs.h"
#elif __has_include("dw3000_deca_regs.h")
#include "dw3000_deca_regs.h"
#else
#error "No DW3000 register-definition header found"
#endif
#include "thinpod_dw3000_identity.h"
#include "thinpod_dw3000_platform.h"
#include "thinpod_protocol.h"

#define THINPOD_RF_TX_INTERVAL_MS 500U
#define THINPOD_RF_TX_STATUS_TIMEOUT_MS 100U
#define THINPOD_RF_RX_HARDWARE_TIMEOUT_UUS 900000U
#define THINPOD_RF_RX_HOST_TIMEOUT_MS 1200U
#define THINPOD_RF_STATUS_POLL_US 50U

#define THINPOD_RF_MAGIC_0 'T'
#define THINPOD_RF_MAGIC_1 'P'
#define THINPOD_RF_MAGIC_2 'R'
#define THINPOD_RF_MAGIC_3 'F'
#define THINPOD_RF_VERSION 1U
#define THINPOD_RF_FRAME_TYPE 1U

#define THINPOD_RF_OFF_MAGIC 0U
#define THINPOD_RF_OFF_VERSION 4U
#define THINPOD_RF_OFF_TYPE 5U
#define THINPOD_RF_OFF_SEQUENCE 6U
#define THINPOD_RF_OFF_SRC_ID 8U
#define THINPOD_RF_OFF_DST_ID 10U
#define THINPOD_RF_OFF_UPTIME_MS 12U
#define THINPOD_RF_OFF_PATTERN 16U
#define THINPOD_RF_OFF_CHECKSUM 20U
#define THINPOD_RF_CHECKSUM_INPUT_LEN 20U

#define THINPOD_RF_RX_EVENT_MASK \
    (SYS_STATUS_RXFCG_BIT_MASK | SYS_STATUS_ALL_RX_TO | \
     SYS_STATUS_ALL_RX_ERR)

#define THINPOD_RF_RX_CLEAR_MASK \
    (SYS_STATUS_RXFCG_BIT_MASK | SYS_STATUS_ALL_RX_TO | \
     SYS_STATUS_ALL_RX_ERR)

static dwt_config_t thinpod_rf_config = {
    .chan = 5,
    .txPreambLength = DWT_PLEN_128,
    .rxPAC = DWT_PAC8,
    .txCode = 9,
    .rxCode = 9,
    .sfdType = DWT_SFD_DW_8,
    .dataRate = DWT_BR_6M8,
    .phrMode = DWT_PHRMODE_STD,
    .phrRate = DWT_PHRRATE_STD,
    .sfdTO = (129 + 8 - 8),
    .stsMode = DWT_STS_MODE_OFF,
    .stsLength = DWT_STS_LEN_64,
    .pdoaMode = DWT_PDOA_M0
};

static dwt_txconfig_t thinpod_rf_tx_config = {
    .PGdly = 0x34,
    .power = 0xFDFDFDFDUL,
    .PGcount = 0U
};

static void put_le16(uint8_t *buffer, uint16_t value)
{
    buffer[0] = (uint8_t)(value & 0xFFU);
    buffer[1] = (uint8_t)(value >> 8U);
}

static void put_le32(uint8_t *buffer, uint32_t value)
{
    buffer[0] = (uint8_t)(value & 0xFFU);
    buffer[1] = (uint8_t)((value >> 8U) & 0xFFU);
    buffer[2] = (uint8_t)((value >> 16U) & 0xFFU);
    buffer[3] = (uint8_t)((value >> 24U) & 0xFFU);
}

static uint16_t get_le16(const uint8_t *buffer)
{
    return (uint16_t)buffer[0] |
           ((uint16_t)buffer[1] << 8U);
}

static uint32_t get_le32(const uint8_t *buffer)
{
    return (uint32_t)buffer[0] |
           ((uint32_t)buffer[1] << 8U) |
           ((uint32_t)buffer[2] << 16U) |
           ((uint32_t)buffer[3] << 24U);
}

static void payload_to_hex(
    const uint8_t *payload,
    size_t payload_length,
    char *hex,
    size_t hex_length)
{
    static const char digits[] = "0123456789abcdef";
    size_t i;

    if ((payload == NULL) || (hex == NULL) ||
        (hex_length < ((payload_length * 2U) + 1U))) {
        return;
    }

    for (i = 0U; i < payload_length; i++) {
        hex[i * 2U] = digits[payload[i] >> 4U];
        hex[(i * 2U) + 1U] = digits[payload[i] & 0x0FU];
    }

    hex[payload_length * 2U] = '\0';
}

static void build_payload(
    uint8_t payload[THINPOD_RF_PROOF_PAYLOAD_LEN],
    uint16_t sequence,
    uint16_t src_id,
    uint16_t dst_id,
    uint32_t uptime_ms)
{
    uint32_t checksum;

    memset(payload, 0, THINPOD_RF_PROOF_PAYLOAD_LEN);

    payload[THINPOD_RF_OFF_MAGIC + 0U] = THINPOD_RF_MAGIC_0;
    payload[THINPOD_RF_OFF_MAGIC + 1U] = THINPOD_RF_MAGIC_1;
    payload[THINPOD_RF_OFF_MAGIC + 2U] = THINPOD_RF_MAGIC_2;
    payload[THINPOD_RF_OFF_MAGIC + 3U] = THINPOD_RF_MAGIC_3;
    payload[THINPOD_RF_OFF_VERSION] = THINPOD_RF_VERSION;
    payload[THINPOD_RF_OFF_TYPE] = THINPOD_RF_FRAME_TYPE;

    put_le16(&payload[THINPOD_RF_OFF_SEQUENCE], sequence);
    put_le16(&payload[THINPOD_RF_OFF_SRC_ID], src_id);
    put_le16(&payload[THINPOD_RF_OFF_DST_ID], dst_id);
    put_le32(&payload[THINPOD_RF_OFF_UPTIME_MS], uptime_ms);

    payload[THINPOD_RF_OFF_PATTERN + 0U] = 0xA5U;
    payload[THINPOD_RF_OFF_PATTERN + 1U] = 0x5AU;
    payload[THINPOD_RF_OFF_PATTERN + 2U] = 0xC3U;
    payload[THINPOD_RF_OFF_PATTERN + 3U] = 0x3CU;

    checksum = thinpod_checksum32(
        payload,
        THINPOD_RF_CHECKSUM_INPUT_LEN);

    put_le32(&payload[THINPOD_RF_OFF_CHECKSUM], checksum);
}

static bool payload_is_valid(
    const uint8_t payload[THINPOD_RF_PROOF_PAYLOAD_LEN],
    uint16_t expected_src_id,
    uint16_t expected_dst_id,
    uint32_t *observed_checksum,
    uint32_t *calculated_checksum)
{
    uint32_t observed;
    uint32_t calculated;

    observed = get_le32(&payload[THINPOD_RF_OFF_CHECKSUM]);
    calculated = thinpod_checksum32(
        payload,
        THINPOD_RF_CHECKSUM_INPUT_LEN);

    if (observed_checksum != NULL) {
        *observed_checksum = observed;
    }

    if (calculated_checksum != NULL) {
        *calculated_checksum = calculated;
    }

    return
        (payload[THINPOD_RF_OFF_MAGIC + 0U] == THINPOD_RF_MAGIC_0) &&
        (payload[THINPOD_RF_OFF_MAGIC + 1U] == THINPOD_RF_MAGIC_1) &&
        (payload[THINPOD_RF_OFF_MAGIC + 2U] == THINPOD_RF_MAGIC_2) &&
        (payload[THINPOD_RF_OFF_MAGIC + 3U] == THINPOD_RF_MAGIC_3) &&
        (payload[THINPOD_RF_OFF_VERSION] == THINPOD_RF_VERSION) &&
        (payload[THINPOD_RF_OFF_TYPE] == THINPOD_RF_FRAME_TYPE) &&
        (get_le16(&payload[THINPOD_RF_OFF_SRC_ID]) == expected_src_id) &&
        (get_le16(&payload[THINPOD_RF_OFF_DST_ID]) == expected_dst_id) &&
        (payload[THINPOD_RF_OFF_PATTERN + 0U] == 0xA5U) &&
        (payload[THINPOD_RF_OFF_PATTERN + 1U] == 0x5AU) &&
        (payload[THINPOD_RF_OFF_PATTERN + 2U] == 0xC3U) &&
        (payload[THINPOD_RF_OFF_PATTERN + 3U] == 0x3CU) &&
        (observed == calculated);
}

static int wait_for_status(
    uint32_t event_mask,
    uint32_t timeout_ms,
    uint32_t *status)
{
    uint32_t start_ms;
    uint32_t current_status;

    if (status == NULL) {
        return -EINVAL;
    }

    start_ms = k_uptime_get_32();

    do {
        current_status = dwt_read32bitreg(SYS_STATUS_ID);

        if ((current_status & event_mask) != 0U) {
            *status = current_status;
            return 0;
        }

        k_busy_wait(THINPOD_RF_STATUS_POLL_US);
    } while ((uint32_t)(k_uptime_get_32() - start_ms) < timeout_ms);

    *status = dwt_read32bitreg(SYS_STATUS_ID);
    return -ETIMEDOUT;
}

static void clear_rx_events(void)
{
    dwt_write32bitreg(
        SYS_STATUS_ID,
        (uint32_t)THINPOD_RF_RX_CLEAR_MASK);
}

int thinpod_dw3000_rf_prepare(
    const char *role,
    uint16_t role_id)
{
    struct thinpod_dw3000_identity_result identity_result;
    int rc;

    if (role == NULL) {
        return -EINVAL;
    }

    rc = thinpod_dw3000_identity_proof(
        role,
        role_id,
        &identity_result);

    if (rc != 0) {
        printk("TPRF_INIT role=%s stage=identity rc=%d result=FAIL\n",
               role,
               rc);
        return rc;
    }

    dwt_forcetrxoff();
    clear_rx_events();
    dwt_write32bitreg(SYS_STATUS_ID, SYS_STATUS_TXFRS_BIT_MASK);

    rc = dwt_configure(&thinpod_rf_config);

    printk("TPRF_INIT role=%s stage=configure rc=%d result=%s\n",
           role,
           rc,
           (rc == DWT_SUCCESS) ? "PASS" : "FAIL");

    if (rc != DWT_SUCCESS) {
        return -EIO;
    }

    dwt_configuretxrf(&thinpod_rf_tx_config);
    dwt_setrxtimeout(0U);

    printk("TPRF_PHY role=%s channel=5 preamble=128 pac=8 tx_code=9 rx_code=9 sfd=dw8 data_rate=6m8 phr=standard sts=off pdoa=off pg_delay=0x34 tx_power=0xfdfdfdfd payload_len=%u air_len=%u result=PASS\n",
           role,
           THINPOD_RF_PROOF_PAYLOAD_LEN,
           THINPOD_RF_PROOF_AIR_LEN);

    return 0;
}

void thinpod_dw3000_rf_run_transmitter(
    const char *role,
    uint16_t src_id,
    uint16_t dst_id)
{
    uint8_t payload[THINPOD_RF_PROOF_PAYLOAD_LEN];
    char payload_hex[(THINPOD_RF_PROOF_PAYLOAD_LEN * 2U) + 1U];
    uint16_t sequence = 1U;
    uint16_t suite_first_sequence = 1U;
    uint32_t suite_number = 1U;
    uint32_t suite_passes = 0U;
    uint32_t suite_failures = 0U;
    uint32_t suite_attempts = 0U;

    while (true) {
        uint32_t status = 0U;
        uint32_t checksum;
        int irq_before;
        int irq_event;
        int irq_after_clear;
        int write_rc;
        int start_rc;
        int wait_rc;
        bool pass;

        build_payload(
            payload,
            sequence,
            src_id,
            dst_id,
            k_uptime_get_32());

        payload_to_hex(
            payload,
            sizeof(payload),
            payload_hex,
            sizeof(payload_hex));

        checksum = get_le32(&payload[THINPOD_RF_OFF_CHECKSUM]);
        irq_before = thinpod_dw3000_irq_level();

        dwt_forcetrxoff();
        dwt_write32bitreg(SYS_STATUS_ID, SYS_STATUS_TXFRS_BIT_MASK);

        write_rc = dwt_writetxdata(
            THINPOD_RF_PROOF_PAYLOAD_LEN,
            payload,
            0U);

        dwt_writetxfctrl(
            THINPOD_RF_PROOF_AIR_LEN,
            0U,
            0U);

        start_rc = (write_rc == DWT_SUCCESS) ?
            dwt_starttx(DWT_START_TX_IMMEDIATE) : DWT_ERROR;

        wait_rc = (start_rc == DWT_SUCCESS) ?
            wait_for_status(
                SYS_STATUS_TXFRS_BIT_MASK,
                THINPOD_RF_TX_STATUS_TIMEOUT_MS,
                &status) : -EIO;

        irq_event = thinpod_dw3000_irq_level();
        pass = (write_rc == DWT_SUCCESS) &&
               (start_rc == DWT_SUCCESS) &&
               (wait_rc == 0) &&
               ((status & SYS_STATUS_TXFRS_BIT_MASK) != 0U);

        if ((status & SYS_STATUS_TXFRS_BIT_MASK) != 0U) {
            dwt_write32bitreg(
                SYS_STATUS_ID,
                SYS_STATUS_TXFRS_BIT_MASK);
        }

        irq_after_clear = thinpod_dw3000_irq_level();

        printk("TPRF_TX role=%s seq=%u src=0x%04x dst=0x%04x payload_len=%u air_len=%u checksum=0x%08x write_rc=%d start_rc=%d wait_rc=%d status=0x%08x irq_before=%d irq_event=%d irq_after_clear=%d payload=%s result=%s\n",
               role,
               sequence,
               src_id,
               dst_id,
               THINPOD_RF_PROOF_PAYLOAD_LEN,
               THINPOD_RF_PROOF_AIR_LEN,
               (unsigned int)checksum,
               write_rc,
               start_rc,
               wait_rc,
               (unsigned int)status,
               irq_before,
               irq_event,
               irq_after_clear,
               payload_hex,
               pass ? "PASS" : "FAIL");

        suite_attempts++;
        if (pass) {
            suite_passes++;
        } else {
            suite_failures++;
        }

        if (suite_attempts == THINPOD_RF_PROOF_SUITE_FRAMES) {
            printk("TPRF_TX_SUMMARY role=%s suite=%u attempted=%u pass=%u fail=%u first_seq=%u last_seq=%u result=%s\n",
                   role,
                   (unsigned int)suite_number,
                   (unsigned int)suite_attempts,
                   (unsigned int)suite_passes,
                   (unsigned int)suite_failures,
                   suite_first_sequence,
                   sequence,
                   (suite_failures == 0U) ? "PASS" : "FAIL");

            suite_number++;
            suite_attempts = 0U;
            suite_passes = 0U;
            suite_failures = 0U;
            suite_first_sequence = (uint16_t)(sequence + 1U);
        }

        sequence++;
        k_msleep(THINPOD_RF_TX_INTERVAL_MS);
    }
}

void thinpod_dw3000_rf_run_receiver(
    const char *role,
    uint16_t src_id,
    uint16_t dst_id)
{
    uint8_t payload[THINPOD_RF_PROOF_PAYLOAD_LEN];
    char payload_hex[(THINPOD_RF_PROOF_PAYLOAD_LEN * 2U) + 1U];
    uint32_t suite_number = 1U;
    uint32_t suite_valid = 0U;
    uint32_t suite_invalid = 0U;
    uint32_t suite_timeouts = 0U;
    uint32_t suite_errors = 0U;
    uint32_t suite_gaps = 0U;
    uint32_t suite_duplicates = 0U;
    uint16_t suite_first_sequence = 0U;
    uint16_t suite_last_sequence = 0U;
    uint16_t expected_sequence = 0U;
    bool exchange_started = false;

    while (true) {
        uint32_t status = 0U;
        uint32_t frame_length;
        uint32_t payload_length;
        uint32_t observed_checksum = 0U;
        uint32_t calculated_checksum = 0U;
        uint16_t sequence = 0U;
        int irq_before;
        int irq_event;
        int irq_after_clear;
        int enable_rc;
        int wait_rc;
        bool length_valid;
        bool payload_valid = false;
        bool sequence_valid = false;
        const char *event_name;
        const char *result_name;

        memset(payload, 0, sizeof(payload));
        payload_hex[0] = '\0';

        dwt_forcetrxoff();
        clear_rx_events();
        dwt_setrxtimeout(THINPOD_RF_RX_HARDWARE_TIMEOUT_UUS);

        irq_before = thinpod_dw3000_irq_level();
        enable_rc = dwt_rxenable(DWT_START_RX_IMMEDIATE);
        wait_rc = (enable_rc == DWT_SUCCESS) ?
            wait_for_status(
                (uint32_t)THINPOD_RF_RX_EVENT_MASK,
                THINPOD_RF_RX_HOST_TIMEOUT_MS,
                &status) : -EIO;
        irq_event = thinpod_dw3000_irq_level();

        frame_length = 0U;
        payload_length = 0U;
        length_valid = false;
        event_name = "unknown";
        result_name = "FAIL";

        if ((wait_rc == 0) &&
            ((status & SYS_STATUS_RXFCG_BIT_MASK) != 0U)) {

            event_name = "good_frame";
            frame_length = dwt_read32bitreg(RX_FINFO_ID) &
                           RX_FINFO_RXFLEN_BIT_MASK;

            length_valid =
                (frame_length == THINPOD_RF_PROOF_AIR_LEN);

            if (frame_length >= THINPOD_RF_PROOF_FCS_LEN) {
                payload_length = frame_length -
                                 THINPOD_RF_PROOF_FCS_LEN;
            }

            if (length_valid) {
                dwt_readrxdata(
                    payload,
                    THINPOD_RF_PROOF_PAYLOAD_LEN,
                    0U);

                payload_to_hex(
                    payload,
                    sizeof(payload),
                    payload_hex,
                    sizeof(payload_hex));

                payload_valid = payload_is_valid(
                    payload,
                    src_id,
                    dst_id,
                    &observed_checksum,
                    &calculated_checksum);

                sequence = get_le16(
                    &payload[THINPOD_RF_OFF_SEQUENCE]);

                if (payload_valid) {
                    if (!exchange_started) {
                        exchange_started = true;
                        suite_first_sequence = sequence;
                        expected_sequence = sequence;
                    }

                    if (sequence == expected_sequence) {
                        sequence_valid = true;
                    } else if (sequence ==
                               (uint16_t)(expected_sequence - 1U)) {
                        suite_duplicates++;
                    } else {
                        suite_gaps++;
                    }

                    expected_sequence = (uint16_t)(sequence + 1U);
                    suite_last_sequence = sequence;
                }
            }

            dwt_write32bitreg(
                SYS_STATUS_ID,
                SYS_STATUS_RXFCG_BIT_MASK);

            if (payload_valid && sequence_valid) {
                suite_valid++;
                result_name = "PASS";
            } else {
                suite_invalid++;
            }
        } else if ((wait_rc == 0) &&
                   ((status & SYS_STATUS_ALL_RX_TO) != 0U)) {
            event_name = "rx_timeout";
            if (exchange_started) {
                suite_timeouts++;
            }
        } else if ((wait_rc == 0) &&
                   ((status & SYS_STATUS_ALL_RX_ERR) != 0U)) {
            event_name = "rx_error";
            if (exchange_started) {
                suite_errors++;
            }
        } else if (enable_rc != DWT_SUCCESS) {
            event_name = "rx_enable_error";
            if (exchange_started) {
                suite_errors++;
            }
        } else {
            event_name = "host_timeout";
            if (exchange_started) {
                suite_timeouts++;
            }
        }

        if ((status & (uint32_t)THINPOD_RF_RX_CLEAR_MASK) != 0U) {
            clear_rx_events();
        }

        if ((wait_rc != 0) ||
            ((status & (SYS_STATUS_ALL_RX_TO |
                        SYS_STATUS_ALL_RX_ERR)) != 0U)) {
            dwt_forcetrxoff();
            dwt_rxreset();
        }

        irq_after_clear = thinpod_dw3000_irq_level();

        if (!exchange_started &&
            (strcmp(event_name, "good_frame") != 0)) {
            printk("TPRF_RX_WAIT role=%s event=%s enable_rc=%d wait_rc=%d status=0x%08x irq_before=%d irq_event=%d irq_after_clear=%d\n",
                   role,
                   event_name,
                   enable_rc,
                   wait_rc,
                   (unsigned int)status,
                   irq_before,
                   irq_event,
                   irq_after_clear);
            continue;
        }

        printk("TPRF_RX role=%s event=%s seq=%u src=0x%04x dst=0x%04x frame_len=%u payload_len=%u expected_air_len=%u length_valid=%u payload_valid=%u sequence_valid=%u checksum_observed=0x%08x checksum_calculated=0x%08x enable_rc=%d wait_rc=%d status=0x%08x irq_before=%d irq_event=%d irq_after_clear=%d payload=%s result=%s\n",
               role,
               event_name,
               sequence,
               src_id,
               dst_id,
               (unsigned int)frame_length,
               (unsigned int)payload_length,
               THINPOD_RF_PROOF_AIR_LEN,
               length_valid ? 1U : 0U,
               payload_valid ? 1U : 0U,
               sequence_valid ? 1U : 0U,
               (unsigned int)observed_checksum,
               (unsigned int)calculated_checksum,
               enable_rc,
               wait_rc,
               (unsigned int)status,
               irq_before,
               irq_event,
               irq_after_clear,
               payload_hex,
               result_name);

        if (suite_valid == THINPOD_RF_PROOF_SUITE_FRAMES) {
            bool suite_pass =
                (suite_invalid == 0U) &&
                (suite_timeouts == 0U) &&
                (suite_errors == 0U) &&
                (suite_gaps == 0U) &&
                (suite_duplicates == 0U);

            printk("TPRF_RX_SUMMARY role=%s suite=%u valid=%u invalid=%u timeouts=%u errors=%u gaps=%u duplicates=%u first_seq=%u last_seq=%u result=%s\n",
                   role,
                   (unsigned int)suite_number,
                   (unsigned int)suite_valid,
                   (unsigned int)suite_invalid,
                   (unsigned int)suite_timeouts,
                   (unsigned int)suite_errors,
                   (unsigned int)suite_gaps,
                   (unsigned int)suite_duplicates,
                   suite_first_sequence,
                   suite_last_sequence,
                   suite_pass ? "PASS" : "FAIL");

            suite_number++;
            suite_valid = 0U;
            suite_invalid = 0U;
            suite_timeouts = 0U;
            suite_errors = 0U;
            suite_gaps = 0U;
            suite_duplicates = 0U;
            suite_first_sequence = expected_sequence;
        }
    }
}
