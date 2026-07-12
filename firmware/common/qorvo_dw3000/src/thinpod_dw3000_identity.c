#include "thinpod_dw3000_identity.h"

#include <errno.h>
#include <stddef.h>
#include <string.h>

#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>

#include "deca_device_api.h"
#include "deca_interface.h"
#include "thinpod_dw3000_platform.h"

#define THINPOD_IDLE_RC_ATTEMPTS 500U
#define THINPOD_IDLE_RC_WAIT_US 100U

extern const struct dwt_driver_s dw3000_driver;

static bool device_id_matches(uint32_t device_id)
{
    return ((device_id & dw3000_driver.devmatch) ==
            (dw3000_driver.devid & dw3000_driver.devmatch));
}

static void initialise_result(
    struct thinpod_dw3000_identity_result *result)
{
    memset(result, 0, sizeof(*result));

    result->final_rc = -EINPROGRESS;
    result->failure_stage = "none";

    result->platform_rc = -EINPROGRESS;
    result->reset_rc = -EINPROGRESS;
    result->probe_rc = -EINPROGRESS;
    result->initialise_rc = -EINPROGRESS;

    result->irq_before = -EINPROGRESS;
    result->irq_after = -EINPROGRESS;

    result->expected_device_id = dw3000_driver.devid;
    result->device_id_mask = dw3000_driver.devmatch;
}

static int wait_for_idle_rc(
    struct thinpod_dw3000_identity_result *result)
{
    uint32_t attempt;

    for (attempt = 0U;
         attempt < THINPOD_IDLE_RC_ATTEMPTS;
         attempt++) {

        result->idle_rc = dwt_checkidlerc();

        if (result->idle_rc != 0U) {
            return 0;
        }

        k_busy_wait(THINPOD_IDLE_RC_WAIT_US);
    }

    return -ETIMEDOUT;
}

static int record_failure(
    const char *role,
    uint16_t role_id,
    struct thinpod_dw3000_identity_result *result,
    const char *stage,
    int rc)
{
    result->final_rc = rc;
    result->failure_stage = stage;

    printk("TPDW_STAGE role=%s stage=%s rc=%d result=FAIL\n",
           role, stage, rc);

    thinpod_dw3000_identity_print_summary(
        role,
        role_id,
        result);

    return rc;
}

void thinpod_dw3000_identity_print_summary(
    const char *role,
    uint16_t role_id,
    const struct thinpod_dw3000_identity_result *result)
{
    uint32_t lot_hi;
    uint32_t lot_lo;
    const char *status;

    if ((role == NULL) || (result == NULL)) {
        return;
    }

    lot_hi = (uint32_t)(result->lot_id >> 32U);
    lot_lo = (uint32_t)(result->lot_id & 0xFFFFFFFFULL);

    status = (result->final_rc == 0) ? "PASS" : "FAIL";

    printk("TPDW_RESULT role=%s role_id=0x%04x result=%s failure_stage=%s final_rc=%d platform_rc=%d reset_rc=%d probe_rc=%d idlerc=%u init_rc=%d devid_match=%u post_match=%u fast_match=%u stable=%u\n",
           role,
           role_id,
           status,
           result->failure_stage,
           result->final_rc,
           result->platform_rc,
           result->reset_rc,
           result->probe_rc,
           result->idle_rc,
           result->initialise_rc,
           result->device_id_match ? 1U : 0U,
           result->post_initialise_match ? 1U : 0U,
           result->fast_device_id_match ? 1U : 0U,
           result->device_id_stable ? 1U : 0U);

    printk("TPDW_DEVICE role=%s api=0x%08x driver=%s expected=0x%08x mask=0x%08x devid_slow=0x%08x devid_after_init=0x%08x devid_fast=0x%08x\n",
           role,
           (unsigned int)result->api_version,
           thinpod_dw3000_driver_version(),
           (unsigned int)result->expected_device_id,
           (unsigned int)result->device_id_mask,
           (unsigned int)result->device_id_slow,
           (unsigned int)result->device_id_after_initialise,
           (unsigned int)result->device_id_fast);

    printk("TPDW_OTP role=%s part_id=0x%08x lot_hi=0x%08x lot_lo=0x%08x otp_revision=%u ref_voltage=%u ref_temperature=%u\n",
           role,
           (unsigned int)result->part_id,
           (unsigned int)lot_hi,
           (unsigned int)lot_lo,
           result->otp_revision,
           result->reference_voltage,
           result->reference_temperature);

    printk("TPDW_IRQ role=%s before=%d after=%d\n",
           role,
           result->irq_before,
           result->irq_after);
}

int thinpod_dw3000_identity_proof(
    const char *role,
    uint16_t role_id,
    struct thinpod_dw3000_identity_result *result)
{
    int rc;

    if ((role == NULL) || (result == NULL)) {
        return -EINVAL;
    }

    initialise_result(result);

    printk("\n");
    printk("TPDW_BEGIN role=%s role_id=0x%04x proof=identity-initialise-v1\n",
           role,
           role_id);

    result->platform_rc = thinpod_dw3000_platform_prepare();

