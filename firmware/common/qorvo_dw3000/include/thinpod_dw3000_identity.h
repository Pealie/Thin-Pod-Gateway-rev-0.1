#ifndef THINPOD_DW3000_IDENTITY_H
#define THINPOD_DW3000_IDENTITY_H

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

struct thinpod_dw3000_identity_result {
    int final_rc;
    const char *failure_stage;

    int platform_rc;
    int reset_rc;
    int probe_rc;
    int initialise_rc;

    int irq_before;
    int irq_after;

    int32_t api_version;
    uint8_t idle_rc;

    uint32_t expected_device_id;
    uint32_t device_id_mask;
    uint32_t device_id_slow;
    uint32_t device_id_after_initialise;
    uint32_t device_id_fast;

    bool device_id_match;
    bool post_initialise_match;
    bool fast_device_id_match;
    bool device_id_stable;

    uint32_t part_id;
    uint64_t lot_id;
    uint8_t otp_revision;
    uint8_t reference_voltage;
    uint8_t reference_temperature;
};

int thinpod_dw3000_identity_proof(
    const char *role,
    uint16_t role_id,
    struct thinpod_dw3000_identity_result *result);

void thinpod_dw3000_identity_print_summary(
    const char *role,
    uint16_t role_id,
    const struct thinpod_dw3000_identity_result *result);

#ifdef __cplusplus
}
#endif

#endif
