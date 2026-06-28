# Thin-Pod Gateway OSHWA Preparation Checklist

## Purpose

This document prepares the Thin-Pod Gateway rev 0.1 repository for a later OSHWA certification submission.

This document directly supports Gateway completion checklist Item 10: OSHWA certification preparation.

No new Gateway architecture scope is introduced here. This checklist records certification-readiness evidence, source boundaries, licensing checks, and remaining gaps for the Gateway rev 0.1 release.

## OSHWA preparation status

| Area | Current status | Notes |
|---|---|---|
| Certification scope | Drafted | Gateway rev 0.1 PCB, documentation, validation evidence, and project-authored software |
| Third-party module boundary | Drafted | NUCLEO, DWM3001-CDK, and XIAO ESP32-C6 treated as commercial/development modules |
| Hardware licence | Present / verify before submission | Confirm exact licence file and README wording before submission |
| Software licence | Present / verify before submission | Confirm scripts and firmware are covered by OSI-approved software licence |
| Documentation licence | Present / verify before submission | Confirm documentation licence wording before submission |
| Hardware design files | TBD | KiCad source, fabrication outputs, BOM, and assembly notes should be complete |
| Validation evidence | In progress | Validation log exists; live hardware and transfer evidence still pending |
| Tagged release | Not yet | Required before final submission |
| OSHWA mark usage | Not yet | Do not use Gateway OSHWA mark until certification is granted |

## Proposed certification scope statement

Thin-Pod Gateway rev 0.1 is an open hardware Gateway carrier/interface PCB and associated minimal firmware/software evidence package. It integrates commercially available development modules, including the STM32 NUCLEO-N657X0-Q, DWM3001-CDK, and Seeed XIAO ESP32-C6.

The intended OSHWA certification scope applies to:

- the Thin-Pod Gateway rev 0.1 PCB design files;
- project-authored documentation;
- project-authored validation evidence;
- project-authored firmware and software contained in this repository;
- BOM, assembly, bring-up, and verification material needed to reproduce the Gateway carrier/interface PCB.

The intended certification scope does not claim openness over the internal designs of third-party development modules.

## Boundary statement for README or OSHWA submission

Thin-Pod Gateway rev 0.1 is an open hardware Gateway carrier/interface PCB that connects a NUCLEO-N657X0-Q host, a DWM3001-CDK UWB module, and a Seeed XIAO ESP32-C6 auxiliary module. The open hardware scope applies to the Gateway PCB design files, documentation, validation evidence, and project-authored firmware/software in this repository. The commercial development modules are identified as third-party modules and are not claimed as part of the project-authored open hardware design.

## OSHWA basis

OSHWA certification is a self-certification process for projects that comply with the community definition of open source hardware. Certified projects receive permission to use the OSHWA certification mark and receive a unique ID for discoverability.

OSHWA expects certified hardware projects to provide documentation including design files, and to allow modification and distribution of those design files. OSHWA also requires an open source licence for hardware components and an OSI-approved licence for software necessary to operate the hardware.

References:

- https://certification.oshwa.org/basics.html
- https://certification.oshwa.org/process.html
- https://certification.oshwa.org/process/hardware.html
- https://certification.oshwa.org/process/software.html
- https://certification.oshwa.org/process/documentation.html
- https://certification.oshwa.org/process/branding.html
- https://certification.oshwa.org/mark-usage.html

## Repository readiness checklist

### 1. Repository visibility

| Check | Status | Evidence / notes |
|---|---|---|
| Repository is public or can be made public for submission | TBD | Public visibility required for practical review/discovery |
| Correct repository selected for Gateway, not node | In progress | `Thin-Pod-Gateway-rev-0.1` |
| Completion branch exists | Done | `gateway-rev0.1-completion-checklist` |
| Final release tag exists | TBD | Suggested tag: `v0.1.0` |
| README clearly describes Gateway rev 0.1 | TBD | README should be checked before release |
| README distinguishes Gateway from Thin-Pod node | TBD | Important because node already has separate OSHWA certification |

### 2. Certification scope clarity

| Check | Status | Evidence / notes |
|---|---|---|
| Gateway PCB design is identified as project-authored open hardware | TBD | Add to README and release notes |
| Third-party modules are clearly identified | Drafted | NUCLEO-N657X0-Q, DWM3001-CDK, XIAO ESP32-C6 |
| Internal designs of third-party modules are excluded from claim | Drafted | Boundary statement above |
| Gateway firmware/software artefacts are identified | In progress | `firmware/gateway_probe/`, `scripts/` |
| Rev 0.1 limitations are stated | In progress | Validation log includes limits |
| No Gateway OSHWA mark used before certification | Confirm before release | Do not use certification mark until granted |

