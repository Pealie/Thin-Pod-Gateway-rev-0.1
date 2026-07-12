#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>

#include "thinpod_protocol.h"
#include "thinpod_dw3000_identity.h"
#include "thinpod_dw3000_platform.h"

int main(void)
{
    struct thinpod_dw3000_identity_result result;

    printk("\n\n");
    printk("Thin-Pod Node-side DWM3001-CDK DW3110 identity proof\n");
    printk("role=node role_id=0x%04x proof=identity-initialise-v1\n",
           THINPOD_NODE_ID);
    printk("driver=%s\n",
           thinpod_dw3000_driver_version());

    (void)thinpod_dw3000_identity_proof(
        "node",
        THINPOD_NODE_ID,
        &result);

    while (1) {
        k_sleep(K_SECONDS(5));

        thinpod_dw3000_identity_print_summary(
            "node",
            THINPOD_NODE_ID,
            &result);
    }

    return 0;
}
