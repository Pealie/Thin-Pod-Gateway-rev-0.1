#ifndef THINPOD_PROTOCOL_H
#define THINPOD_PROTOCOL_H

#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

#define THINPOD_PROTOCOL_VERSION      1u
#define THINPOD_NODE_ID               0x0001u
#define THINPOD_GATEWAY_ID            0x0000u
#define THINPOD_VIB_SAMPLES           32u
#define THINPOD_SAMPLE_RATE_HZ        1600u

#define THINPOD_MSG_POLL              0x01u
#define THINPOD_MSG_VIB_WINDOW        0x10u

typedef struct __attribute__((packed)) {
    uint8_t  version;
    uint8_t  msg_type;
    uint16_t sequence;
    uint16_t src_id;
    uint16_t dst_id;
    uint32_t uptime_ms;
} thinpod_header_t;

typedef struct __attribute__((packed)) {
    thinpod_header_t header;
    uint16_t sample_rate_hz;
    uint16_t sample_count;
    int16_t  accel_mg[THINPOD_VIB_SAMPLES];
    uint32_t checksum;
} thinpod_vibration_window_t;

typedef struct {
    int16_t min_mg;
    int16_t max_mg;
    int32_t mean_mg;
    uint32_t rms_mg;
} thinpod_window_stats_t;

uint32_t thinpod_checksum32(const void *data, size_t len);
void thinpod_make_vibration_window(thinpod_vibration_window_t *pkt,
                                   uint16_t sequence,
                                   uint16_t src_id,
                                   uint16_t dst_id,
                                   uint32_t uptime_ms);
int thinpod_validate_vibration_window(const thinpod_vibration_window_t *pkt);
thinpod_window_stats_t thinpod_window_stats(const thinpod_vibration_window_t *pkt);

#ifdef __cplusplus
}
#endif
#endif
