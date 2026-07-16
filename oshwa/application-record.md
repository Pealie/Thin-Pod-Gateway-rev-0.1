# Thin-Pod Gateway rev 0.1: OSHWA Application and Certification Record

**Project:** Thin-Pod Gateway<br>
**Hardware release:** rev 0.1<br>
**Responsible individual:** Neil Thomson / Pealie<br>
**Repository:** [Pealie/Thin-Pod-Gateway-rev-0.1](https://github.com/Pealie/Thin-Pod-Gateway-rev-0.1)<br>
**Application status:** Certified by the Open Source Hardware Association<br>
**Certification date:** 16 July 2026<br>
**OSHWA UID:** [`UK000092`](https://certification.oshwa.org/uk000092.html)<br>
**Document status:** Public, non-sensitive application and certification record<br>

## 1. Purpose

This document records the public substance of the OSHWA certification application and issued certification for **Thin-Pod Gateway rev 0.1**.

Private postal-address, correspondence-email and application-account details supplied directly to OSHWA are excluded. The public record covers the certified item, declared scope, licences, documentation location and supporting release evidence.

## 2. Certified item

The certified item is:

> **Thin-Pod Gateway rev 0.1: an open-hardware carrier PCB for experimental Thin-Pod UWB telemetry and Gateway-side host-interface work.**

The carrier board provides power distribution, local decoupling, pull-ups, test access and SPI/control routing between a commercial STM32 NUCLEO-N657X0-Q host, Qorvo DWM3001-CDK UWB subsystem and optional Seeed Studio XIAO ESP32-C6 onward-networking module.

## 3. Certification boundary

### Included

| Included element | Description |
|---|---|
| Gateway rev 0.1 PCB | Creator-designed two-layer carrier board |
| Commercial-module interfaces | NUCLEO-N657X0-Q, DWM3001-CDK and optional XIAO ESP32-C6 mating and electrical interfaces |
| Power and passive circuitry | 5 V and 3.3 V distribution, common-ground routing, decoupling and pull-ups |
| Host-interface routing | Shared SPI5 SCK/COPI/CIPO and separate chip-select, interrupt and reset lines |
| Test and mechanical features | TP1–TP13 and board/module mounting geometry |
| Editable source | Native KiCad project, schematic and PCB files |
| Fabrication package | Gerber, drill, job and hashed fabrication archive |
| Reproduction records | BOM, assembly, bring-up, validation and provenance documentation |
| Project-authored software | MIT-licensed firmware, scripts, protocol code and validation utilities published in the repository |

### Excluded

| Excluded element | Reason |
|---|---|
| STMicroelectronics NUCLEO-N657X0-Q internal design | Commercial third-party development board |
| Qorvo DWM3001-CDK and DW3110 internal design | Commercial third-party development board and device |
| Seeed Studio XIAO ESP32-C6 internal design | Commercial optional module |
| External Qorvo DW3 SDK source | Separately licensed vendor dependency |
| Thin-Pod rev 0.1 sensor-node carrier PCB | Separately certified product, UID `UK000091` |
| Complete vibration-window transport and ranging | Wider system implementation beyond the carrier-board claim |
| DSP, TinyML and predictive-maintenance performance | Later analysis and validation work |
| Security assurance and production OTA | Separate implementation and assurance work |
| EMC, radio, electrical safety, CE and UKCA approval | Separate regulatory assessment |

## 4. Public project information

| Application field | Public record |
|---|---|
| Project name | Thin-Pod Gateway |
| Project version | 0.1 |
| Certifying party | Individual |
| Responsible individual | Neil Thomson |
| Country | United Kingdom |
| Project website | [GitHub repository](https://github.com/Pealie/Thin-Pod-Gateway-rev-0.1) |
| Documentation location | [GitHub repository](https://github.com/Pealie/Thin-Pod-Gateway-rev-0.1) |
| Primary project type | Electronics |
| Related certified project | Thin-Pod rev 0.1, UID [`UK000091`](https://certification.oshwa.org/uk000091.html) |
| Incorporation relationship | Separate product and certification boundary |

## 5. Project description

Thin-Pod Gateway rev 0.1 is an open-hardware carrier PCB for experimental Thin-Pod UWB telemetry and Gateway-side host-interface work. It provides power distribution, local decoupling, pull-ups, test access and SPI/control routing between a commercial STM32 NUCLEO-N657X0-Q host, Qorvo DWM3001-CDK UWB subsystem and optional Seeed XIAO ESP32-C6 onward-networking module. The open contribution comprises the creator-designed carrier PCB, editable design source, fabrication outputs, BOM, documentation and project-authored software. The commercial modules remain outside the open-hardware claim.

## 6. Project keywords

The public OSHWA directory records:

```text
open-source hardware, UWB, telemetry, gateway, carrier PCB,
IoT, predictive maintenance
```

## 7. Licensing declarations

| Material category in OSHWA record | Declared licence |
|---|---|
| Hardware | CERN Open Hardware Licence Version 2, Weakly Reciprocal (`CERN-OHL-W-2.0`) |
| Software | MIT License (`MIT`) |
| Documentation | Creative Commons Attribution 4.0 International (`CC-BY-4.0`) |

Repository material remains subject to the detailed boundaries in [`../LICENSE.md`](../LICENSE.md), [`../LICENSE-HARDWARE.md`](../LICENSE-HARDWARE.md), [`../LICENSE-SOFTWARE.md`](../LICENSE-SOFTWARE.md), [`../LICENSE-DOCUMENTATION.md`](../LICENSE-DOCUMENTATION.md) and [`../THIRD_PARTY_NOTICES.md`](../THIRD_PARTY_NOTICES.md).

## 8. Third-party module and dependency declaration

Commercial modules are listed in the BOM to make the assembly reproducible. Their inclusion does not place their internal designs, vendor firmware, documentation or branding under the Thin-Pod licences.

The external Qorvo DW3 SDK is not distributed by this repository. Thin-Pod-authored adapter and integration files remain distinguishable from vendor source.

CAD provenance is recorded in [`../docs/footprint-provenance.md`](../docs/footprint-provenance.md).

## 9. Release evidence

| Evidence item | Recorded result |
|---|---|
| Manufactured and assembled carrier PCB | Complete |
| Stable powered NUCLEO and DWM bring-up | PASS |
| Power, ground, RESET and READY measurements | PASS |
| Bidirectional NUCLEO-to-DWM TPHIP exchange | PASS |
| Physical DW3110 identity and initialisation | PASS |
| Matched 20-frame Gateway-to-Node UWB exchange | PASS |
| Native KiCad source | Published |
| Fabrication package | Published, revision-labelled and SHA-256 recorded |
| Schematic ERC and PCB DRC | PASS, zero reported violations |
| Gerber and drill inspection | PASS |
| BOM reconciliation | Complete for the release basis |
| Symbol and footprint provenance | Recorded |
| Repository readiness checks | PASS |
| Canonical public release tag | `v0.1` |

## 10. Certification status record

| Milestone | Status | Date / identifier |
|---|---|---|
| Gateway hardware scope established | Completed | rev 0.1 |
| Physical bring-up and host-interface evidence recorded | Completed | July 2026 |
| Native source, BOM and fabrication package published | Completed | July 2026 |
| ERC, DRC, Gerber inspection and provenance checks completed | Completed | July 2026 |
| Public immutable release baseline created | Completed | `v0.1` |
| OSHWA application submitted | Completed | Pre-certification application record |
| OSHWA certification issued | **Issued** | 16 July 2026 / **UK000092** |
| UID and certification mark added to repository | Completed | 16 July 2026 |

## 11. Relationship to the Thin-Pod node

The companion sensor-node carrier PCB remains a separate certification:

| Product | UID | Date |
|---|---|---|
| Thin-Pod rev 0.1 | [`UK000091`](https://certification.oshwa.org/uk000091.html) | 28 May 2026 |
| Thin-Pod Gateway rev 0.1 | [`UK000092`](https://certification.oshwa.org/uk000092.html) | 16 July 2026 |

Neither certification extends the hardware boundary of the other.

## 12. Post-certification maintenance

1. Preserve tag `v0.1` as the canonical certified Gateway rev 0.1 hardware baseline.
2. Record documentation-only corrections without changing the released hardware identity.
3. Assign electrical, mechanical, copper or interface changes to a later Gateway revision.
4. Preserve the declared third-party module and CAD boundaries.
5. Use the OSHWA certification mark only with UID `UK000092` and under OSHWA's mark-usage terms.
6. Keep dated pre-certification plans and engineering logs available as historical records.

## 13. Public-record privacy note

This record excludes private postal-address, application-account and correspondence details. The official OSHWA directory remains the authoritative public record of the issued UID and certification date.

## 14. References

- OSHWA certification record: <https://certification.oshwa.org/uk000092.html>
- OSHWA certification requirements: <https://certification.oshwa.org/requirements.html>
- OSHWA certification-mark usage: <https://certification.oshwa.org/mark-usage.html>
- OSHWA certification-mark licence agreement: <https://certification.oshwa.org/license-agreement.html>
- Gateway release repository: <https://github.com/Pealie/Thin-Pod-Gateway-rev-0.1>
- Companion Thin-Pod node record: <https://certification.oshwa.org/uk000091.html>

---

**Certification record:** Thin-Pod Gateway rev 0.1 is OSHWA-certified under UID `UK000092`, issued 16 July 2026.
