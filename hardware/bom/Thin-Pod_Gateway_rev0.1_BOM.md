# Thin-Pod Gateway rev 0.1 Bill of Materials

**Project:** Thin-Pod Gateway  
**Hardware release path:** rev 0.1  
**Ordered fabrication-build identifier:** `rev 0.1f`, where present on the manufactured PCB/silkscreen  
**Release status:** OSHWA candidate documentation; PCB ordered and physical bring-up pending  
**BOM status:** Complete design-basis BOM for the ordered carrier-board implementation, with procurement and footprint identifiers marked for final reconciliation where the available release evidence does not yet fix them exactly  
**Document date:** 27 May 2026  

## 1. Purpose and release boundary

This Bill of Materials documents the ordered **Thin-Pod Gateway rev 0.1** carrier-board architecture. The currently ordered PCB may be marked `rev 0.1f`; that marking is treated as the pre-release fabrication-build identity within the Gateway rev 0.1 release path.

The Gateway is a separate hardware product from **Thin-Pod rev 0.1**, the sensor-node carrier PCB already submitted to OSHWA. Nothing in this BOM changes or extends the certification boundary of the submitted Thin-Pod node.

The present Gateway design is a carrier board intended to interconnect three commercially supplied development modules:

```text
STM32 NUCLEO-N657X0-Q       main host / prospective analytic supervisor
Qorvo DWM3001-CDK           Gateway-side UWB subsystem
Seeed Studio XIAO ESP32-C6  optional onward-networking subsystem
```

The commercial modules are included in this BOM because they are required to understand and reproduce the functional assembly. Their inclusion does not claim that they are Thin-Pod-authored open hardware or covered by the Gateway hardware licence.

## 2. Rev 0.1 design summary

| Design characteristic | Recorded rev 0.1 / ordered rev 0.1f position |
|---|---|
| Board type | Creator-designed carrier PCB for commercial modules |
| Layer count | Two copper layers |
| Approximate board dimensions | 160 mm × 145 mm |
| PCB thickness setting recorded in design audit | 1.6 mm |
| Ground plane | Filled `GND` zone on `B.Cu` |
| Mechanical mounting | Four M3 mounting holes |
| Primary host module | STM32 `NUCLEO-N657X0-Q` |
| UWB module | Qorvo `DWM3001-CDK` |
| Optional network/backhaul module | Seeed Studio `XIAO ESP32-C6` |
| Host bus strategy | NUCLEO SPI5 master with shared SCK/MOSI/MISO and separate CS/interrupt/control routes |
| Main module power rail | `5V_GATEWAY` from NUCLEO `CN3.6` |
| Auxiliary/pull-up rail | `3V3_GATEWAY` from NUCLEO `CN3.16` |
| Test access | TP1–TP13 |
| OSHWA status | Gateway certification not yet submitted; physical verification required before submission |
| Deferred later direction | `rev 0.3` SMT/chip-down work is not part of this BOM |

## 3. Populated and functional assembly BOM

The table below distinguishes required carrier-board parts, functional commercial modules, optional modules and design features whose hardware population depends on assembly practice.

