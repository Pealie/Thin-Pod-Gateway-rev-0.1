#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>
#include "thinpod_protocol.h"
#include "thinpod_dw3000_platform.h"

int main(void)
{
    uint16_t seq = 1;
    thinpod_vibration_window_t window;

    printk("\n\nThin-Pod Node UWB responder stage-1\n");
    printk("node_id=0x%04x role=responder backend=stub\n", THINPOD_NODE_ID);
    printk("dw3110_driver=linked version=%s access=deferred\n",
           thinpod_dw3000_driver_version());
    printk("status=ready waiting_for_gateway_poll\n");

    while (1) {
        thinpod_make_vibration_window(&window, seq, THINPOD_NODE_ID, THINPOD_GATEWAY_ID,
                                      (uint32_t)k_uptime_get_32());

        printk("TPNODE_RESPONDER_READY seq=%u src=0x%04x dst=0x%04x samples=%u rate=%u checksum=0x%08x first_mg=%d\n",
               window.header.sequence, window.header.src_id, window.header.dst_id,
               window.sample_count, window.sample_rate_hz, window.checksum, window.accel_mg[0]);

        seq++;
        k_sleep(K_MSEC(1000));
    }

    return 0;
}
