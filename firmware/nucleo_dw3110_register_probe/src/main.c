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

#define PROBE_SLEEP_MS 1000
#define RESET_LOW_MS 10
#define RESET_SETTLE_MS 500

#define CS_SETUP_US 10
#define CS_HOLD_US 10

#define DW3110_DEV_ID_REG 0x00U
#define DW3110_DEV_ID_LEN 4U
#define DW3110_EXPECTED_DEV_ID 0xDECA0302U

#define DW_READ_PROBE_LEN 6U

static const struct device *const spi5_dev = DEVICE_DT_GET(DT_NODELABEL(spi5));

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

static void dwm_reset_pulse(void)
{
	printk("Applying DWM reset pulse on PD0\n");

	gpio_pin_set_raw(dwm_reset.port, dwm_reset.pin, 0);
	k_msleep(RESET_LOW_MS);

	gpio_pin_set_raw(dwm_reset.port, dwm_reset.pin, 1);
	k_msleep(RESET_SETTLE_MS);

	printk("DWM reset released\n");
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

static void print_candidate(const char *name, const uint8_t *rx, size_t len,
			    size_t offset)
{
	uint32_t le;
	uint32_t be;

	if ((offset + DW3110_DEV_ID_LEN) > len) {
		printk("%s offset=%u unavailable\n",
		       name,
		       (unsigned int)offset);
		return;
	}

	le = read_le32_at(rx, offset);
	be = read_be32_at(rx, offset);

	printk("%s offset=%u le=0x%08x be=0x%08x",
	       name,
	       (unsigned int)offset,
	       (unsigned int)le,
	       (unsigned int)be);

	if (le == DW3110_EXPECTED_DEV_ID) {
		printk(" MATCH_LE");
	}

	if (be == DW3110_EXPECTED_DEV_ID) {
		printk(" MATCH_BE");
	}

	if (((le & 0xFFFF0000U) == 0xDECA0000U) ||
	    ((be & 0xFFFF0000U) == 0xDECA0000U)) {
		printk(" DECA_PREFIX");
	}

	printk("\n");
}

static int dw3110_raw_dev_id_probe(uint32_t probe)
{
	int ret;
	int irq_level;

	uint8_t tx[DW_READ_PROBE_LEN] = { 0 };
	uint8_t rx[DW_READ_PROBE_LEN] = { 0 };

	const struct spi_buf tx_spi_buf = {
		.buf = tx,
		.len = sizeof(tx),
	};

	struct spi_buf rx_spi_buf = {
		.buf = rx,
		.len = sizeof(rx),
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

	/*
	 * First bring-up read attempt:
	 *
	 * DEV_ID is register 0x00.
	 * TX byte 0 is the register header/address.
	 * Remaining bytes clock out the possible 4-byte response.
	 *
	 * Candidate parsing checks offset 0, offset 1 and offset 2 so the log
	 * remains useful even if the first response byte is dummy/alignment data.
	 */
	tx[0] = DW3110_DEV_ID_REG;

	memset(rx, 0, sizeof(rx));

	irq_level = gpio_pin_get_raw(dwm_irq.port, dwm_irq.pin);

	gpio_pin_set_raw(dwm_cs.port, dwm_cs.pin, 0);
	k_usleep(CS_SETUP_US);

	ret = spi_transceive(spi5_dev, &spi_cfg, &tx_set, &rx_set);

	k_usleep(CS_HOLD_US);
	gpio_pin_set_raw(dwm_cs.port, dwm_cs.pin, 1);

	printk("\ndw3110_probe=%u ret=%d irq=%d reg=0x%02x len=%u\n",
	       probe,
	       ret,
	       irq_level,
	       DW3110_DEV_ID_REG,
	       (unsigned int)sizeof(tx));

	print_bytes("tx=", tx, sizeof(tx));
	print_bytes("rx=", rx, sizeof(rx));

	if (ret != 0) {
		printk("ERROR: SPI transaction failed before register data could be evaluated\n");
		return ret;
	}

	print_candidate("candidate", rx, sizeof(rx), 0U);
	print_candidate("candidate", rx, sizeof(rx), 1U);
	print_candidate("candidate", rx, sizeof(rx), 2U);

	printk("expected_dw3110_dev_id=0x%08x\n",
	       (unsigned int)DW3110_EXPECTED_DEV_ID);

	return ret;
}

int main(void)
{
	int ret;
	uint32_t probe = 0U;

	printk("\nThin-Pod Gateway rev0.1 NUCLEO DW3110 register-read probe\n");
	printk("Board: nucleo_n657x0_q\n");
	printk("Purpose: Gateway-local DW3110 DEV_ID read over verified SPI5 path\n");
	printk("Register: DEV_ID / register 0x00 / length 4 bytes\n");
	printk("SPI naming: SCK, CS, COPI/MOSI, CIPO/MISO\n");
	printk("Expected DW3110-class DEV_ID candidate: 0x%08x\n",
	       (unsigned int)DW3110_EXPECTED_DEV_ID);
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

	if (gpio_pin_configure_dt(&dwm_reset, GPIO_OUTPUT_HIGH) != 0) {
		printk("ERROR: failed to configure DWM_RESET / PD0\n");
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
	printk("Manual CS is idle high and driven low during SPI transfer\n\n");

	dwm_reset_pulse();

	while (1) {
		ret = dw3110_raw_dev_id_probe(probe);

		if (ret == 0) {
			printk("probe_result=SPI_OK\n");
		} else {
			printk("probe_result=SPI_ERROR ret=%d\n", ret);
		}

		probe++;
		k_msleep(PROBE_SLEEP_MS);
	}

	return 0;
}