| Ref. | Qty | Population | Function | Part / value | Manufacturer / MPN | Footprint or interface | Release / verification note |
|---|---:|---|---|---|---|---|---|
| `PCB1` | 1 | Required | Thin-Pod Gateway carrier PCB | Thin-Pod Gateway rev 0.1; ordered fabrication build marked rev 0.1f | PCB fabricator / Custom fabrication item | `Two-layer PCB; approximately 160 mm × 145 mm; four M3 mounting holes` | Normative source is the final committed KiCad design; reconcile rev 0.1f physical build to rev 0.1 release before OSHWA submission. |
| `U1` | 1 | Required for functional Gateway assembly | Main Gateway host and prospective analytic supervisor | STM32 Nucleo-144 development board with STM32N657X0 MCU | STMicroelectronics / NUCLEO-N657X0-Q | `Gateway carrier interface through ST morpho-header connections CN3 and CN15` | Use supported NUCLEO power configuration during bring-up; record exact supply route and jumper setting. |
| `U2` | 1 | Required for UWB Gateway assembly | Gateway-side UWB communications subsystem | DWM3001-CDK development board | Qorvo / DWM3001CDK | `CDK carrier/interface footprint using J10 Raspberry Pi-compatible header connections` | Rev 0.1 uses the CDK module board, not the later DWM3001C chip-down/module-only design. |
| `U3` | 1 | Optional for onward-networking assembly | Optional Wi-Fi / onward-networking subsystem | Seeed Studio XIAO ESP32-C6 | Seeed Studio / XIAO ESP32C6 — confirm exact purchasable SKU before Gateway release freeze | `XIAO-ESP32-C6-DIP carrier interface` | Do not fit during the first NUCLEO-only power validation stage. |
| `R1` | 1 | Required | DWM chip-select pull-up | 10 kΩ, axial through-hole resistor | Generic / To be selected / record fitted MPN before release freeze | `Through-hole axial resistor footprint — confirm exact KiCad identifier from final source` | Expected to hold U2 inactive during host reset/start-up. |
| `R2` | 1 | Required | XIAO chip-select pull-up | 10 kΩ, axial through-hole resistor | Generic / To be selected / record fitted MPN before release freeze | `Through-hole axial resistor footprint — confirm exact KiCad identifier from final source` | Expected to hold optional U3 inactive during host reset/start-up. |
| `C1` | 1 | Required | Local high-frequency supply decoupling | 100 nF, through-hole ceramic capacitor | Generic / To be selected / record fitted MPN before release freeze | `Through-hole capacitor footprint — confirm exact KiCad identifier from final source` | Specified in audited ordered design; population/rail check pending |
| `C2` | 1 | Required | Local bulk supply decoupling | 10 µF, through-hole capacitor | Generic / To be selected / record fitted MPN and polarity status before release freeze | `Through-hole capacitor footprint — confirm exact KiCad identifier from final source` | Observe polarity at assembly if a polarised part is selected. |
| `C3` | 1 | Required | Local high-frequency supply decoupling | 100 nF, through-hole ceramic capacitor | Generic / To be selected / record fitted MPN before release freeze | `Through-hole capacitor footprint — confirm exact KiCad identifier from final source` | Specified in audited ordered design; population/rail check pending |
| `C4` | 1 | Required | Local bulk supply decoupling | 10 µF, through-hole capacitor | Generic / To be selected / record fitted MPN and polarity status before release freeze | `Through-hole capacitor footprint — confirm exact KiCad identifier from final source` | Observe polarity at assembly if a polarised part is selected. |
| `C5` | 1 | Required | Additional local bulk energy storage | 22 µF, through-hole capacitor | Generic / To be selected / record fitted MPN and polarity status before release freeze | `Through-hole capacitor footprint — confirm exact KiCad identifier from final source` | Observe polarity at assembly if a polarised part is selected. |
| `H1–H4` | 4 | PCB feature; mounting hardware optional | Carrier-board mounting holes | M3 mounting holes | N/A for drilled feature | `Mounting-hole footprints — confirm exact KiCad identifiers from final source` | Exact standoff and fastener selection is installation dependent and not yet fixed as a supplied assembly item. |
| `ASSEMBLY-IF` | As required | Required according to chosen module mounting method | Headers / sockets required to mate commercial modules to the Gateway carrier PCB | Pin headers and/or receptacle/socket strips matching U1, U2 and fitted U3 interfaces | To be selected / To be selected before assembly/release freeze | `Determined by carrier-board module footprints and stacking/clearance arrangement` | Not a schematic reference designator; included because a reproducible assembled Gateway requires an explicit socket/header decision. |

## 4. Test-point map

The ordered rev 0.1f implementation contains a deliberately extensive test-point set. These are PCB features; optional fitted probe pins or loops may be selected during assembly.

| Ref. | Net / value | Measurement purpose | Related interface |
|---|---|---|---|
| `TP1` | `GND_DWM` | DWM-side ground reference | Qorvo subsystem ground |
| `TP2` | `5V_GATEWAY` | Gateway 5 V rail measurement | NUCLEO-derived module supply |
| `TP3` | `GND_C6` | XIAO-side ground reference | Optional networking subsystem ground |
| `TP4` | `3V3_GATEWAY` | Auxiliary / chip-select pull-up rail measurement | NUCLEO 3.3 V auxiliary rail |
| `TP5` | `DWM_RESET` | DWM reset observation/control | NUCLEO `CN15.33 / PD0` to DWM `J10.12 / RESET` |
| `TP6` | `DWM_IRQ` | DWM interrupt/data-ready observation | NUCLEO `CN15.16 / PB9` to DWM `J10.15 / GPIO_RPI` |
| `TP7` | `DWM_CS` | DWM host-select observation | NUCLEO `CN15.17 / PA3` to DWM `J10.24 / CS_RPI`; pulled up by `R1` |
| `TP8` | `C6_CS` | XIAO host-select observation | NUCLEO `CN15.19 / PD7` to XIAO `D3 / GPIO21`; pulled up by `R2` |
| `TP9` | `C6_INT` | XIAO interrupt/data-ready observation | NUCLEO `CN15.5 / PC1` to XIAO `D2 / GPIO2` |
| `TP10` | `SPI5_SCK` | Shared SPI clock observation | NUCLEO to DWM/XIAO clock route |
| `TP11` | `SPI5_MOSI` | Shared host-to-module data observation | NUCLEO to DWM/XIAO MOSI route |
| `TP12` | `SPI5_MISO` | Shared module-to-host data observation | DWM/XIAO to NUCLEO MISO route |
| `TP13` | `5V_NUCLEO` | NUCLEO source-rail confirmation | Input source for `5V_GATEWAY` distribution |

