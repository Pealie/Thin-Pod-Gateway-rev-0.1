#ifndef THINPOD_DW3000_PLATFORM_H
#define THINPOD_DW3000_PLATFORM_H

#include <stdbool.h>
#include <stdint.h>

#include "deca_device_api.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Build-time integration marker.
 *
 * This function does not access the DW3110. It ensures that the Qorvo
 * compatibility layer, device driver and Thin-Pod Zephyr platform adapter
 * are all linked into the application.
 */
const char *thinpod_dw3000_driver_version(void);

/*
 * Prepare the Zephyr SPI/GPIO objects.
 *
 * This is intentionally not called by the Stage-2 scaffold applications
 * during the first build-only milestone.
 */
int thinpod_dw3000_platform_prepare(void);

/* Read the physical DW3110 IRQ input. */
bool thinpod_dw3000_irq_is_active(void);

/* Qorvo probe interface for the DW3000/DW3110 driver family. */
extern const struct dwt_probe_s thinpod_dw3000_probe_interf;

/* Qorvo platform callbacks. */
void port_set_dw_ic_spi_slowrate(void);
void port_set_dw_ic_spi_fastrate(void);

int32_t writetospi(
    uint16_t header_length,
    const uint8_t *header_buffer,
    uint16_t body_length,
    const uint8_t *body_buffer);

int32_t writetospiwithcrc(
    uint16_t header_length,
    const uint8_t *header_buffer,
    uint16_t body_length,
    const uint8_t *body_buffer,
    uint8_t crc8);

int32_t readfromspi(
    uint16_t header_length,
    uint8_t *header_buffer,
    uint16_t read_length,
    uint8_t *read_buffer);

void reset_DWIC(void);
void wakeup_device_with_io(void);

void deca_sleep(unsigned int time_ms);
void deca_usleep(unsigned long time_us);

decaIrqStatus_t decamutexon(void);
void decamutexoff(decaIrqStatus_t state);

#ifdef __cplusplus
}
#endif

#endif
