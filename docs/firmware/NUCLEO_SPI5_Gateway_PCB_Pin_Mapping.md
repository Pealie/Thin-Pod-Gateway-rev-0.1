# NUCLEO SPI5 Gateway PCB Pin Mapping and Overlay Constraints

This document directly supports Gateway completion checklist Item 2: final verified pin map.

## Purpose

This note records the grounded Zephyr/device-tree evidence for the Thin-Pod Gateway rev 0.1 manufactured PCB host interface between the STM32 NUCLEO-N657X0-Q and the DWM3001-CDK UWB subsystem.

It exists to prevent the Stage-2 Zephyr RF bring-up from accidentally using the standalone `decawave_dwm3001cdk` board mapping when the relevant host-side firmware target is the NUCLEO Gateway supervisor.

The manufactured Gateway PCB pinout is the authority for this stage of work.

## Relevant hardware architecture

Thin-Pod Gateway rev 0.1 uses the STM32 NUCLEO-N657X0-Q as the Gateway supervisor. The DWM3001-CDK is connected to the Gateway PCB as a UWB subsystem over a routed host interface.

This is different from running firmware directly on the DWM3001-CDK nRF52833. The standalone Zephyr `decawave_dwm3001cdk` board files describe the CDK’s internal Nordic-side board support and are not the correct authority for the manufactured Gateway PCB host interface.

For Gateway rev 0.1 Stage-2 host-interface work, the relevant Zephyr board support is:

```text
C:\ncs\v3.3.1\zephyr\boards\st\nucleo_n657x0_q
```

Likely Zephyr board target:

```text
nucleo_n657x0_q
```
## Verification status

Status: Final verification in progress for Gateway rev 0.1.

This document directly supports Gateway completion checklist item 2: final verified pin map.

The pin map in this document is the firmware source of truth for the Thin-Pod Gateway rev 0.1 PCB. It is intended to be verified by continuity checks, powered voltage checks, GPIO probing and minimal SPI5 activity from the Gateway probe firmware.

No new Gateway architecture scope is introduced here.

## Gateway rev 0.1 verified signal map

| Signal | NUCLEO / MCU pin | Gateway net | Connected module pin | Verification method | Status |
|---|---|---|---|---|---|
| SPI5_SCK | PE15 | SPI5_SCK | DWM3001-CDK J10.23 | Continuity + SPI probe | TBD |
| SPI5_MISO | PG1 | SPI5_MISO | DWM3001-CDK J10.21 | Continuity + SPI probe | TBD |
| SPI5_MOSI | PG2 | SPI5_MOSI | DWM3001-CDK J10.19 | Continuity + SPI probe | TBD |
| DWM_CS | PA3 | DWM_CS | DWM3001-CDK J10.24 | Continuity + GPIO toggle | TBD |
| DWM_IRQ | PB9 | DWM_IRQ | DWM3001-CDK J10.15 | Continuity + GPIO read | TBD |
| DWM_RESET | PD0 | DWM_RESET | DWM3001-CDK J10.12 | Continuity + GPIO toggle | TBD |
| DWM_5V | NUCLEO 5 V rail | DWM_5V | DWM3001-CDK J10.2 | Powered voltage check | TBD |
| DWM_GND | Gateway GND | DWM_GND | DWM3001-CDK J10.6 / J10.9 / J10.14 / J10.20 / J10.25 | Continuity check | TBD |
| C6_3V3 | NUCLEO 3.3 V rail | C6_3V3 | XIAO ESP32-C6 3V3 | Powered voltage check | TBD |
| C6_GND | Gateway GND | C6_GND | XIAO ESP32-C6 GND | Continuity check | TBD |

## Manufactured Gateway PCB mapping

The manufactured Gateway PCB routes the NUCLEO pins to the DWM3001-CDK as follows:

