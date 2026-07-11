# DWM3001-CDK Host-Interface Pin Feasibility

## Document control

**Document identifier:** TP-GW-ARCH-0004<br>
**Document revision:** Draft A<br>
**Product context:** Thin-Pod Gateway rev 0.1<br>
**Document status:** Pin/interface feasibility investigation<br>
**Related documents:**<br>
- `docs/architecture/Gateway_UWB_Module_Access_Model.md`<br>
- `docs/dwm-to-nucleo-host-interface-protocol.md`<br>
- `logs/96_Engineering_Log.md`

---

## 1. Purpose

This document determines whether the Gateway rev 0.1 hardware can support a direct host interface between:

```text
NUCLEO-N657X0-Q
    and
DWM3001-CDK onboard nRF52833 firmware
```

The target host-interface proof is:

```text
GET_CAPABILITIES
GET_STATUS
GET_COUNTERS
```

The central feasibility question is:

```text
Which exposed DWM3001-CDK pins can the onboard nRF52833 use for a host interface to the NUCLEO?
```

---

## 2. Starting architectural position

Gateway rev 0.1 adopts the DWM3001-CDK-as-UWB-coprocessor model.

```text
DWM3001-CDK:
    intelligent UWB coprocessor/module
    owns DW3110 radio control

NUCLEO-N657X0-Q:
    Gateway supervisor / host / analysis controller

Gateway PCB:
    modular open-hardware carrier/interface layer
```

The direct NUCLEO-to-DW3110 DEV_ID probe is retained as diagnostic evidence. The host-interface proof should therefore target DWM3001-CDK firmware, not the DW3110 silicon directly.

---

## 3. Known Gateway-side signals

The Gateway rev 0.1 PCB currently exposes the following DWM-related signal paths:

```text
TP7  / J10.24 : DWM_CS
TP10 / J10.23 : SCK
TP11 / J10.19 : COPI / MOSI
TP12 / J10.21 : CIPO / MISO
TP6  / J10.15 : DWM_IRQ
TP5  / J10.12 : DWM_RESET
TP1          : GND reference
```

These paths have already supported GPIO and SPI electrical bring-up from the NUCLEO side.

The unresolved question is whether those DWM3001-CDK header pins reach nRF52833 GPIOs that can be configured as a host-interface peripheral, or whether they primarily expose the DW3110-facing path / CDK-specific diagnostic routing.

---

## 4. Known Zephyr DWM3001-CDK board-definition evidence

The local Zephyr board definition for `decawave_dwm3001cdk` shows an onboard SPI instance:

```text
&spi3 {
    compatible = "nordic,nrf-spim";
    cs-gpios = <&gpio1 6 GPIO_ACTIVE_LOW>;
}
```

The associated pinctrl entries show:

```text
SPIM_SCK  : P0.3
SPIM_MOSI : P0.8
SPIM_MISO : P0.29
CS        : P1.6
IRQ       : P0.16
```

This appears to describe the onboard nRF52833-to-DW3110 SPI master path.

Therefore, Gateway rev 0.1 should not assume that the existing Gateway SPI route can be reused as an nRF52833 SPI peripheral host interface until the external connector routing is checked.

---

## 5. Feasibility matrix

| Candidate route | Required exposed signals | Evidence required | Status |
|---|---:|---|---|
| SPI peripheral / SPIS | SCK, COPI/MOSI, CIPO/MISO, CS, IRQ | DWM3001-CDK header pins map to nRF52833 GPIOs that support SPIS and are not reserved for DW3110 | Pending |
| UART | TX, RX, optional IRQ/GPIO | DWM3001-CDK header pins expose a usable nRF UARTE pair connected to NUCLEO-accessible pins | Pending |
| GPIO mailbox | 2-4 GPIO lines | Enough nRF GPIOs are exposed and routed to NUCLEO for command/status strobes | Pending |
| Existing DW3110 SPI path | Existing TP7/TP10/TP11/TP12 | Proves direct silicon path only; diagnostic branch, not preferred host-interface route | Diagnostic only |
| Future wiring / rev 0.2 | Explicit host-interface pins | Current rev 0.1 lacks clean exposed interface | Pending |

---

## 6. Investigation procedure

### 6.1 Confirm repo signal names

Search the Gateway repo for the DWM connector, test points and signal names.

```powershell
cd C:\ThinPod\Thin-Pod-Gateway-rev-0.1

git grep -n "J10\.23\|J10\.21\|J10\.19\|J10\.24\|J10\.15\|J10\.12\|TP7\|TP10\|TP11\|TP12\|DWM_CS\|DWM_IRQ\|DWM_RESET\|MOSI\|MISO\|SCK"
```

