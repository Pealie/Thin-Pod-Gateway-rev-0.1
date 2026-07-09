#include <errno.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include <zephyr/device.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/spi.h>
#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>

#define SPI5_FREQUENCY_HZ 1000000U

#define PROBE_SLEEP_MS 2000
#define INTER_VARIANT_SLEEP_MS 200

#define RESET_LOW_MS 10
#define RESET_SETTLE_MS 500

#define CS_SETUP_US 10
#define CS_HOLD_US 10
#define CS_WAKE_US 700
#define CS_WAKE_SETTLE_MS 2

#define DW3110_DEV_ID_REG 0x00U
#define DW3110_DEV_ID_LEN 4U
#define DW3110_EXPECTED_DEV_ID 0xDECA0302U

#define MAX_PROBE_LEN 8U

static const struct device *const spi5_dev = DEVICE_DT_GET(DT_NODELABEL(spi5));

static const struct gpio_dt_spec dwm_reset =
	GPIO_DT_SPEC_GET(DT_ALIAS(dwm_reset), gpios);

static const struct gpio_dt_spec dwm_cs =
	GPIO_DT_SPEC_GET(DT_ALIAS(dwm_cs), gpios);

static const struct gpio_dt_spec dwm_irq =
	GPIO_DT_SPEC_GET(DT_ALIAS(dwm_irq), gpios);

struct dw3110_probe_variant {
	const char *name;
	size_t len;
	size_t nominal_data_offset;
	uint8_t tx[MAX_PROBE_LEN];
};