### 3. Hardware source files

| Check | Status | Expected path / notes |
|---|---|---|
| KiCad schematic source present | TBD | Suggested: `hardware/source/` |
| KiCad PCB layout source present | TBD | Suggested: `hardware/source/` |
| Custom symbols/footprints included if required | TBD | Include if not standard library-only |
| Fabrication outputs present | TBD | Suggested: `hardware/fabrication/` |
| Gerber files present | TBD | Suggested: `hardware/fabrication/gerbers/` |
| Drill files present | TBD | Suggested: `hardware/fabrication/` |
| PCB manufacturer notes present | TBD | Include Newbury / TrainPCB notes if useful |
| Pick-and-place file present if available | TBD | Optional for hand-assembled board but helpful |
| Assembly drawing or placement image present | TBD | Helpful for reproducibility |
| Schematic PDF/export present | TBD | Helpful, but source files remain primary |
| PCB image/render present | TBD | Helpful for readers |
| BOM present | In progress | `hardware/bom/` visible in repo |
| BOM includes manufacturer/module details | TBD | Ensure commercial modules are named clearly |
| BOM distinguishes project PCB parts from third-party modules | TBD | Important for certification boundary |

### 4. Hardware documentation

| Check | Status | Evidence / notes |
|---|---|---|
| Hardware bring-up note exists | Done | `docs/bringup/Gateway_rev0_1_Hardware_Bringup_Note.md` |
| Bring-up note includes final measured voltages | TBD | Awaiting final bench values |
| Bring-up note records ground return correction | In progress | Existing note shell includes issue/resolution |
| Final verified pin map exists | In progress | `docs/firmware/NUCLEO_SPI5_Gateway_PCB_Pin_Mapping.md` |
| Pin map includes final continuity/probe status | TBD | Awaiting final entries |
| Power distribution is documented | In progress | Pin map and bring-up note |
| Test points are documented | In progress | Pin map should remain source of truth |
| Safety and power limits are documented | TBD | Battery/PoE boundaries and bench-use limitations |
| Known limitations are documented | In progress | Validation log and README should include release limits |

### 5. Firmware and software artefacts

| Check | Status | Evidence / notes |
|---|---|---|
| Minimal SPI/GPIO probe app exists | Done / scaffold | `firmware/gateway_probe/` |
| Probe app has README | Done | `firmware/gateway_probe/README.md` |
| Probe app build/run instructions present | Partial | Add exact board/toolchain command once confirmed |
| Probe app has live hardware output evidence | TBD | Awaiting bench run |
| Telemetry packet definition exists | Done | `docs/protocol/ThinPod_Gateway_Telemetry_Packet_v1.md` |
| Sample telemetry packet exists | Done | `samples/telemetry_packet_v1_sample.hex` |
| Parser/logging script exists | Done | `scripts/gateway_packet_logger.py` |
| Parser tested against sample packet | Done | Validation log records output summary |
| Scripted release artefact check exists | Done | `scripts/check_gateway_release_artifacts.py` |
| GitHub Actions workflow exists | Done | `.github/workflows/gateway-checks.yml` |
| GitHub Actions workflow passes | Done | GitHub run succeeded on commit `0c858b6` |
| Live node-to-Gateway transfer parser output exists | TBD | Awaiting Item 5 |

### 6. Validation evidence

| Check | Status | Evidence / notes |
|---|---|---|
| Validation log exists | Done | `docs/validation/Gateway_rev0_1_Validation_Log.md` |
| Validation log includes artifact check output | Done | Current-stage local output recorded |
| Validation log includes GitHub Actions result | TBD | Add latest GitHub Actions success entry |
| Validation log includes hardware voltage values | TBD | Awaiting bench measurements |
| Validation log includes final pin verification | TBD | Awaiting continuity/probe results |
| Validation log includes SPI/GPIO probe evidence | TBD | Awaiting live probe |
| Validation log includes node-to-Gateway transfer capture | TBD | Awaiting Item 5 |
| Raw transfer capture file exists | TBD | Suggested: `logs/gateway_transfer_capture_YYYYMMDD.txt` |
| Parsed transfer output exists | TBD | Suggested: `logs/gateway_packets_YYYYMMDD.csv` |
| Known limitations explicitly stated | In progress | Validation log includes limitations |

### 7. Licensing checklist