Record the files and net names in this document.

### 6.2 Inspect Zephyr DWM3001-CDK board files

```powershell
cd C:\Users\n_tho\zephyrproject\zephyr

Get-Content boards\qorvo\decawave_dwm3001cdk\decawave_dwm3001cdk.dts

Get-Content boards\qorvo\decawave_dwm3001cdk\decawave_dwm3001cdk-pinctrl.dtsi
```

Record all SPI, UART, GPIO, button, LED, IRQ and reset pins.

### 6.3 Search for external connector documentation

```powershell
cd C:\Users\n_tho\zephyrproject\zephyr

Get-ChildItem boards\qorvo\decawave_dwm3001cdk -Recurse |
  Select-Object FullName
```

Then search the Thin-Pod Gateway repo:

```powershell
cd C:\ThinPod\Thin-Pod-Gateway-rev-0.1

git ls-files | Select-String -Pattern "DWM|DWM3001|CDK|J10|pin|connector|schematic|interface"
```

Record any connector tables already present.

### 6.4 Map Gateway pins to DWM3001-CDK function

Create a table with one row per Gateway-connected DWM pin:

| Gateway TP | CDK connector pin | Gateway signal | Known CDK function | nRF52833 GPIO? | DW3110 signal? | Usable for host interface? |
|---|---:|---|---|---|---|---|
| TP7 | J10.24 | DWM_CS | Pending | Pending | Pending | Pending |
| TP10 | J10.23 | SCK | Pending | Pending | Pending | Pending |
| TP11 | J10.19 | COPI / MOSI | Pending | Pending | Pending | Pending |
| TP12 | J10.21 | CIPO / MISO | Pending | Pending | Pending | Pending |
| TP6 | J10.15 | DWM_IRQ | Pending | Pending | Pending | Pending |
| TP5 | J10.12 | DWM_RESET | Pending | Pending | Pending | Pending |

### 6.5 Bench confirmation, if documentation is ambiguous

Use non-destructive checks first:

```text
Power removed:
    DMM continuity from Gateway test points to DWM3001-CDK header pins.

Power applied:
    scope only after ground sanity checks.
```

If candidate nRF GPIOs are identified, write a DWM3001-CDK GPIO toggle probe that toggles one candidate pin at a time and observe the corresponding Gateway TP.

Do not run a broad GPIO sweep against unknown pins. Avoid toggling pins that are documented as reset, power, SWD, DW3110 SPI or IRQ without a specific reason.

---

## 7. Decision criteria

### SPI peripheral / SPIS is viable if:

```text
At least four NUCLEO-connected DWM-side pins map to free nRF52833 GPIOs:
    SCK input
    COPI / MOSI input
    CIPO / MISO output
    CS input

A fifth pin is available for IRQ/ready notification.
```

### UART is viable if:

```text
At least two NUCLEO-connected DWM-side pins map to free nRF52833 GPIOs:
    DWM TX -> NUCLEO RX
    DWM RX <- NUCLEO TX

An optional IRQ or ready GPIO improves robustness.
```

### GPIO mailbox is viable if:

```text
A small number of NUCLEO-connected GPIO lines can carry command strobes,
ready state and small status/counter values for proof only.
```

### Rev 0.1 host-interface proof should be deferred if:

```text
The existing Gateway-connected DWM pins do not expose a clean nRF52833 firmware interface.
```

In that case, document Gateway rev 0.1 as a modular carrier with proven power/GPIO/SPI routing and record the host-interface as a rev 0.2 hardware requirement.

---

## 8. Preliminary conclusion

The local Zephyr board definition confirms that `spi3` is configured as an nRF SPIM path, apparently for the onboard DW3110 connection. This supports caution: the existing Gateway SPI path should not be assumed to be a DWM firmware host interface.

The next evidence item is a connector-level pin map from Gateway TP/J10 signals to DWM3001-CDK exposed functions and nRF52833 GPIO availability.

---

## 9. Interim findings from Gateway repo and DWM3001-CDK J10 audit

Gateway repo searches confirm that the manufactured Gateway rev 0.1 design routes the NUCLEO to the DWM3001-CDK J10 Raspberry Pi-compatible interface pins as follows:

