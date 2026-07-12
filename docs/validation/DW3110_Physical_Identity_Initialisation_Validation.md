# DW3110 Physical Identity and Initialisation Validation

## Purpose

This document records the physical DW3110 identity and initialisation proof completed on the Thin-Pod Gateway-side and Node-side DWM3001-CDK boards.

## Validation date

```text
12 July 2026
```

## Repository state

```text
Branch: gateway-dw3110-rf-exchange
Implementation commit: 368c570af42dcaf4aa6e2e291875ed80c76d801f
```

## Validation scope

The validation covered:

- Zephyr SPI3 and GPIO preparation;
- physical DW3110 reset assertion and release;
- Qorvo `dwt_probe()` execution;
- masked DW3110-family device-ID verification;
- IDLE_RC readiness;
- `dwt_initialise(DWT_READ_OTP_ALL)`;
- post-initialisation device-ID readback;
- device-ID stability at 2 MHz and 8 MHz;
- OTP identity capture;
- physical IRQ-level observation;
- independent testing of both DWM3001-CDK boards.

The validation introduced no RF transmission, reception, ranging or telemetry exchange.

## Firmware environment

| Component | Value |
|---|---|
| nRF Connect SDK | v3.3.1 |
| Zephyr | v4.3.99 |
| Qorvo driver | DW3XXX Device Driver Version 08.19.02 |
| Board target | `decawave_dwm3001cdk/nrf52833` |
| External Qorvo SDK | `DW3_QM33_SDK_1.1.0` |
| Implementation commit | `368c570af42dcaf4aa6e2e291875ed80c76d801f` |

## Build artefact hashes

```text
SHA256  872BC431F2DFEADC609F4A04C8B79BDB9E0CDEE7CB894D80CA70DA46689A33D4  gateway zephyr.elf
SHA256  9FAEA9B8698F03D92BA6E776B13F57D17E3CEC113D27B089387ED3E14A1FE7D1  gateway merged.hex
SHA256  C26C4C893B4666C77B1A323640252779D684D9C0F59BE595A42881D1F56CBBC1  node zephyr.elf
SHA256  7F419321B029CBCC661A48E9DEFA453A8BAF19EDDBF259237466774E7FEA5F3C  node merged.hex
```

## Gateway-side DWM3001-CDK

### Result

```text
TPDW_RESULT role=gateway role_id=0x0000 result=PASS failure_stage=none final_rc=0 platform_rc=0 reset_rc=0 probe_rc=0 idlerc=1 init_rc=0 devid_match=1 post_match=1 fast_match=1 stable=1
```

### Device evidence

```text
TPDW_DEVICE role=gateway api=0x00081902 driver=DW3XXX Device Driver Version 08.19.02 expected=0xdeca0312 mask=0xffffff0f devid_slow=0xdeca0302 devid_after_init=0xdeca0302 devid_fast=0xdeca0302
```

### OTP evidence

```text
TPDW_OTP role=gateway part_id=0x0f34872a lot_hi=0x00005056 lot_lo=0x36583637 otp_revision=1 ref_voltage=119 ref_temperature=124
```

### IRQ observation

```text
TPDW_IRQ role=gateway before=1 after=1
```

### Physical identity

| Field | Value |
|---|---|
| Role ID | `0x0000` |
| Device ID | `0xdeca0302` |
| Expected family ID | `0xdeca0312` |
| Device-ID mask | `0xffffff0f` |
| Part ID | `0x0f34872a` |
| Lot ID high | `0x00005056` |
| Lot ID low | `0x36583637` |
| OTP revision | `1` |
| Reference voltage | `119` |
| Reference temperature | `124` |
| IRQ before initialisation | `1` |
| IRQ after initialisation | `1` |

### Evidence file

```text
logs/validation/dw3110-identity-init/gateway-dwm3001cdk-rtt.log
```

### Evidence SHA-256

