# Thin-Pod Gateway rev 0.1 Bill of Materials

**Project:** Thin-Pod Gateway<br>
**Hardware release:** rev 0.1<br>
**Manufactured carrier identity:** rev 0.1f<br>
**Status:** OSHWA certified, UID [`UK000092`](https://certification.oshwa.org/uk000092.html); manufactured, assembled and physically validated<br>
**BOM basis:** Published KiCad source and demonstrated carrier assembly

## Scope

This BOM covers the creator-designed Thin-Pod Gateway carrier PCB and the commercial modules and ordinary assembly hardware required to reproduce its functional assembly. Commercial modules remain outside the Gateway open-hardware claim.

## Carrier and modules

| Ref. | Qty | Population | Function | Part / value | Manufacturer / MPN | Interface |
|---|---:|---|---|---|---|---|
| `PCB1` | 1 | Required | Thin-Pod Gateway carrier PCB | Thin-Pod Gateway rev 0.1, manufactured build rev 0.1f | Custom two-layer PCB | Fabricate from `hardware/fabrication/rev0.1/` |
| `U1` | 1 | Required for functional Gateway | Main host and supervisor | STM32 Nucleo-144 board | STMicroelectronics / `NUCLEO-N657X0-Q` | ST Morpho connections through CN3 and CN15 |
| `U2` | 1 | Required for UWB Gateway | Gateway-side UWB subsystem | DWM3001-CDK | Qorvo / `DWM3001CDK` | J10 carrier interface |
| `U3` | 1 | Optional | Onward Wi-Fi/networking subsystem | XIAO ESP32-C6 | Seeed Studio / `XIAO ESP32C6` | XIAO carrier interface |

## Passive components and PCB features

| Ref. | Qty | Function | Part / value | Reproducibility note |
|---|---:|---|---|---|
| `R1` | 1 | DWM chip-select pull-up | 10 kΩ axial through-hole resistor | Any suitably rated compatible part |
| `R2` | 1 | XIAO chip-select pull-up | 10 kΩ axial through-hole resistor | Any suitably rated compatible part |
| `C1` | 1 | Local high-frequency decoupling | 100 nF through-hole ceramic capacitor | Suitable voltage rating |
| `C2` | 1 | Local bulk decoupling | 10 µF through-hole capacitor | Suitable voltage rating and polarity where applicable |
| `C3` | 1 | Local high-frequency decoupling | 100 nF through-hole ceramic capacitor | Suitable voltage rating |
| `C4` | 1 | Local bulk decoupling | 10 µF through-hole capacitor | Suitable voltage rating and polarity where applicable |
| `C5` | 1 | Additional local bulk storage | 22 µF through-hole capacitor | Suitable voltage rating and polarity where applicable |
| `TP1–TP13` | 13 | Electrical test access | Plated through-hole test-point features | Optional probe pins or loops may be fitted |
| `H1–H4` | 4 | Carrier mounting | M3 mounting-hole PCB features | Builder-selected fasteners and standoffs |

## Module mounting and interconnect hardware

Builder-selected pin headers and/or socket strips compatible with the NUCLEO-N657X0-Q ST Morpho interfaces, DWM3001-CDK J10 interface, and optional XIAO ESP32-C6 carrier footprint are used to assemble the carrier. Header brand, plating, socket style and standoff selection are assembly choices outside the carrier-PCB certification scope.

The released PCB geometry and published KiCad footprints define the required pitch, position and mating arrangement. Equivalent mechanically and electrically compatible interconnect hardware may be selected by the builder.

## Electrical interface summary

### NUCLEO to DWM3001-CDK

| Gateway net | NUCLEO endpoint | DWM3001-CDK endpoint |
|---|---|---|
| `SPI5_SCK` | `CN15.11 / PE15` | `J10.23 / SPI1_CLK` |
| `SPI5_MISO` / CIPO | `CN15.13 / PG1` | `J10.21 / SPI1_MISO` |
| `SPI5_MOSI` / COPI | `CN15.15 / PG2` | `J10.19 / SPI1_MOSI` |
| `DWM_CS` | `CN15.17 / PA3` | `J10.24 / CS_RPI` |
| `DWM_IRQ` | `CN15.16 / PB9` | `J10.15 / GPIO_RPI` |
| `DWM_RESET` | `CN15.33 / PD0` | `J10.12 / RESET` |
| `5V_GATEWAY` | `CN3.6 / 5V` | `J10.2 / VRPI` |
| `GND` | Common carrier ground | DWM ground interfaces |

### Optional XIAO ESP32-C6

| Gateway net | NUCLEO endpoint | XIAO endpoint |
|---|---|---|
| `SPI5_SCK` | `CN15.11 / PE15` | `D8 / GPIO19 / SCK` |
| `SPI5_MISO` / CIPO | `CN15.13 / PG1` | `D9 / GPIO20 / MISO` |
| `SPI5_MOSI` / COPI | `CN15.15 / PG2` | `D10 / GPIO18 / MOSI` |
| `C6_CS` | `CN15.19 / PD7` | `D3 / GPIO21` |
| `C6_INT` | `CN15.5 / PC1` | `D2 / GPIO2` |
| `5V_GATEWAY` | `CN3.6 / 5V` | `VBUS / 5V` |
| `GND` | Common carrier ground | `GND` |

## Verification and release reconciliation

The manufactured rev 0.1f carrier has been assembled and demonstrated through stable powered bring-up, NUCLEO-to-DWM host-interface communication, DW3110 identity and initialisation, and a matched Gateway-to-Node UWB exchange. The reconciled rev 0.1 design basis was certified by OSHWA on 16 July 2026 under UID `UK000092`.

The regenerated revision-0.1 Gerber and drill outputs preserve the intended electrical connectivity, board outline, mounting geometry, module-interface positions, test-point arrangement and layer content of the manufactured carrier. Schematic ERC and PCB DRC reports record clean results under `docs/validation/cad/rev0.1/`.

## Open-hardware and third-party boundary

The creator-controlled contribution is the Gateway carrier-board schematic, PCB layout, board-specific interfaces, test access, mechanical arrangement and derived fabrication outputs. U1, U2 and U3 are commercial third-party products and retain their manufacturers' terms.

CAD dependency provenance is recorded in [`docs/footprint-provenance.md`](../../docs/footprint-provenance.md). Creator-authored hardware source is released under `CERN-OHL-W-2.0`; this BOM and related documentation are released under `CC-BY-4.0`.

## Certification record

This reconciled BOM forms part of the Thin-Pod Gateway rev 0.1 public certification package, OSHWA UID [`UK000092`](https://certification.oshwa.org/uk000092.html), issued 16 July 2026.