## 5. Electrical interface basis recorded by the BOM

### 5.1 Power roles

| Net | Origin | Load / role |
|---|---|---|
| `5V_GATEWAY` | NUCLEO `CN3.6 / 5V` | Powers DWM3001-CDK through its `J10.2 / VRPI` input path and powers XIAO pin 14 / `VBUS / 5V` |
| `3V3_GATEWAY` | NUCLEO `CN3.16 / 3V3` | Pull-up / auxiliary rail for logic functions and test access |
| `GND` | Common carrier-board reference | Connects NUCLEO, DWM3001-CDK, XIAO and passive returns |
| XIAO pin 12 / `3V3` | Not carrier-powered in rev 0.1 | Must remain unconnected to the Gateway power rails unless a later design change is explicitly made |

### 5.2 NUCLEO to DWM3001-CDK host route

| Gateway net | NUCLEO endpoint | DWM3001-CDK endpoint | BOM relevance |
|---|---|---|---|
| `SPI5_SCK` | `CN15.11 / PE15` | `J10.23 / SPI1_CLK` | Routed signal exposed at `TP10` |
| `SPI5_MISO` | `CN15.13 / PG1` | `J10.21 / SPI1_MISO` | Routed signal exposed at `TP12` |
| `SPI5_MOSI` | `CN15.15 / PG2` | `J10.19 / SPI1_MOSI` | Routed signal exposed at `TP11` |
| `DWM_CS` | `CN15.17 / PA3` | `J10.24 / CS_RPI` | Pulled up by `R1`, observed at `TP7` |
| `DWM_IRQ` | `CN15.16 / PB9` | `J10.15 / GPIO_RPI` | Observed at `TP6` |
| `DWM_RESET` | `CN15.33 / PD0` | `J10.12 / RESET` | Observed at `TP5` |
| `5V_GATEWAY` | `CN3.6 / 5V` | `J10.2 / VRPI` | Module power path |
| `GND` | Relevant common-ground pins | Relevant CDK GND pins plus `J1_2` where represented | Shared return path |

### 5.3 NUCLEO to optional XIAO ESP32-C6 route

| Gateway net | NUCLEO endpoint | XIAO endpoint | BOM relevance |
|---|---|---|---|
| `SPI5_SCK` | `CN15.11 / PE15` | Pin 9, `D8 / GPIO19 / SCK` | Routed signal exposed at `TP10` |
| `SPI5_MISO` | `CN15.13 / PG1` | Pin 10, `D9 / GPIO20 / MISO` | Routed signal exposed at `TP12` |
| `SPI5_MOSI` | `CN15.15 / PG2` | Pin 11, `D10 / GPIO18 / MOSI` | Routed signal exposed at `TP11` |
| `C6_CS` | `CN15.19 / PD7` | Pin 4, `D3 / GPIO21` | Pulled up by `R2`, observed at `TP8` |
| `C6_INT` | `CN15.5 / PC1` | Pin 3, `D2 / GPIO2` | Observed at `TP9` |
| `5V_GATEWAY` | `CN3.6 / 5V` | Pin 14, `VBUS / 5V` | Optional-module power path |
| `GND` | Common ground | Pin 13, `GND` | Shared return path |
| `3V3_GATEWAY` | `CN3.16 / 3V3` | Not connected to XIAO pin 12 | Confirms rev 0.1 power-domain boundary |

## 6. Commercial module and open-hardware boundary

The Gateway BOM intentionally includes commercial modules required to reproduce the physical Gateway assembly. BOM inclusion is not a licence claim.

| Reference | Commercial item | Included because | Open-hardware treatment |
|---|---|---|---|
| `U1` | STM32 `NUCLEO-N657X0-Q` | The carrier PCB is physically and electrically built around the host-board interface | Proprietary commercial development board; not covered by the Gateway CERN-OHL-W-2.0 grant |
| `U2` | Qorvo `DWM3001-CDK` | It is the Gateway-side UWB development subsystem in rev 0.1 | Proprietary commercial development board; not covered by the Gateway CERN-OHL-W-2.0 grant |
| `U3` | Seeed Studio `XIAO ESP32-C6` | It is the optional onward-networking subsystem provided for by the carrier PCB | Proprietary commercial module; not covered by the Gateway CERN-OHL-W-2.0 grant |