static const struct dw3110_probe_variant probe_variants[] = {
	/*
	 * DEV_ID / register 0x00 minimal read.
	 * One zero header byte, followed by four read clocks.
	 */
	{
		.name = "v0_minimal_1_header_4_read",
		.len = 5U,
		.nominal_data_offset = 1U,
		.tx = { 0x00, 0x00, 0x00, 0x00, 0x00 },
	},

	/*
	 * Diagnostic form matching the previous first probe: six zero bytes.
	 * This also clocks sixteen zero bits at the start of the transfer.
	 */
	{
		.name = "v1_six_zero_clocks",
		.len = 6U,
		.nominal_data_offset = 2U,
		.tx = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
	},

	/*
	 * Experimental two-byte subaddress-style framing:
	 * register 0x00, offset 0x00, then four read clocks.
	 */
	{
		.name = "v2_subaddr_2_header_4_read",
		.len = 6U,
		.nominal_data_offset = 2U,
		.tx = { 0x40, 0x00, 0x00, 0x00, 0x00, 0x00 },
	},

	/*
	 * Experimental extended three-byte header-style framing:
	 * register 0x00, extended offset 0x0000, then four read clocks.
	 */
	{
		.name = "v3_ext_3_header_4_read",
		.len = 7U,
		.nominal_data_offset = 3U,
		.tx = { 0x60, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
	},
};

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

static int dwm_reset_low_then_release(void)
{
	int ret;
	int reset_level;

	printk("Applying DWM reset pulse on PD0: drive low, then release high-Z\n");

	ret = gpio_pin_configure(dwm_reset.port, dwm_reset.pin, GPIO_OUTPUT);
	if (ret != 0) {
		printk("ERROR: failed to configure DWM_RESET / PD0 as output ret=%d\n", ret);
		return ret;
	}

	gpio_pin_set_raw(dwm_reset.port, dwm_reset.pin, 0);
	k_msleep(RESET_LOW_MS);

	ret = gpio_pin_configure(dwm_reset.port, dwm_reset.pin, GPIO_INPUT);
	if (ret != 0) {
		printk("ERROR: failed to release DWM_RESET / PD0 as input ret=%d\n", ret);
		return ret;
	}

	k_msleep(RESET_SETTLE_MS);

	reset_level = gpio_pin_get_raw(dwm_reset.port, dwm_reset.pin);
	printk("DWM reset released high-Z; reset_level=%d\n", reset_level);

	return 0;
}

static void dwm_cs_wake_pulse(void)
{
	printk("Applying CS wake pulse: CS low for %u us\n", CS_WAKE_US);

	gpio_pin_set_raw(dwm_cs.port, dwm_cs.pin, 0);
	k_usleep(CS_WAKE_US);
	gpio_pin_set_raw(dwm_cs.port, dwm_cs.pin, 1);

	k_msleep(CS_WAKE_SETTLE_MS);
	printk("CS wake pulse complete\n");
}

static void print_bytes(const char *label, const uint8_t *data, size_t len)
{
	size_t i;

	printk("%s", label);

	for (i = 0U; i < len; i++) {
		printk("%02x", data[i]);

		if ((i + 1U) < len) {
			printk(" ");
		}
	}

	printk("\n");
}

static uint32_t read_le32_at(const uint8_t *data, size_t offset)
{
	return ((uint32_t)data[offset]) |
	       ((uint32_t)data[offset + 1U] << 8U) |
	       ((uint32_t)data[offset + 2U] << 16U) |
	       ((uint32_t)data[offset + 3U] << 24U);
}

static uint32_t read_be32_at(const uint8_t *data, size_t offset)
{
	return ((uint32_t)data[offset] << 24U) |
	       ((uint32_t)data[offset + 1U] << 16U) |
	       ((uint32_t)data[offset + 2U] << 8U) |
	       ((uint32_t)data[offset + 3U]);
}

static int print_candidate(const char *name, const uint8_t *rx, size_t len,
			   size_t offset, size_t nominal_data_offset)
{
	uint32_t le;
	uint32_t be;
	int matched = 0;

	if ((offset + DW3110_DEV_ID_LEN) > len) {
		return 0;
	}

	le = read_le32_at(rx, offset);
	be = read_be32_at(rx, offset);

	printk("%s offset=%u le=0x%08x be=0x%08x",
	       name,
	       (unsigned int)offset,
	       (unsigned int)le,
	       (unsigned int)be);

	if (offset == nominal_data_offset) {
		printk(" NOMINAL");
	}

	if (le == DW3110_EXPECTED_DEV_ID) {
		printk(" MATCH_LE");
		matched = 1;
	}

	if (be == DW3110_EXPECTED_DEV_ID) {
		printk(" MATCH_BE");
		matched = 1;
	}

	if (((le & 0xFFFF0000U) == 0xDECA0000U) ||
	    ((be & 0xFFFF0000U) == 0xDECA0000U)) {
		printk(" DECA_PREFIX");
	}

	printk("\n");

	return matched;
}

static int dw3110_probe_variant(uint32_t cycle,
				const struct dw3110_probe_variant *variant)
{
	int ret;
	int irq_level;
	int match_count = 0;
	size_t i;

	uint8_t tx[MAX_PROBE_LEN] = { 0 };
	uint8_t rx[MAX_PROBE_LEN] = { 0 };

	struct spi_buf tx_spi_buf = {
		.buf = tx,
		.len = variant->len,
	};

	struct spi_buf rx_spi_buf = {
		.buf = rx,
		.len = variant->len,
	};

	const struct spi_buf_set tx_set = {
		.buffers = &tx_spi_buf,
		.count = 1,
	};

	const struct spi_buf_set rx_set = {
		.buffers = &rx_spi_buf,
		.count = 1,
	};

	const struct spi_config spi_cfg = {
		.frequency = SPI5_FREQUENCY_HZ,
		.operation = SPI_OP_MODE_MASTER |
			     SPI_WORD_SET(8) |
			     SPI_TRANSFER_MSB,
		.slave = 0,
	};

	memcpy(tx, variant->tx, variant->len);
	memset(rx, 0, sizeof(rx));

	irq_level = gpio_pin_get_raw(dwm_irq.port, dwm_irq.pin);

	gpio_pin_set_raw(dwm_cs.port, dwm_cs.pin, 0);
	k_usleep(CS_SETUP_US);

	ret = spi_transceive(spi5_dev, &spi_cfg, &tx_set, &rx_set);

	k_usleep(CS_HOLD_US);
	gpio_pin_set_raw(dwm_cs.port, dwm_cs.pin, 1);

	printk("\ndw3110_cycle=%u variant=%s ret=%d irq=%d len=%u nominal_data_offset=%u\n",
	       cycle,
	       variant->name,
	       ret,
	       irq_level,
	       (unsigned int)variant->len,
	       (unsigned int)variant->nominal_data_offset);

	print_bytes("tx=", tx, variant->len);
	print_bytes("rx=", rx, variant->len);

	if (ret != 0) {
		printk("probe_result=SPI_ERROR ret=%d\n", ret);
		return ret;
	}

	for (i = 0U; i <= (variant->len - DW3110_DEV_ID_LEN); i++) {
		match_count += print_candidate("candidate",
					       rx,
					       variant->len,
					       i,
					       variant->nominal_data_offset);
	}

	printk("expected_dw3110_dev_id=0x%08x\n",
	       (unsigned int)DW3110_EXPECTED_DEV_ID);

	if (match_count > 0) {
		printk("probe_result=DEV_ID_MATCH match_count=%d\n", match_count);
	} else {
		printk("probe_result=SPI_OK_NO_DEV_ID_MATCH\n");
	}

	return ret;
}

int main(void)
{
	int ret;
	uint32_t cycle = 0U;
	size_t i;

	printk("\nThin-Pod Gateway rev0.1 NUCLEO DW3110 register-read probe v2\n");
	printk("Board: nucleo_n657x0_q\n");
	printk("Purpose: Gateway-local DW3110 DEV_ID read refinement over verified SPI5 path\n");
	printk("Register: DEV_ID / register 0x00 / length 4 bytes\n");
	printk("SPI naming: SCK, CS, COPI/MOSI, CIPO/MISO\n");
	printk("Expected DW3110-class DEV_ID candidate: 0x%08x\n",
	       (unsigned int)DW3110_EXPECTED_DEV_ID);
	printk("Reset handling: drive low, then release high-Z\n");
	printk("CS handling: manual CS low across complete header/read transaction\n");
	printk("No UWB RF. No ranging. No node-to-Gateway traffic.\n\n");

	if (!device_is_ready(spi5_dev)) {
		printk("ERROR: SPI5 device not ready\n");
		return 0;
	}

	printk("SPI5 ready: device=%s frequency=%u Hz\n",
	       spi5_dev->name, SPI5_FREQUENCY_HZ);

	if (check_gpio_ready(&dwm_reset, "DWM_RESET / PD0") != 0) {
		return 0;
	}

	if (check_gpio_ready(&dwm_cs, "DWM_CS / PA3") != 0) {
		return 0;
	}

	if (check_gpio_ready(&dwm_irq, "DWM_IRQ / PB9") != 0) {
		return 0;
	}

	if (gpio_pin_configure_dt(&dwm_cs, GPIO_OUTPUT_HIGH) != 0) {
		printk("ERROR: failed to configure DWM_CS / PA3\n");
		return 0;
	}

	if (gpio_pin_configure_dt(&dwm_irq, GPIO_INPUT) != 0) {
		printk("ERROR: failed to configure DWM_IRQ / PB9\n");
		return 0;
	}

	printk("GPIO configuration complete\n");
	printk("Manual CS is idle high and driven low during each complete SPI transfer\n\n");

	ret = dwm_reset_low_then_release();
	if (ret != 0) {
		printk("ERROR: reset release sequence failed ret=%d\n", ret);
		return 0;
	}

	dwm_cs_wake_pulse();

	while (1) {
		printk("\n=== DW3110 register-probe cycle %u ===\n", cycle);

		for (i = 0U; i < (sizeof(probe_variants) / sizeof(probe_variants[0])); i++) {
			(void)dw3110_probe_variant(cycle, &probe_variants[i]);
			k_msleep(INTER_VARIANT_SLEEP_MS);
		}

		cycle++;
		k_msleep(PROBE_SLEEP_MS);
	}

	return 0;
}
