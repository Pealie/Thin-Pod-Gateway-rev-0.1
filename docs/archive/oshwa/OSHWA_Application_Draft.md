# Archived pre-certification application draft

> **Historical record.** This document preserves the pre-certification application draft as it existed before OSHWA certification was issued on 16 July 2026. The current public record is [`oshwa/application-record.md`](../../../oshwa/application-record.md), and the official directory entry is [`UK000092`](https://certification.oshwa.org/uk000092.html).

# Thin-Pod Gateway rev 0.1: OSHWA application draft

**Status:** Preparation record; native source and fabrication package published, final validation and release freeze outstanding<br>
**Responsible individual:** Neil Thomson / Pealie<br>
**Country:** United Kingdom<br>
**Primary project type:** Electronics<br>
**Related certified project:** Thin-Pod rev 0.1, OSHWA UID `UK000091`

## Proposed application fields

| Field | Proposed value |
|---|---|
| Project name | Thin-Pod Gateway |
| Project version | rev 0.1 |
| Project website | Public tagged release of `Pealie/Thin-Pod-Gateway-rev-0.1` |
| Documentation URL | Same public tagged release |
| Hardware licence | CERN Open Hardware Licence Version 2, Weakly Reciprocal (`CERN-OHL-W-2.0`) |
| Software licence | MIT License (`MIT`) |
| Documentation licence | Creative Commons Attribution 4.0 International (`CC-BY-4.0`) |
| Project type | Electronics |
| Certification relationship | Separate project; does not reuse or extend UID `UK000091` |

## Proposed short description

Thin-Pod Gateway rev 0.1 is an open-hardware carrier PCB for experimental Thin-Pod UWB telemetry and Gateway-side host-interface work. It provides power distribution, local decoupling, pull-ups, test access and SPI/control routing between a commercial STM32 NUCLEO-N657X0-Q host, Qorvo DWM3001-CDK UWB subsystem and optional Seeed XIAO ESP32-C6 onward-networking module. The open contribution is the creator-designed carrier PCB, editable design source, fabrication outputs, BOM, documentation and project-authored software. The commercial modules remain outside the open-hardware claim.

## Proposed certification scope

### Included

- creator-designed two-layer Gateway carrier PCB;
- NUCLEO, DWM3001-CDK and optional XIAO carrier interfaces;
- 5 V and 3.3 V distribution, common-ground routing and local decoupling;
- DWM and XIAO chip-select pull-ups;
- shared SPI5 SCK/COPI/CIPO routing and separate control lines;
- TP1–TP13 test access and mounting geometry;
- published native KiCad project, schematic and PCB under `hardware/source/kicad/rev0.1/`;
- published Gerber, drill and fabrication archive under `hardware/fabrication/rev0.1/`;
- BOM, assembly, bring-up, validation and provenance documentation;
- Thin-Pod-authored firmware, scripts and test utilities under MIT.

### Excluded

- internal designs of the STMicroelectronics NUCLEO-N657X0-Q;
- internal designs of the Qorvo DWM3001-CDK and DW3110;
- internal design of the Seeed XIAO ESP32-C6;
- external Qorvo DW3 SDK source distributed under `LicenseRef-QORVO-2`;
- the separately certified Thin-Pod rev 0.1 sensor-node carrier PCB;
- unverified Node-to-Gateway vibration-window transport;
- ranging, field RF qualification, security assurance and regulatory approval;
- Gateway DSP, TinyML and predictive-maintenance performance claims.

## Evidence available

| Evidence | Status |
|---|---|
| Manufactured and assembled carrier PCB | Complete |
| Stable PoE-powered NUCLEO and DWM bring-up | PASS |
| Power, ground, RESET and READY measurements | PASS |
| Bidirectional NUCLEO-to-DWM TPHIP exchange | PASS |
| Physical DW3110 identity and initialisation | PASS |
| Matched 20-frame Gateway-to-Node UWB exchange | PASS |
| Raw validation logs and automated checkers | Present |
| Native KiCad source | Published for revision `0.1` |
| Fabrication package | Published, hashed and revision-labelled `0.1` |
| Gerber job revision | `0.1`; earlier `rev?` defect resolved |
| BOM | Present; fitted-part and header/socket reconciliation remains open |
| Symbol/footprint provenance | Incomplete; release blocker |
| Clean-environment KiCad opening, ERC and DRC | Outstanding; release blocker |
| Gerber-viewer inspection record | Outstanding; release blocker |
| Public immutable release tag | Missing |
| Public repository visibility | Repository currently private |

## Licence decision

The licence combination maps each work category to a recognised licence designed for that category:

- `CERN-OHL-W-2.0` preserves reciprocal access to modifications of the creator-controlled hardware design while allowing the carrier to interface with separately licensed commercial modules.
- `MIT` is OSI-approved, concise and suitable for the project-authored proof firmware, protocol code, platform adapters and Python utilities. External vendor SDK source remains excluded.
- `CC-BY-4.0` permits broad reuse of explanatory documentation and creator-owned images with attribution.

## Remaining submission blockers

1. Open the published project in a clean KiCad 10 environment and confirm that all required symbols and footprints resolve.
2. Record schematic ERC and PCB DRC results.
3. Inspect and record the published Gerber and drill package in a Gerber viewer.
4. Record provenance and licence treatment for each non-standard CAD dependency.
5. Reconcile the BOM with the fitted assembly, including exact header/socket choices and known fitted part identifiers.
6. Confirm that the regenerated revision-labelled fabrication package preserves the intended electrical and geometric design of the manufactured rev 0.1f carrier.
7. Run the OSHWA readiness checker with no blocking failures.
8. Make the repository public and create the final immutable release tag.

## Submission judgement

The physical carrier, editable source, fabrication package and evidence chain are sufficiently mature for final certification preparation. Submission should follow completion of the clean-environment CAD checks, BOM and provenance reconciliation, public repository transition and immutable release freeze.