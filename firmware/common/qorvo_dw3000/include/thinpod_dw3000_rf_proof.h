#ifndef THINPOD_DW3000_RF_PROOF_H
#define THINPOD_DW3000_RF_PROOF_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define THINPOD_RF_PROOF_PAYLOAD_LEN 24U
#define THINPOD_RF_PROOF_FCS_LEN 2U
#define THINPOD_RF_PROOF_AIR_LEN \
    (THINPOD_RF_PROOF_PAYLOAD_LEN + THINPOD_RF_PROOF_FCS_LEN)
#define THINPOD_RF_PROOF_SUITE_FRAMES 20U

/*
 * Initialise and configure the already-proven DW3110 path for a bounded,
 * non-ranging, channel-5 data-frame exchange.
 */
int thinpod_dw3000_rf_prepare(
    const char *role,
    uint16_t role_id);

/* Run indefinitely, emitting a summary after every 20 attempted frames. */
void thinpod_dw3000_rf_run_transmitter(
    const char *role,
    uint16_t src_id,
    uint16_t dst_id);

/* Run indefinitely, emitting a summary after every 20 valid frames. */
void thinpod_dw3000_rf_run_receiver(
    const char *role,
    uint16_t src_id,
    uint16_t dst_id);

#ifdef __cplusplus
}
#endif

#endif
