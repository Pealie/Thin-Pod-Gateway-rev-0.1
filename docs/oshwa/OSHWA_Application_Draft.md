# Thin-Pod Gateway rev 0.1: OSHWA application draft

**Status:** Preparation record; submission blocked by the editable-source gate  
**Responsible individual:** Neil Thomson / Pealie  
**Country:** United Kingdom  
**Primary project type:** Electronics  
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
- exact editable KiCad project and project-local libraries once committed;
- derived Gerber and drill outputs;
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
| BOM | Present, with several fitted-part identifiers still requiring release reconciliation |
| Fabrication package | Supplied, audited and hashed; binary archive still to be committed |
| Exact editable KiCad source | **Missing; submission blocker** |
| Symbol/footprint provenance | **Incomplete; submission blocker** |
| Public immutable release tag | Missing |
| Public repository visibility | Repository currently private |

## Licence decision

The licence combination is appropriate because it maps each work category to a recognised licence designed for that category:

- `CERN-OHL-W-2.0` preserves reciprocal access to modifications of the creator-controlled hardware design while allowing the carrier to interface with separately licensed commercial modules.
- `MIT` is OSI-approved, concise and suitable for the project-authored proof firmware, protocol code, platform adapters and Python utilities. External vendor SDK source remains excluded.
- `CC-BY-4.0` permits broad reuse of explanatory documentation and creator-owned images with attribution.

## Submission blockers

1. Commit the exact preferred-format KiCad source used for the manufactured design.
2. Include every required project-local symbol, footprint and library table.
3. Record provenance and licence treatment for each non-standard CAD dependency.
4. Set a definite design revision in the KiCad project; the supplied Gerber job currently records `rev?`.
5. Commit the binary fabrication archive and, where needed, regenerated outputs.
6. Reconcile the BOM with the fitted assembly, including exact header/socket choices and any known part numbers.
7. Run clean-environment project opening, ERC, DRC and Gerber inspection.
8. Run the OSHWA readiness checker with no blocking failures.
9. Make the repository public and create the final release tag.

## Submission judgement

The physical carrier and evidence chain are sufficiently mature to justify formal preparation. Submission at the present repository state would be premature because the editable hardware source, CAD provenance and public release freeze are central certification requirements rather than optional polish.