| Gateway TP | NUCLEO signal | NUCLEO pin | DWM3001-CDK pin / function | Current interpretation |
|---|---|---|---|---|
| TP10 | SPI5_SCK | CN15.11 / PE15 | J10.23 / SPI1_CLK | External host clock candidate |
| TP12 | SPI5_MISO | CN15.13 / PG1 | J10.21 / SPI1_MISO | DWM-to-host return candidate |
| TP11 | SPI5_MOSI | CN15.15 / PG2 | J10.19 / SPI1_MOSI | Host-to-DWM command/data candidate |
| TP7 | DWM_CS | CN15.17 / PA3 | J10.24 / CS_RPI | External host chip-select candidate |
| TP6 | DWM_IRQ | CN15.16 / PB9 | J10.15 / GPIO_RPI | External host notification candidate |
| TP5 | DWM_RESET | CN15.33 / PD0 | J10.12 / RESET | Controlled module reset / recovery |

This supports the view that the Gateway PCB was designed around the DWM3001-CDK external-host interface route.

This does not yet prove that current DWM3001-CDK firmware configures those pins as an SPI peripheral endpoint. It establishes the hardware and documentation target for the next firmware proof.

The local Zephyr DWM3001-CDK board definition separately identifies `spi3` as an onboard nRF SPIM route, consistent with the nRF52833-to-DW3110 internal control path. That path should remain distinct from the J10 Raspberry Pi host-interface route.

## 10. Updated feasibility position

SPI peripheral / SPIS remains the preferred proof route if the J10 `SPI1_*` and `CS_RPI` pins are exposed to configurable nRF52833 peripheral logic.

The first implementation should therefore attempt a DWM-side SPI-peripheral stub on the J10 host-interface pins and a NUCLEO-side SPI host probe on the already-proven Gateway SPI5 pins.

If SPIS on those pins is not supported or conflicts with the DWM3001-CDK firmware environment, the fallback route is a UART or GPIO-mailbox proof over exposed J10 GPIO/interface pins.
---

## 11. Qorvo DWM3001CDK J10-to-DWM3001C pin mapping

The Gateway KiCad schematic confirms the NUCLEO-to-DWM3001-CDK J10 wiring for the intended host-interface route.

The Qorvo DWM3001CDK Quick Start Guide confirms the corresponding J10-to-DWM3001C module pin mapping.

| Gateway TP | Gateway signal | DWM3001-CDK J10 function | DWM3001C module pin | nRF52833 GPIO | Host-interface use |
|---|---|---|---|---|---|
| TP10 | SPI5_SCK | J10.23 / SPI1_CLK | Pin 42 | P0.31 | SPI peripheral clock input candidate |
| TP11 | SPI5_MOSI | J10.19 / SPI1_MOSI | Pin 41 | P0.27 | SPI peripheral COPI/MOSI input candidate |
| TP12 | SPI5_MISO | J10.21 / SPI1_MISO | Pin 40 | P0.07 | SPI peripheral CIPO/MISO output candidate |
| TP7 | DWM_CS | J10.24 / CS_RPI | Pin 43 | P0.30 | SPI chip-select input candidate |
| TP6 | DWM_IRQ | J10.15 / GPIO_RPI | Pin 33 | P0.28 | DWM-to-NUCLEO ready/status notification candidate |
| TP5 | DWM_RESET | J10.12 / RESET | Pin 47 | P0.18 | Reset / recovery path, not normal data path |

Additional J10 interface pins available for fallback investigation:

| DWM3001-CDK J10 function | DWM3001C module pin | nRF52833 GPIO | Possible use |
|---|---|---|---|
| J10.8 / TXD | Pin 32 | P0.15 | UART fallback candidate |
| J10.10 / RXD_RPI / RXD | Pin 34 | P0.19 | UART fallback candidate |
| J10.3 / SDA_RPI | Pin 37 | P0.26 | I2C/GPIO fallback candidate |
| J10.5 / SCL_RPI | Pin 39 | P0.23 | I2C/GPIO fallback candidate |

## 12. Feasibility conclusion after schematic and Qorvo pin-map check

The Gateway rev 0.1 hardware route is now confirmed at both sides of the interface:

```text
NUCLEO SPI5 / GPIO pins
    -> Gateway PCB nets and test points
        -> DWM3001-CDK J10 Raspberry Pi-compatible header
            -> DWM3001C / nRF52833 GPIO pins
```

This supports proceeding to a DWM-side SPI-peripheral proof using:

```text
SPIS SCK  = P0.31
SPIS MOSI = P0.27
SPIS MISO = P0.07
SPIS CS   = P0.30
IRQ/READY = P0.28
RESET     = P0.18, controlled externally through J10.12
```

The next firmware step is to create a DWM3001-CDK host-interface stub overlay that configures the J10 SPI1_* pins as an nRF SPIS endpoint, while preserving the separate onboard nRF-to-DW3110 SPI path.

The implementation remains a proof of the DWM-to-NUCLEO firmware host interface. It is not a direct NUCLEO-to-DW3110 register-control path.
