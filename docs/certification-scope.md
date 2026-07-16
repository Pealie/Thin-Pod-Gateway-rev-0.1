# Thin-Pod Gateway rev 0.1: OSHWA Certification Scope

**Project:** Thin-Pod Gateway<br>
**Hardware release:** rev 0.1<br>
**Release status:** OSHWA-certified public hardware baseline<br>
**OSHWA UID:** [`UK000092`](https://certification.oshwa.org/uk000092.html)<br>
**Certification date:** 16 July 2026<br>
**Canonical release tag:** `v0.1`<br>
**Document updated:** 16 July 2026<br>

## Purpose

This document defines the certification boundary for **Thin-Pod Gateway rev 0.1**, a hardware product separate from the Thin-Pod rev 0.1 sensor-node carrier PCB.

The certified item is:

> **Thin-Pod Gateway rev 0.1: an open-hardware carrier PCB for experimental Thin-Pod UWB telemetry and Gateway-side host-interface work using commercially supplied processing and wireless development modules.**

The carrier PCB provides power distribution, local decoupling, pull-ups, test access and wired host-interface routing between an STM32 NUCLEO-N657X0-Q, a Qorvo DWM3001-CDK and an optional Seeed Studio XIAO ESP32-C6.

## Certification identity

| Item | Record |
|---|---|
| OSHWA project name | Thin-Pod Gateway |
| Version | 0.1 |
| UID | [`UK000092`](https://certification.oshwa.org/uk000092.html) |
| Certification date | 16 July 2026 |
| Country | United Kingdom |
| Primary project type | Electronics |
| Hardware licence | `CERN-OHL-W-2.0` |
| Software licence | `MIT` |
| Documentation licence | `CC-BY-4.0` |

## Relationship to Thin-Pod rev 0.1

The sensor node and Gateway are independently certified products.

| Product | Revision | OSHWA record |
|---|---:|---|
| Thin-Pod sensor-node carrier PCB | rev 0.1 | [`UK000091`](https://certification.oshwa.org/uk000091.html), certified 28 May 2026 |
| Thin-Pod Gateway carrier PCB | rev 0.1 | [`UK000092`](https://certification.oshwa.org/uk000092.html), certified 16 July 2026 |

The shared revision number indicates the first hardware release of each product family. Each UID has its own design, evidence and licensing boundary.

## Fabrication-build identity

The manufactured board may carry the silkscreen/build identifier `rev 0.1f`. The repository retains that marking as the physical fabrication-build identity within the certified Gateway rev 0.1 record.

The public source and fabrication package use release revision `0.1`. The relationship between the physical marking and public release is recorded in the README, fabrication manifest and validation evidence.

## Included in the certification scope

| Scope element | Description |
|---|---|
| Gateway carrier PCB | Creator-designed two-layer interconnect carrier board |
| NUCLEO mechanical/electrical interface | Carrier-board connections to the commercial NUCLEO-N657X0-Q |
| DWM3001-CDK mechanical/electrical interface | Carrier-board connections to the commercial Gateway-side UWB development board |
| XIAO ESP32-C6 mechanical/electrical interface | Carrier-board connections to the optional commercial onward-networking module |
| Power distribution | `5V_GATEWAY`, `3V3_GATEWAY` auxiliary rail and common-ground routing |
| Local decoupling and pull-ups | Passive circuitry fitted on the carrier board |
| Host-interface wiring | Shared SPI5 SCK/COPI/CIPO route and separate chip-select, interrupt and reset lines |
| Test access | TP1–TP13 observability points |
| Mounting geometry | Carrier-board mechanical support and module placement |
| Editable hardware source | KiCad project, schematic and PCB source |
| Fabrication outputs | Gerber, drill, job and fabrication-archive outputs derived from the release source |
| BOM and documentation | Public sourcing, assembly, bring-up, verification, provenance and scope records |
| Project-authored software | MIT-licensed firmware, scripts, protocol code, checkers and reference utilities published in this repository |

## Outside the certification scope

| Excluded item | Boundary |
|---|---|
| STM32 NUCLEO-N657X0-Q internal design | Commercial STMicroelectronics development board |
| Qorvo DWM3001-CDK and DW3110 internal design | Commercial Qorvo development board and device |
| Seeed Studio XIAO ESP32-C6 internal design | Commercial optional networking module |
| External Qorvo DW3 SDK source | Separately licensed vendor dependency; not distributed or relicensed here |
| Thin-Pod sensor-node PCB | Separate certified hardware product, UID `UK000091` |
| Complete Node-to-Gateway vibration-window transport | Later system milestone outside the carrier-board certification claim |
| Ranging and field RF qualification | Separate implementation and validation work |
| DSP, TinyML and fault-detection performance | Later analysis functionality and performance evidence |
| Secure boot, production OTA and full security assurance | Separate implementation and assurance work |
| PoE system certification | External supply/integration matter |
| Radio, EMC, electrical safety, CE or UKCA compliance | Separate regulatory assessment |
| Later SMT/chip-down Gateway revision | Future hardware release outside rev 0.1 |

## Commercial module boundary

The BOM identifies the NUCLEO, DWM3001-CDK and XIAO where required to reproduce the functional assembly. BOM inclusion documents the assembly; it does not place those commercial products under the Thin-Pod hardware licence.

The creator-controlled open-hardware contribution comprises the Gateway carrier-board design, project-authored source, project-authored interface material and derived fabrication outputs. Third-party CAD dependencies retain their recorded original terms.

## Licensing declarations

| Material category | Certified treatment |
|---|---|
| Creator-designed Gateway hardware source | `CERN-OHL-W-2.0` |
| Creator-authored documentation and owned images | `CC-BY-4.0` |
| Project-authored firmware and software | `MIT` |
| Third-party CAD dependencies | Original applicable terms and attribution |
| Commercial development modules | Identified for reproduction; original ownership and licensing retained |

The OSHWA certification mark remains subject to OSHWA's certification-mark licence and branding requirements.

## Evidence basis

The certification package includes:

- manufactured and assembled carrier-board evidence;
- stable powered bring-up and rail measurements;
- verified power, ground, RESET and READY behaviour;
- bidirectional NUCLEO-to-DWM `TPHIP GET_CAPABILITIES` exchange;
- physical DW3110 identity and initialisation evidence;
- a matched 20-frame Gateway-to-Node UWB exchange;
- published native KiCad source;
- a hashed fabrication package;
- clean ERC and DRC reports;
- Gerber and drill inspection;
- BOM reconciliation;
- symbol and footprint provenance;
- repository and CI checks;
- immutable public release tag `v0.1`.

## Post-certification maintenance boundary

1. Preserve `v0.1` as the canonical certified rev 0.1 hardware baseline.
2. Keep documentation-only corrections explicit and traceable.
3. Assign any electrical, mechanical, copper or interface design change to a later hardware revision.
4. Preserve the separation between UIDs `UK000091` and `UK000092`.
5. Use the certification mark only with the Gateway UID and in accordance with OSHWA requirements.
6. Retain pre-certification drafts and engineering logs as dated historical records rather than treating their former status language as current.

## Conclusion

Thin-Pod Gateway rev 0.1 is an OSHWA-certified open-hardware carrier PCB under UID `UK000092`. The certification covers the declared creator-controlled hardware, associated open documentation and project-authored MIT-licensed software package. Commercial modules, wider system performance and regulatory approvals remain outside this certification boundary.