    printk("TPDW_STAGE role=%s stage=platform rc=%d result=%s\n",
           role,
           result->platform_rc,
           (result->platform_rc == 0) ? "PASS" : "FAIL");

    if (result->platform_rc != 0) {
        return record_failure(
            role,
            role_id,
            result,
            "platform",
            result->platform_rc);
    }

    port_set_dw_ic_spi_slowrate();

    printk("TPDW_STAGE role=%s stage=slow_spi rate_hz=2000000 result=PASS\n",
           role);

    result->reset_rc = thinpod_dw3000_reset();

    printk("TPDW_STAGE role=%s stage=reset rc=%d result=%s\n",
           role,
           result->reset_rc,
           (result->reset_rc == 0) ? "PASS" : "FAIL");

    if (result->reset_rc != 0) {
        return record_failure(
            role,
            role_id,
            result,
            "reset",
            result->reset_rc);
    }

    result->irq_before = thinpod_dw3000_irq_level();

    if (result->irq_before < 0) {
        return record_failure(
            role,
            role_id,
            result,
            "irq_before",
            result->irq_before);
    }

    result->probe_rc = dwt_probe(
        (struct dwt_probe_s *)&thinpod_dw3000_probe_interf);

    printk("TPDW_STAGE role=%s stage=probe rc=%d result=%s\n",
           role,
           result->probe_rc,
           (result->probe_rc == DWT_SUCCESS) ? "PASS" : "FAIL");

    if (result->probe_rc != DWT_SUCCESS) {
        return record_failure(
            role,
            role_id,
            result,
            "probe",
            result->probe_rc);
    }

    result->api_version = dwt_apiversion();
    result->device_id_slow = dwt_readdevid();
    result->device_id_match =
        device_id_matches(result->device_id_slow);

    printk("TPDW_STAGE role=%s stage=device_id id=0x%08x expected=0x%08x mask=0x%08x match=%u result=%s\n",
           role,
           (unsigned int)result->device_id_slow,
           (unsigned int)result->expected_device_id,
           (unsigned int)result->device_id_mask,
           result->device_id_match ? 1U : 0U,
           result->device_id_match ? "PASS" : "FAIL");

    if (!result->device_id_match) {
        return record_failure(
            role,
            role_id,
            result,
            "device_id",
            -ENODEV);
    }

    rc = wait_for_idle_rc(result);

    printk("TPDW_STAGE role=%s stage=idle_rc value=%u rc=%d result=%s\n",
           role,
           result->idle_rc,
           rc,
           (rc == 0) ? "PASS" : "FAIL");

    if (rc != 0) {
        return record_failure(
            role,
            role_id,
            result,
            "idle_rc",
            rc);
    }

    result->initialise_rc = dwt_initialise(DWT_READ_OTP_ALL);

    printk("TPDW_STAGE role=%s stage=initialise rc=%d result=%s\n",
           role,
           result->initialise_rc,
           (result->initialise_rc == DWT_SUCCESS) ?
               "PASS" : "FAIL");

    if (result->initialise_rc != DWT_SUCCESS) {
        return record_failure(
            role,
            role_id,
            result,
            "initialise",
            result->initialise_rc);
    }

    result->device_id_after_initialise = dwt_readdevid();
    result->post_initialise_match =
        device_id_matches(result->device_id_after_initialise);

    result->part_id = dwt_getpartid();
    result->lot_id = dwt_getlotid();
    result->otp_revision = dwt_otprevision();
    result->reference_voltage = dwt_geticrefvolt();
    result->reference_temperature = dwt_geticreftemp();

    if (!result->post_initialise_match) {
        return record_failure(
            role,
            role_id,
            result,
            "post_initialise_device_id",
            -ENODEV);
    }

    port_set_dw_ic_spi_fastrate();

    result->device_id_fast = dwt_readdevid();
    result->fast_device_id_match =
        device_id_matches(result->device_id_fast);

    result->device_id_stable =
        (result->device_id_slow ==
         result->device_id_after_initialise) &&
        (result->device_id_after_initialise ==
         result->device_id_fast);

    printk("TPDW_STAGE role=%s stage=fast_spi rate_hz=8000000 id=0x%08x match=%u stable=%u result=%s\n",
           role,
           (unsigned int)result->device_id_fast,
           result->fast_device_id_match ? 1U : 0U,
           result->device_id_stable ? 1U : 0U,
           (result->fast_device_id_match &&
            result->device_id_stable) ?
               "PASS" : "FAIL");

    if (!result->fast_device_id_match ||
        !result->device_id_stable) {

        return record_failure(
            role,
            role_id,
            result,
            "fast_spi",
            -EIO);
    }

    result->irq_after = thinpod_dw3000_irq_level();

    if (result->irq_after < 0) {
        return record_failure(
            role,
            role_id,
            result,
            "irq_after",
            result->irq_after);
    }

    result->final_rc = 0;
    result->failure_stage = "none";

    thinpod_dw3000_identity_print_summary(
        role,
        role_id,
        result);

    return 0;
}