| Check | Status | Evidence / notes |
|---|---|---|
| Hardware licence file exists | Verify | Check repository root licence files |
| Hardware licence is named clearly | TBD | Suggested README section: Hardware licence |
| Software licence file exists | Verify | Check repository root licence files |
| Software licence is OSI-approved | Verify | Required for software necessary to operate hardware |
| Documentation licence is stated | Verify | README should state doc licence |
| Third-party module licences are not misrepresented | TBD | Do not imply third-party modules are project-authored |
| Third-party software dependencies are identified | TBD | Python standard library only for current scripts |
| Licence headers are optional but consistent | TBD | Consider later if useful |
| README contains licence summary | TBD | Add before release if missing |

### 8. Branding and naming checklist

| Check | Status | Evidence / notes |
|---|---|---|
| Product name is consistent | In progress | `Thin-Pod Gateway rev 0.1` |
| Repository name is consistent | Done | `Thin-Pod-Gateway-rev-0.1` |
| Gateway is not confused with the Thin-Pod node certification | TBD | README and OSHWA submission should distinguish them |
| OSHWA mark is not used before certification | Confirm | Only use Gateway mark after certification is granted |
| Existing Thin-Pod node OSHWA UID is not reused for Gateway | Required | Gateway needs its own certification if submitted |
| Release title is clear | TBD | Suggested below |

### 9. Suggested release information

Suggested tag:

```text
v0.1.0
```

Suggested release title:

```text
Thin-Pod Gateway rev 0.1 - Hardware Bring-Up and Telemetry Proof
```

Suggested release summary:

```text
Thin-Pod Gateway rev 0.1 is the first fixed hardware and firmware evidence release for the Gateway PCB. It demonstrates stable hardware bring-up evidence, a verified NUCLEO-to-DWM3001-CDK pin map workflow, a minimal SPI/GPIO probe app scaffold, a defined telemetry packet, packet parsing/logging, a validation evidence ledger, and automated release-artifact checks.

This release does not claim a complete production DW3110 driver, full DSP/TinyML Gateway pipeline, field-qualified RF performance, or a completed node-to-Gateway validation transfer until those entries are explicitly marked complete in the validation log.
```

Final release wording should be adjusted after the live node-to-Gateway transfer is complete.

### 10. Suggested OSHWA submission fields

| Field | Draft value |
|---|---|
| Project name | Thin-Pod Gateway rev 0.1 |
| Project version | 0.1.0 |
| Country | United Kingdom |
| Project type | Electronics |
| Project website | GitHub repository / release URL |
| Documentation URL | GitHub repository / tagged release |
| Hardware licence | TBD |
| Software licence | TBD |
| Documentation licence | TBD |
| Creator / maintainer | TBD |
| Short description | Open hardware Gateway carrier/interface PCB for Thin-Pod telemetry experiments |
| Certification scope | Gateway PCB design files, documentation, validation evidence, and project-authored firmware/software |
| Exclusions | Internal designs of third-party modules including NUCLEO-N657X0-Q, DWM3001-CDK, and XIAO ESP32-C6 |

### 11. Pre-submission checks

Before submitting for OSHWA certification, complete the following:

| Check | Status |
|---|---|
| Run current artifact check | Done |
| Run GitHub Actions workflow | Done |
| Run release-mode artifact check | TBD |
| Fill final hardware bring-up measurements | TBD |
| Fill final pin-map verification results | TBD |
| Record live SPI/GPIO probe result | TBD |
| Record node-to-Gateway transfer capture | TBD |
| Parse live capture with `gateway_packet_logger.py` | TBD |
| Update validation log with final evidence | TBD |
| Confirm hardware source files are complete | TBD |
| Confirm fabrication outputs are complete | TBD |
| Confirm BOM is complete | TBD |
| Confirm licences are correct | TBD |
| Confirm README describes actual release tree | TBD |
| Confirm release tag exists | TBD |
| Confirm OSHWA mark is not used before approval | TBD |

## Completion checklist link

This document satisfies:

- Item 10: OSHWA certification preparation

It also supports:

- Item 7: one validation log
- Item 8: one GitHub Actions or scripted check
- Item 9: one tagged Gateway release

## Current conclusion

The Gateway rev 0.1 repository now has the structure needed for OSHWA certification preparation, but it is not yet ready for final submission.

The strongest next actions are:

1. complete final hardware measurements;
2. complete final pin-map verification;
3. run and record the SPI/GPIO probe result;
4. capture one real node-to-Gateway transfer;
5. update the validation log;
6. confirm hardware source and fabrication files;
7. verify licensing and README boundary wording;
8. tag the Gateway rev 0.1 release;
9. submit for OSHWA certification only after the release evidence is complete.

## Revision history

| Date | Change |
|---|---|
| 2026-06-27 | Initial OSHWA preparation checklist created |