The creator-controlled open-hardware contribution is the Gateway carrier-board design, its editable KiCad source, creator-authored interface source where applicable, fabrication outputs and documentation. Non-standard CAD/library dependencies must receive a separate provenance and licence record before Gateway OSHWA submission.

## 7. Population options

| Assembly form | Required population | Purpose |
|---|---|---|
| Bare PCB inspection state | `PCB1` only | Visual, Gerber-to-board and unpowered PCB checks |
| NUCLEO-only bring-up state | `PCB1`, passives, required interface hardware, `U1` | Validate Gateway rails and idle pull-ups before fitting downstream modules |
| Core Gateway UWB state | NUCLEO-only state plus `U2` | Validate DWM power, reset/IRQ/SPI route and later host-buffer transfer |
| Full optional networking state | Core Gateway UWB state plus `U3` | Evaluate onward-networking after the DWM-to-NUCLEO path is proven |

## 8. Procurement and final-release confirmation register

This BOM is complete as a design-basis record of the ordered rev 0.1f carrier implementation. The following procurement and source identifiers must be confirmed before a later Gateway OSHWA certification application is submitted.

| Confirmation item | Why it remains open | Required close-out evidence |
|---|---|---|
| Exact orderable XIAO ESP32-C6 SKU | Functional module identity is known; purchasing SKU has not been frozen in the design record | Supplier/part-number entry in final BOM |
| Exact resistor MPNs and tolerance/rating | Design fixes value and role, not procurement choice | Fitted or approved substitution list |
| Exact capacitor MPNs, dielectric/voltage rating and any polarity | Design fixes value/function, not full procurement choice | Fitted or approved substitution list and assembly note |
| Exact KiCad footprint identifiers for R/C/test-point/mechanical features | Design audit records component form but not every library token | Final committed source-to-BOM reconciliation |
| Exact header/socket/standoff arrangement | Required for physical module mounting but dependent on assembly/clearance choice | Mechanical fit record and final BOM line items |
| Interface footprint provenance for U1/U2/U3 | Required for public OSHWA release source | `docs/footprint-provenance.md` completed before application |
| Physical population/bring-up result | PCB is ordered but has not yet arrived and been tested | Completed `docs/gateway-bring-up-and-verification-protocol.md` evidence |

## 9. Items explicitly not included in rev 0.1 BOM

The following later-design items are **not** part of this rev 0.1 carrier-board BOM:

| Deferred item | Reason |
|---|---|
| Castellated `DWM3001C` module fitted directly to the Gateway PCB | Reserved for later SMT/chip-down direction, presently rev 0.3 |
| Local `3V3_DWM` low-noise regulator, SWD/UART service interface or RF keep-out implementation for a bare DWM3001C | Required only when the CDK is replaced by the module |
| Protected/isolation power-path refinements for the XIAO | Candidate later design improvement, not present in the ordered rev 0.1f carrier implementation unless explicitly revised |
| Integrated PoE powered-device front end | Not implemented by the current Gateway and outside rev 0.1 |
| Thin-Pod sensor-node hardware | Separate product; already handled in its own rev 0.1 repository and OSHWA submission |
| Firmware, DSP or TinyML dependencies | System/software work outside this carrier-board BOM |

## 10. Source and supporting references

### Project design record

This BOM is based on the previously reviewed Gateway KiCad source and ordered fabrication package recorded in the project documentation:

- `Thin-Pod Gateway(7).kicad_sch`
- `Thin-Pod Gateway(2).kicad_pcb`
- `ThinPodGatewayFabricationPullupsCouplingCapsRev0.1f-Ordered.zip`
- `docs/gateway-bring-up-and-verification-protocol.md`
- `docs/system-interface-control-document.md`

### Manufacturer references

- STMicroelectronics, `NUCLEO-N657X0-Q`: https://www.st.com/en/evaluation-tools/nucleo-n657x0-q.html
- Qorvo, `DWM3001CDK`: https://www.qorvo.com/products/p/DWM3001CDK
- Seeed Studio, `XIAO ESP32C6`: https://wiki.seeedstudio.com/xiao_esp32c6_getting_started/

### Certification reference

- OSHWA Documentation Guidance: https://certification.oshwa.org/process/documentation.html

## 11. Licensing

This BOM is creator-authored Gateway documentation intended to be released under **Creative Commons Attribution 4.0 International (`CC-BY-4.0`)**, consistent with `LICENSE-DOCUMENTATION.md`.

The creator-designed Gateway hardware source is intended to be licensed separately under **CERN Open Hardware Licence Version 2 — Weakly Reciprocal (`CERN-OHL-W-2.0`)**, consistent with `LICENSE-HARDWARE.md`.

Commercial development boards, components, vendor documentation and third-party CAD/library files remain subject to their respective rights holders’ terms and are not relicensed merely by being listed in this BOM.
