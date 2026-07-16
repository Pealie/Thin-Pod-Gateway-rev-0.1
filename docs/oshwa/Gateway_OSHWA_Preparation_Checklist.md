# Thin-Pod Gateway OSHWA Preparation and Certification Checklist

**Hardware release:** rev 0.1<br>
**Final status:** Completed and certified<br>
**OSHWA UID:** [`UK000092`](https://certification.oshwa.org/uk000092.html)<br>
**Certification date:** 16 July 2026<br>
**Canonical release tag:** `v0.1`<br>

## Purpose

This document preserves the completion state of the Gateway rev 0.1 OSHWA preparation work and links each principal certification gate to the public repository evidence.

It supersedes the earlier working checklist status while retaining the same path for traceability.

## Certification result

| Area | Final status | Evidence |
|---|---|---|
| Certification scope | COMPLETE | [`../certification-scope.md`](../certification-scope.md) |
| Public application record | COMPLETE | [`../../oshwa/application-record.md`](../../oshwa/application-record.md) |
| Hardware licence | COMPLETE | [`../../LICENSE-HARDWARE.md`](../../LICENSE-HARDWARE.md) |
| Software licence | COMPLETE | [`../../LICENSE-SOFTWARE.md`](../../LICENSE-SOFTWARE.md) |
| Documentation licence | COMPLETE | [`../../LICENSE-DOCUMENTATION.md`](../../LICENSE-DOCUMENTATION.md) |
| Third-party boundary | COMPLETE | [`../../THIRD_PARTY_NOTICES.md`](../../THIRD_PARTY_NOTICES.md) |
| Native hardware source | PUBLISHED | [`../../hardware/source/`](../../hardware/source/) |
| Fabrication outputs | PUBLISHED | [`../../hardware/fabrication/`](../../hardware/fabrication/) |
| BOM | RECONCILED | [`../../hardware/bom/`](../../hardware/bom/) |
| Validation evidence | COMPLETE FOR RELEASE CLAIM | [`../validation/`](../validation/) |
| Public repository | COMPLETE | `Pealie/Thin-Pod-Gateway-rev-0.1` |
| Immutable release baseline | COMPLETE | `v0.1` |
| OSHWA certification | ISSUED | `UK000092`, 16 July 2026 |
| Certification mark | ADDED | [`../../images/certification/oshwa-uk000092.png`](../../images/certification/oshwa-uk000092.png) |

## Scope checks

| Check | Result |
|---|---|
| Gateway carrier PCB identified as the creator-controlled hardware item | PASS |
| NUCLEO-N657X0-Q, DWM3001-CDK and XIAO ESP32-C6 identified as commercial modules | PASS |
| Commercial-module internal designs excluded from the open-hardware claim | PASS |
| Thin-Pod node and Gateway certification boundaries kept separate | PASS |
| Project-authored firmware and software declared under MIT | PASS |
| Hardware, software and documentation licence selections aligned with the OSHWA record | PASS |
| Regulatory and industrial-performance claims excluded | PASS |

## Source and fabrication checks

| Check | Result |
|---|---|
| KiCad project, schematic and PCB source present | PASS |
| Gerber, drill and Gerber-job files present | PASS |
| Fabrication archive present and SHA-256 recorded | PASS |
| Gerber job records revision `0.1` | PASS |
| Schematic ERC report records zero messages, errors and warnings | PASS |
| PCB DRC report records zero violations, unconnected pads and footprint errors | PASS |
| Gerber and drill inspection record passes | PASS |
| BOM records the release basis and commercial modules | PASS |
| Symbol and footprint provenance documented | PASS |
| Manufactured `rev 0.1f` marking reconciled with public release revision `0.1` | PASS |

## Physical and functional evidence checks

| Check | Result |
|---|---|
| Stable carrier-board power-up | PASS |
| 5 V, 3.3 V and common-ground continuity | PASS |
| RESET and READY behaviour | PASS |
| Bidirectional NUCLEO-to-DWM `TPHIP GET_CAPABILITIES` exchange | PASS |
| Physical DW3110 identity and initialisation | PASS |
| Matched 20-frame Gateway-to-Node UWB exchange | PASS |
| Raw logs and validation records retained | PASS |

## Repository and automation checks

| Check | Result |
|---|---|
| Release-artifact checker present | PASS |
| OSHWA certification-record checker present | PASS |
| Python scripts compile | PASS |
| GitHub Actions workflow present | PASS |
| README contains UID, certification date and directory link | PASS |
| Application record contains UID, certification date and directory link | PASS |
| UID-bearing certification mark stored in the repository | PASS |

## Certification metadata

| Field | Final value |
|---|---|
| Project name | Thin-Pod Gateway |
| Version | 0.1 |
| Country | United Kingdom |
| Project type | Electronics |
| Hardware licence | `CERN-OHL-W-2.0` |
| Software licence | `MIT` |
| Documentation licence | `CC-BY-4.0` |
| OSHWA UID | `UK000092` |
| Certification date | 16 July 2026 |
| Official record | <https://certification.oshwa.org/uk000092.html> |

## Residual limits

Certification completion does not add claims for:

- complete Node-to-Gateway vibration-window transport;
- ranging or field-qualified RF performance;
- calibrated transmit power or antenna/coexistence qualification;
- secure boot enforcement or production OTA;
- Gateway DSP/TinyML performance;
- authenticated maintenance records;
- EMC, radio, electrical-safety, CE or UKCA compliance.

## Post-certification controls

1. Preserve `v0.1` as the certified hardware baseline.
2. Treat later electrical or mechanical changes as a new hardware revision.
3. Keep UID `UK000092` attached only to the Gateway rev 0.1 certification boundary.
4. Preserve UID `UK000091` for the separate Thin-Pod node.
5. Retain the historical application draft under `docs/archive/oshwa/`.
6. Run the repository checks after any certification-document update.

## Revision history

| Date | Change |
|---|---|
| 27 June 2026 | Initial OSHWA preparation checklist created |
| 16 July 2026 | Reconciled checklist with issued certification UID `UK000092` and final public evidence |
