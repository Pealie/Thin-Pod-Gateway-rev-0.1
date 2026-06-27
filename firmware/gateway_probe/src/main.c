#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/spi.h>
#include <zephyr/sys/printk.h>

#define PROBE_SLEEP_MS 500

/*
 * Thin-Pod Gateway rev 0.1 minimal SPI/GPIO probe.
 *
 * This file intentionally avoids DW3110 register-level behaviour.
 * The objective is physical and firmware-level path verification:
 * reset toggle, chip-select toggle, IRQ read, and basic SPI transmit.
 */

int main(void)
{
    printk("\nThin-Pod Gateway rev0.1 SPI5/GPIO probe\n");
    printk("Purpose: verify DWM3001-CDK GPIO and SPI signal paths\n");

    printk("NOTE: GPIO/SPI device bindings still need to be matched to the final Zephyr board overlay.\n");
    printk("Probe skeleton running.\n");

    while (1) {
        printk("Gateway probe heartbeat\n");
        k_msleep(PROBE_SLEEP_MS);
    }

    return 0;
}