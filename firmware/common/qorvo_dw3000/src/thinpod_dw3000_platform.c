#include "thinpod_dw3000_platform.h"

#include <errno.h>
#include <stddef.h>
#include <string.h>

#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/spi.h>
#include <zephyr/irq.h>
#include <zephyr/kernel.h>
#include <zephyr/sys/util.h>

#include "deca_interface.h"

/*
 * DWM3001-CDK internal DW3110 connection:
 *
 * SPI3 SCK       P0.03
 * SPI3 COPI      P0.08
 * SPI3 CIPO      P0.29
 * SPI3 CS        P1.06
 * DW3110 RESET   P0.25
 * DW3110 IRQ     P1.02
 */

#define DW3000_NODE DT_NODELABEL(dw3000)

#if !DT_NODE_EXISTS(DW3000_NODE)
#error "The dw3000 devicetree node is missing"
#endif

static struct spi_dt_spec dw3000_spi =
    SPI_DT_SPEC_GET(
        DW3000_NODE,
        SPI_WORD_SET(8) |
        SPI_TRANSFER_MSB |
        SPI_OP_MODE_MASTER);

static const struct gpio_dt_spec dw3000_reset =
    GPIO_DT_SPEC_GET(DT_PATH(zephyr_user), dw3000_reset_gpios);

static const struct gpio_dt_spec dw3000_irq =
    GPIO_DT_SPEC_GET(DT_PATH(zephyr_user), dw3000_irq_gpios);

static const struct gpio_dt_spec dw3000_cs =
    GPIO_DT_SPEC_GET(DT_PATH(zephyr_user), dw3000_cs_gpios);

static bool platform_prepared;

static int ensure_platform_prepared(void)
{
    if (platform_prepared) {
        return 0;
    }

    return thinpod_dw3000_platform_prepare();
}

const char *thinpod_dw3000_driver_version(void)
{
    return dwt_version_string();
}

int thinpod_dw3000_platform_prepare(void)
{
    int rc;

    if (platform_prepared) {
        return 0;
    }

    if (!spi_is_ready_dt(&dw3000_spi)) {
        return -ENODEV;
    }

    if (!gpio_is_ready_dt(&dw3000_reset) ||
        !gpio_is_ready_dt(&dw3000_irq) ||
        !gpio_is_ready_dt(&dw3000_cs)) {
        return -ENODEV;
    }

    /*
     * RESET is open-drain in behaviour. Leave it released as an input.
     * It must never be actively driven high.
     */
    rc = gpio_pin_configure_dt(&dw3000_reset, GPIO_INPUT);
    if (rc != 0) {
        return rc;
    }

    rc = gpio_pin_configure_dt(&dw3000_irq, GPIO_INPUT);
    if (rc != 0) {
        return rc;
    }

    /*
     * Keep CS inactive while it is under manual GPIO control for wake-up.
     * Normal SPI transfers use the SPI devicetree CS definition.
     */
    rc = gpio_pin_configure_dt(&dw3000_cs, GPIO_OUTPUT_INACTIVE);
    if (rc != 0) {
        return rc;
    }

    platform_prepared = true;
    return 0;
}

bool thinpod_dw3000_irq_is_active(void)
{
    int value;

    if (ensure_platform_prepared() != 0) {
        return false;
    }

    value = gpio_pin_get_dt(&dw3000_irq);
    return value > 0;
}

void port_set_dw_ic_spi_slowrate(void)
{
    dw3000_spi.config.frequency = 2000000U;
}

void port_set_dw_ic_spi_fastrate(void)
{
    /*
     * Conservative first-integration rate.
     * Qorvo permits higher rates after initialisation.
     */
    dw3000_spi.config.frequency = 8000000U;
}

static int spi_write_segments(
    const uint8_t *header,
    uint16_t header_length,
    const uint8_t *body,
    uint16_t body_length,
    const uint8_t *trailer,
    uint16_t trailer_length)
{
    struct spi_buf tx_buffers[3];
    struct spi_buf_set tx_set;
    size_t count = 0U;

    if (ensure_platform_prepared() != 0) {
        return -1;
    }

    if ((header_length > 0U) && (header == NULL)) {
        return -1;
    }

    if ((body_length > 0U) && (body == NULL)) {
        return -1;
    }

    if ((trailer_length > 0U) && (trailer == NULL)) {
        return -1;
    }

    if (header_length > 0U) {
        tx_buffers[count].buf = (void *)header;
        tx_buffers[count].len = header_length;
        count++;
    }

    if (body_length > 0U) {
        tx_buffers[count].buf = (void *)body;
        tx_buffers[count].len = body_length;
        count++;
    }

    if (trailer_length > 0U) {
        tx_buffers[count].buf = (void *)trailer;
        tx_buffers[count].len = trailer_length;
        count++;
    }

    tx_set.buffers = tx_buffers;
    tx_set.count = count;

    return spi_write_dt(&dw3000_spi, &tx_set) == 0 ? 0 : -1;
}

