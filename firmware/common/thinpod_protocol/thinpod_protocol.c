#include "thinpod_protocol.h"
#include <string.h>

uint32_t thinpod_checksum32(const void *data, size_t len)
{
    const uint8_t *p = (const uint8_t *)data;
    uint32_t sum = 0x54485044u;
    for (size_t i = 0; i < len; i++) {
        sum = (sum << 5) | (sum >> 27);
        sum ^= p[i];
        sum += 0x9E3779B9u;
    }
    return sum;
}

void thinpod_make_vibration_window(thinpod_vibration_window_t *pkt,
                                   uint16_t sequence,
                                   uint16_t src_id,
                                   uint16_t dst_id,
                                   uint32_t uptime_ms)
{
    memset(pkt, 0, sizeof(*pkt));
    pkt->header.version = THINPOD_PROTOCOL_VERSION;
    pkt->header.msg_type = THINPOD_MSG_VIB_WINDOW;
    pkt->header.sequence = sequence;
    pkt->header.src_id = src_id;
    pkt->header.dst_id = dst_id;
    pkt->header.uptime_ms = uptime_ms;
    pkt->sample_rate_hz = THINPOD_SAMPLE_RATE_HZ;
    pkt->sample_count = THINPOD_VIB_SAMPLES;

    for (uint16_t i = 0; i < THINPOD_VIB_SAMPLES; i++) {
        int16_t carrier = (int16_t)(((sequence * 7u) + (i * 19u)) % 140u);
        int16_t alternating = (i & 1u) ? -22 : 22;
        pkt->accel_mg[i] = (int16_t)(carrier - 70 + alternating);
    }

    pkt->checksum = 0u;
    pkt->checksum = thinpod_checksum32(pkt, sizeof(*pkt));
}

int thinpod_validate_vibration_window(const thinpod_vibration_window_t *pkt)
{
    thinpod_vibration_window_t tmp = *pkt;
    uint32_t expected = pkt->checksum;
    tmp.checksum = 0u;

    if (pkt->header.version != THINPOD_PROTOCOL_VERSION) return -1;
    if (pkt->header.msg_type != THINPOD_MSG_VIB_WINDOW) return -2;
    if (pkt->sample_count != THINPOD_VIB_SAMPLES) return -3;
    return (thinpod_checksum32(&tmp, sizeof(tmp)) == expected) ? 0 : -4;
}

static uint32_t isqrt_u64(uint64_t x)
{
    uint64_t op = x, res = 0, one = (uint64_t)1 << 62;
    while (one > op) one >>= 2;
    while (one != 0) {
        if (op >= res + one) {
            op -= res + one;
            res = (res >> 1) + one;
        } else {
            res >>= 1;
        }
        one >>= 2;
    }
    return (uint32_t)res;
}

thinpod_window_stats_t thinpod_window_stats(const thinpod_vibration_window_t *pkt)
{
    thinpod_window_stats_t stats = {0};
    int32_t sum = 0;
    uint64_t sum_sq = 0;
    stats.min_mg = pkt->accel_mg[0];
    stats.max_mg = pkt->accel_mg[0];

    for (uint16_t i = 0; i < THINPOD_VIB_SAMPLES; i++) {
        int16_t v = pkt->accel_mg[i];
        if (v < stats.min_mg) stats.min_mg = v;
        if (v > stats.max_mg) stats.max_mg = v;
        sum += v;
        sum_sq += (uint64_t)((int32_t)v * (int32_t)v);
    }

    stats.mean_mg = sum / (int32_t)THINPOD_VIB_SAMPLES;
    stats.rms_mg = isqrt_u64(sum_sq / THINPOD_VIB_SAMPLES);
    return stats;
}