| Gateway net | NUCLEO pin | DWM3001-CDK connection | Function |
|---|---:|---|---|
| `SPI5_SCK` | `PE15` | `J10.23 / SPI1_CLK` | SPI clock |
| `SPI5_MISO` | `PG1` | `J10.21 / SPI1_MISO` | SPI MISO |
| `SPI5_MOSI` | `PG2` | `J10.19 / SPI1_MOSI` | SPI MOSI |
| `DWM_CS` | `PA3` | `J10.24 / CS_RPI` | SPI chip select |
| `DWM_IRQ` | `PB9` | `J10.15 / GPIO_RPI` | DWM interrupt/status line |
| `DWM_RESET` | `PD0` | `J10.12 / RESET` | DWM reset line |

## Zephyr NUCLEO board-support findings

The installed Zephyr/NCS tree contains the NUCLEO board support at:

```text
C:\ncs\v3.3.1\zephyr\boards\st\nucleo_n657x0_q
```

The top-level DTS file is:

```text
nucleo_n657x0_q.dts
```

which includes:

```text
nucleo_n657x0_q_common.dtsi
```

The common DTS includes the STM32N657 pinctrl definitions:

```text
#include <st/n6/stm32n657x0hxq-pinctrl.dtsi>
```

The NUCLEO common DTS defines `&spi5` as enabled:

```dts
&spi5 {
        clocks = <&rcc STM32_CLOCK(APB2, 20)>,
                 <&rcc STM32_SRC_CKPER SPI5_SEL(1)>;
        pinctrl-0 = <&spi5_nss_pa3 &spi5_sck_pe15 &spi5_miso_pg1 &spi5_mosi_pg2>;
        pinctrl-names = "default";
        status = "okay";
};
```

This confirms that the NUCLEO board support already maps SPI5 onto the same pins used by the manufactured Gateway PCB:

```text
SPI5_NSS   PA3
SPI5_SCK   PE15
SPI5_MISO  PG1
SPI5_MOSI  PG2
```

The Arduino R3 connector mapping also aligns with the Gateway route:

```text
D10 -> PA3
D11 -> PG2
D12 -> PG1
D13 -> PE15
arduino_spi -> &spi5
```

## Overlay constraint: PA3 chip-select

The default NUCLEO board support includes `spi5_nss_pa3` in the SPI5 pinctrl list.

For Gateway bring-up, `PA3` should preferably be treated as a software-controlled GPIO chip-select using `cs-gpios`, rather than relying on hardware NSS.

Reason:

```text
DWM_CS is a board-level routed control signal to DWM3001-CDK J10.24 / CS_RPI.
Software-controlled CS is simpler to reason about during bring-up and easier to instrument.
```

Therefore, the Gateway overlay should probably override `&spi5` so that the default pinctrl group contains only:

```text
PE15 -> SPI5_SCK
PG1  -> SPI5_MISO
PG2  -> SPI5_MOSI
```

and then define:

```dts
cs-gpios = <&gpioa 3 GPIO_ACTIVE_LOW>;
```

## Overlay constraint: PD0 conflict with FDCAN1

The default NUCLEO common DTS enables `fdcan1`:

```dts
&fdcan1 {
        clocks = <&rcc STM32_CLOCK(APB1_2, 8)>,
                 <&rcc STM32_SRC_CKPER FDCAN_SEL(1)>;
        pinctrl-0 = <&fdcan1_rx_pd0 &fdcan1_tx_ph2>;
        pinctrl-names = "default";
        status = "okay";
};
```

The board `chosen` node also selects:

```dts
zephyr,canbus = &fdcan1;
```

This conflicts with the manufactured Gateway PCB use of `PD0` as:

```text
DWM_RESET -> DWM3001-CDK J10.12 / RESET
```

Therefore, any Gateway-specific overlay that uses `PD0` as `DWM_RESET` must disable `fdcan1` and remove or override the chosen CAN bus reference.

Required overlay action:

```dts
/ {
    chosen {
        /delete-property/ zephyr,canbus;
    };
};

&fdcan1 {
    status = "disabled";
};
```

## PB9 IRQ status