```text
DA14DCC850B29FFE88ED0CE282CE651F9DB178BF7CC7AFFE446EA073F634BF9E
```

Result: **PASS**

## Node-side DWM3001-CDK

### Result

```text
TPDW_RESULT role=node role_id=0x0001 result=PASS failure_stage=none final_rc=0 platform_rc=0 reset_rc=0 probe_rc=0 idlerc=1 init_rc=0 devid_match=1 post_match=1 fast_match=1 stable=1
```

### Device evidence

```text
TPDW_DEVICE role=node api=0x00081902 driver=DW3XXX Device Driver Version 08.19.02 expected=0xdeca0312 mask=0xffffff0f devid_slow=0xdeca0302 devid_after_init=0xdeca0302 devid_fast=0xdeca0302
```

### OTP evidence

```text
TPDW_OTP role=node part_id=0x8ec49c9a lot_hi=0x00005056 lot_lo=0x34583230 otp_revision=1 ref_voltage=118 ref_temperature=126
```

### IRQ observation

```text
TPDW_IRQ role=node before=1 after=1
```

### Physical identity

| Field | Value |
|---|---|
| Role ID | `0x0001` |
| Device ID | `0xdeca0302` |
| Expected family ID | `0xdeca0312` |
| Device-ID mask | `0xffffff0f` |
| Part ID | `0x8ec49c9a` |
| Lot ID high | `0x00005056` |
| Lot ID low | `0x34583230` |
| OTP revision | `1` |
| Reference voltage | `118` |
| Reference temperature | `126` |
| IRQ before initialisation | `1` |
| IRQ after initialisation | `1` |

### Evidence file

```text
logs/validation/dw3110-identity-init/node-dwm3001cdk-rtt.log
```

### Evidence SHA-256

```text
9AC40F65C265C37CE6FC36067F6D0D118E1ACB244C8DF193C7EC39CF8EF3CB05
```

Result: **PASS**

## Acceptance results

| Criterion | Gateway | Node |
|---|---:|---:|
| Zephyr SPI/GPIO preparation | PASS | PASS |
| RESET assertion and release | PASS | PASS |
| `dwt_probe()` | PASS | PASS |
| Device-ID family match | PASS | PASS |
| IDLE_RC readiness | PASS | PASS |
| `dwt_initialise()` | PASS | PASS |
| Post-initialisation device-ID readback | PASS | PASS |
| 8 MHz device-ID readback | PASS | PASS |
| Device-ID stability | PASS | PASS |
| OTP identity capture | PASS | PASS |
| IRQ-level observation | captured | captured |

## Device distinction

The Gateway and Node boards reported distinct OTP part IDs:

```text
Gateway: 0x0f34872a
Node:    0x8ec49c9a
```

This confirms that the physical validation covered two separate DW3110 transceivers.

## IRQ observation

Both boards reported:

```text
before=1 after=1
```

The IRQ level was captured as an observation. Interrupt polarity, idle behaviour and event clearing will be examined during the receive-path milestone.

## Overall result

```text
PASS
```

Both DWM3001-CDK boards independently established physical SPI communication with their internal DW3110 transceivers, selected the Qorvo DW3000/DW3110 driver, returned matching device identifiers, reached IDLE_RC and completed initialisation successfully.

## Validated claim

```text
The Thin-Pod Gateway-side and Node-side DWM3001-CDK boards independently established physical SPI communication with their internal DW3110 transceivers, returned Qorvo-driver-matching device identifiers, reached IDLE_RC and completed DW3110 initialisation successfully.
```

## Remaining boundary

This validation does not demonstrate:

- UWB RF transmission;
- UWB RF reception;
- frame exchange;
- ranging;
- telemetry transport;
- vibration-window transport;
- IRQ-driven receive handling;
- RF performance;
- regulatory compliance.

The next firmware gate is a one-way UWB frame transmitted by one DWM3001-CDK and received by the other.
