#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>

#include "thinpod_dw3000_platform.h"
#include "thinpod_dw3000_rf_proof.h"
#include "thinpod_protocol.h"

int main(void)
{
    int rc;

    printk("\n\n");
    printk("Thin-Pod Node-side DWM3001-CDK one-way DW3110 RF proof\n");
    printk("role=node role_id=0x%04x direction=gateway-to-node proof=one-way-rf-frame-v1\n",
           THINPOD_NODE_ID);
    printk("driver=%s\n", thinpod_dw3000_driver_version());

    rc = thinpod_dw3000_rf_prepare(
        "node",
        THINPOD_NODE_ID);

    if (rc != 0) {
        printk("TPRF_FATAL role=node stage=prepare rc=%d result=FAIL\n",
               rc);
        while (1) {
            k_sleep(K_SECONDS(5));
        }
    }

    printk("TPRF_READY role=node mode=rx src=0x%04x dst=0x%04x hardware_timeout_uus=900000 suite_frames=%u\n",
           THINPOD_GATEWAY_ID,
           THINPOD_NODE_ID,
           THINPOD_RF_PROOF_SUITE_FRAMES);

    thinpod_dw3000_rf_run_receiver(
        "node",
        THINPOD_GATEWAY_ID,
        THINPOD_NODE_ID);

    return 0;
}
