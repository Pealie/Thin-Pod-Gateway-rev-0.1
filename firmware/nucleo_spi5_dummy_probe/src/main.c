#include <errno.h>
#include <stdint.h>
#include <string.h>

#include <zephyr/device.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/spi.h>
#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>

#define SPI5_FREQUENCY_HZ 1000000U
#define SPI_PROBE_BYTES 64U
#define PROBE_SLEEP_MS 1000
#define RESET_LOW_MS 10
#define RESET_SETTLE_MS 100
#define CS_SETUP_US 10
#define CS_HOLD_US 10

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

int main(void)
{
int ret;
int irq_level;
uint32_t probe = 0U;
uint32_t i;

static const uint8_t pattern[4] = { 0xAA, 0x55, 0x00, 0xFF };
static uint8_t tx_buf[SPI_PROBE_BYTES];
static uint8_t rx_buf[SPI_PROBE_BYTES];

for (i = 0U; i < SPI_PROBE_BYTES; i++) {
tx_buf[i] = pattern[i % sizeof(pattern)];
}

const struct spi_buf tx_spi_buf = {
.buf = tx_buf,
.len = sizeof(tx_buf),
};

struct spi_buf rx_spi_buf = {
.buf = rx_buf,
.len = sizeof(rx_buf),
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

printk("\nThin-Pod Gateway rev0.1 NUCLEO SPI5 stretched dummy-transfer probe\n");
printk("Board: nucleo_n657x0_q\n");
printk("Purpose: SPI5 electrical/path confirmation only\n");
printk("Pattern: repeated 0xAA 0x55 0x00 0xFF\n");
printk("Transfer length: %u bytes\n", SPI_PROBE_BYTES);
printk("No DW3110 register access. No UWB RF.\n\n");

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
memset(rx_buf, 0, sizeof(rx_buf));

irq_level = gpio_pin_get_raw(dwm_irq.port, dwm_irq.pin);

gpio_pin_set_raw(dwm_cs.port, dwm_cs.pin, 0);
k_usleep(CS_SETUP_US);

ret = spi_transceive(spi5_dev, &spi_cfg, &tx_set, &rx_set);

k_usleep(CS_HOLD_US);
gpio_pin_set_raw(dwm_cs.port, dwm_cs.pin, 1);

printk("spi_probe=%u ret=%d irq=%d len=%u "
       "tx_start=%02x %02x %02x %02x "
       "rx_start=%02x %02x %02x %02x\n",
       probe,
       ret,
       irq_level,
       SPI_PROBE_BYTES,
       tx_buf[0], tx_buf[1], tx_buf[2], tx_buf[3],
       rx_buf[0], rx_buf[1], rx_buf[2], rx_buf[3]);

probe++;
k_msleep(PROBE_SLEEP_MS);
}

return 0;
}