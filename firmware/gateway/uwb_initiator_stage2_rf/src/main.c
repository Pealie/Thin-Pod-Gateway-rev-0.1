#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>
#include "thinpod_protocol.h"

static void log_gateway_packet(const thinpod_vibration_window_t *window)
{
    thinpod_window_stats_t stats = thinpod_window_stats(window);
    int valid = thinpod_validate_vibration_window(window);

    printk("TPGW_PKT seq=%u src=0x%04x dst=0x%04x uptime_ms=%u valid=%d samples=%u rate=%u first_mg=%d min_mg=%d max_mg=%d mean_mg=%d rms_mg=%u checksum=0x%08x\n",
           window->header.sequence, window->header.src_id, window->header.dst_id,
           window->header.uptime_ms, valid, window->sample_count, window->sample_rate_hz,
           window->accel_mg[0], stats.min_mg, stats.max_mg, stats.mean_mg,
           stats.rms_mg, window->checksum);
}

int main(void)
{
    uint16_t seq = 1;
    thinpod_vibration_window_t rx_window;

    printk("\n\nThin-Pod Gateway UWB initiator stage-1\n");
    printk("gateway_id=0x%04x role=initiator backend=stub\n", THINPOD_GATEWAY_ID);
    printk("packet_log_format=TPGW_PKT key=value\n");

    while (1) {
        printk("TPGW_POLL seq=%u dst=0x%04x\n", seq, THINPOD_NODE_ID);

        thinpod_make_vibration_window(&rx_window, seq, THINPOD_NODE_ID, THINPOD_GATEWAY_ID,
                                      (uint32_t)k_uptime_get_32());

        log_gateway_packet(&rx_window);

        seq++;
        k_sleep(K_MSEC(1000));
    }

    return 0;
}
