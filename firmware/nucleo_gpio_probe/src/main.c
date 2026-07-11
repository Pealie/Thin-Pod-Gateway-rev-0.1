#include <errno.h>
#include <stdint.h>

#include <zephyr/device.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>

#define PROBE_SLEEP_MS 1000
#define TOGGLE_HOLD_MS 250

static const struct gpio_dt_spec dwm_reset =
	GPIO_DT_SPEC_GET(DT_ALIAS(dwm_reset), gpios);

static const struct gpio_dt_spec dwm_cs =
	GPIO_DT_SPEC_GET(DT_ALIAS(dwm_cs), gpios);

static const struct gpio_dt_spec dwm_irq =
	GPIO_DT_SPEC_GET(DT_ALIAS(dwm_irq), gpios);

static int check_gpio_ready(const struct gpio_dt_spec *spec, const char *name)
{
	if (!device_is_ready(spec->port)) {
		printk("ERROR: %s GPIO device not ready\n", name);
		return -ENODEV;
	}

	printk("%s ready: controller=%s pin=%u\n",
	       name, spec->port->name, spec->pin);

	return 0;
}

int main(void)
{
	int irq_level;
	uint32_t heartbeat = 0U;

	printk("\nThin-Pod Gateway rev0.1 NUCLEO GPIO probe\n");
	printk("Board: nucleo_n657x0_q\n");
	printk("Purpose: RESET / CS / IRQ pin-path confirmation only\n");
	printk("No SPI. No DW3110 register access. No UWB RF.\n\n");

	if (check_gpio_ready(&dwm_reset, "DWM_RESET / PD0") != 0) {
		return 0;
	}

	if (check_gpio_ready(&dwm_cs, "DWM_CS / PA3") != 0) {
		return 0;
	}

	if (check_gpio_ready(&dwm_irq, "DWM_IRQ / PB9") != 0) {
		return 0;
	}

	if (gpio_pin_configure_dt(&dwm_reset, GPIO_OUTPUT_LOW) != 0) {
		printk("ERROR: failed to configure DWM_RESET / PD0\n");
		return 0;
	}

	if (gpio_pin_configure_dt(&dwm_cs, GPIO_OUTPUT_LOW) != 0) {
		printk("ERROR: failed to configure DWM_CS / PA3\n");
		return 0;
	}

	if (gpio_pin_configure_dt(&dwm_irq, GPIO_INPUT) != 0) {
		printk("ERROR: failed to configure DWM_IRQ / PB9\n");
		return 0;
	}

	printk("GPIO configuration complete\n");
	printk("Probe pattern: RESET and CS toggle low/high; IRQ is sampled\n\n");

	while (1) {
		gpio_pin_set_raw(dwm_reset.port, dwm_reset.pin, 0);
		gpio_pin_set_raw(dwm_cs.port, dwm_cs.pin, 0);
		k_msleep(TOGGLE_HOLD_MS);

		irq_level = gpio_pin_get_raw(dwm_irq.port, dwm_irq.pin);
		printk("probe=%u phase=LOW reset=0 cs=0 irq=%d\n",
		       heartbeat, irq_level);

		gpio_pin_set_raw(dwm_reset.port, dwm_reset.pin, 1);
		gpio_pin_set_raw(dwm_cs.port, dwm_cs.pin, 1);
		k_msleep(TOGGLE_HOLD_MS);

		irq_level = gpio_pin_get_raw(dwm_irq.port, dwm_irq.pin);
		printk("probe=%u phase=HIGH reset=1 cs=1 irq=%d\n",
		       heartbeat, irq_level);

		heartbeat++;
		k_msleep(PROBE_SLEEP_MS);
	}

	return 0;
}