The manufactured Gateway PCB uses:

```text
PB9 -> DWM_IRQ -> DWM3001-CDK J10.15 / GPIO_RPI
```

At this documentation stage, `PB9` must be treated as a GPIO input for the DWM host-interface IRQ/status line.

Before finalising the overlay, confirm that `PB9` is not already claimed by another enabled peripheral in the generated DTS for the NUCLEO target.

Expected overlay use:

```dts
irq-gpios = <&gpiob 9 GPIO_ACTIVE_HIGH>;
```

The active polarity may need confirmation during bring-up.

## Draft overlay shape

The following is the intended shape of the Gateway overlay. It should be treated as a design target, not yet as proven firmware.

```dts
/*
 * Thin-Pod Gateway rev 0.1
 * NUCLEO-N657X0-Q to DWM3001-CDK host-interface route.
 *
 * Manufactured Gateway PCB mapping:
 *   SPI5_SCK  PE15 -> DWM3001-CDK J10.23 / SPI1_CLK
 *   SPI5_MISO PG1  -> DWM3001-CDK J10.21 / SPI1_MISO
 *   SPI5_MOSI PG2  -> DWM3001-CDK J10.19 / SPI1_MOSI
 *   DWM_CS    PA3  -> DWM3001-CDK J10.24 / CS_RPI
 *   DWM_IRQ   PB9  -> DWM3001-CDK J10.15 / GPIO_RPI
 *   DWM_RESET PD0  -> DWM3001-CDK J10.12 / RESET
 */

/ {
    chosen {
        /delete-property/ zephyr,canbus;
    };
};

&fdcan1 {
    status = "disabled";
};

&pinctrl {
    spi5_thinpod_default: spi5_thinpod_default {
        group1 {
            pinmux = <STM32_PINMUX('E', 15, AF5)>,
                     <STM32_PINMUX('G', 1, AF5)>,
                     <STM32_PINMUX('G', 2, AF5)>;
        };
    };
};

&spi5 {
    status = "okay";
    pinctrl-0 = <&spi5_thinpod_default>;
    pinctrl-names = "default";

    cs-gpios = <&gpioa 3 GPIO_ACTIVE_LOW>;

    dwm3001_host: dwm3001-host@0 {
        compatible = "thinpod,dwm3001-host";
        reg = <0>;
        spi-max-frequency = <1000000>;
        irq-gpios = <&gpiob 9 GPIO_ACTIVE_HIGH>;
        reset-gpios = <&gpiod 0 GPIO_ACTIVE_LOW>;
    };
};
```

Before committing this overlay as code, confirm:

```text
PE15/PG1/PG2 alternate-function value for SPI5
PB9 not claimed by another enabled peripheral
PD0 released by disabling fdcan1
Generated DTS contains the expected final SPI5 and GPIO mapping
```

## Engineering conclusion

The NUCLEO SPI5 route is now grounded.

The next firmware step should be a minimal NUCLEO-side SPI5/GPIO probe, not a full DW3110 driver port.

Recommended sequence:

```text
1. Commit this mapping note.
2. Add Gateway-specific Zephyr overlay.
3. Build hello-world or a tiny SPI5/GPIO probe against `nucleo_n657x0_q`.
4. Inspect generated DTS to confirm SPI5, PA3, PB9 and PD0.
5. Only then add a minimal DWM host-interface probe application.
```

## Valid claim after this note

This note supports the claim:

```text
The manufactured Thin-Pod Gateway rev 0.1 PCB host-interface mapping has been reconciled with the STM32 NUCLEO-N657X0-Q Zephyr board support. SPI5 aligns with the routed Gateway PCB SPI nets, while PA3 chip-select and PD0 reset require Gateway-specific overlay handling.
```

It does not yet claim:

```text
The DWM3001-CDK host interface has been exercised.
The DW3110/DW3000 radio has been initialised from the NUCLEO.
UWB RF TX/RX has been demonstrated from Zephyr on the Gateway.
```