int32_t writetospi(
    uint16_t header_length,
    const uint8_t *header_buffer,
    uint16_t body_length,
    const uint8_t *body_buffer)
{
    return spi_write_segments(
        header_buffer,
        header_length,
        body_buffer,
        body_length,
        NULL,
        0U);
}

int32_t writetospiwithcrc(
    uint16_t header_length,
    const uint8_t *header_buffer,
    uint16_t body_length,
    const uint8_t *body_buffer,
    uint8_t crc8)
{
    return spi_write_segments(
        header_buffer,
        header_length,
        body_buffer,
        body_length,
        &crc8,
        sizeof(crc8));
}

int32_t readfromspi(
    uint16_t header_length,
    uint8_t *header_buffer,
    uint16_t read_length,
    uint8_t *read_buffer)
{
    struct spi_buf tx_buffers[2];
    struct spi_buf rx_buffers[2];
    struct spi_buf_set tx_set;
    struct spi_buf_set rx_set;
    uint8_t discard[4] = { 0 };
    uint8_t dummy[256] = { 0 };

    if (ensure_platform_prepared() != 0) {
        return -1;
    }

    if ((header_length == 0U) ||
        (header_buffer == NULL) ||
        (read_buffer == NULL) ||
        (header_length > sizeof(discard)) ||
        (read_length > sizeof(dummy))) {
        return -1;
    }

    tx_buffers[0].buf = header_buffer;
    tx_buffers[0].len = header_length;
    tx_buffers[1].buf = dummy;
    tx_buffers[1].len = read_length;

    rx_buffers[0].buf = discard;
    rx_buffers[0].len = header_length;
    rx_buffers[1].buf = read_buffer;
    rx_buffers[1].len = read_length;

    tx_set.buffers = tx_buffers;
    tx_set.count = ARRAY_SIZE(tx_buffers);

    rx_set.buffers = rx_buffers;
    rx_set.count = ARRAY_SIZE(rx_buffers);

    return spi_transceive_dt(
        &dw3000_spi,
        &tx_set,
        &rx_set) == 0 ? 0 : -1;
}

void reset_DWIC(void)
{
    if (ensure_platform_prepared() != 0) {
        return;
    }

    /*
     * Assert RESET by driving low, hold it, then release the line by
     * returning the GPIO to input mode. Never drive RESET high.
     */
    (void)gpio_pin_configure_dt(&dw3000_reset, GPIO_OUTPUT_ACTIVE);
    k_busy_wait(20U);
    (void)gpio_pin_configure_dt(&dw3000_reset, GPIO_INPUT);
    k_msleep(2);
}

void wakeup_device_with_io(void)
{
    if (ensure_platform_prepared() != 0) {
        return;
    }

    /*
     * Wake using CS. Hold CS low for at least 500 microseconds, release it,
     * then allow the device to become ready.
     */
    (void)gpio_pin_set_dt(&dw3000_cs, 1);
    k_busy_wait(600U);
    (void)gpio_pin_set_dt(&dw3000_cs, 0);
    k_busy_wait(500U);
}

void deca_sleep(unsigned int time_ms)
{
    k_msleep(time_ms);
}

void deca_usleep(unsigned long time_us)
{
    k_busy_wait((uint32_t)time_us);
}

decaIrqStatus_t decamutexon(void)
{
    return (decaIrqStatus_t)irq_lock();
}

void decamutexoff(decaIrqStatus_t state)
{
    irq_unlock((unsigned int)state);
}

extern const struct dwt_driver_s dw3000_driver;

static struct dwt_driver_s *thinpod_driver_list[] = {
    (struct dwt_driver_s *)&dw3000_driver
};

static const struct dwt_spi_s thinpod_dw3000_spi_functions = {
    .readfromspi = readfromspi,
    .writetospi = writetospi,
    .writetospiwithcrc = writetospiwithcrc,
    .setslowrate = port_set_dw_ic_spi_slowrate,
    .setfastrate = port_set_dw_ic_spi_fastrate
};

const struct dwt_probe_s thinpod_dw3000_probe_interf = {
    .dw = NULL,
    .spi = (void *)&thinpod_dw3000_spi_functions,
    .wakeup_device_with_io = wakeup_device_with_io,
    .driver_list = thinpod_driver_list,
    .dw_driver_num = ARRAY_SIZE(thinpod_driver_list)
};
