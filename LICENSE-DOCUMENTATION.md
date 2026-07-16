# Thin-Pod Gateway rev 0.1: Documentation Licence

**Project:** Thin-Pod Gateway<br>
**Hardware release path:** rev 0.1<br>
**Documentation status:** OSHWA-certified rev 0.1 release documentation, UID `UK000092`<br>
**Copyright holder:** Copyright © 2026 Neil Thomson / Pealie<br>
**Documentation licence:** Creative Commons Attribution 4.0 International<br>
**SPDX licence identifier:** `CC-BY-4.0`<br>

## 1. Licence declaration

Unless marked otherwise, the creator-authored documentation in this repository is licensed under the **Creative Commons Attribution 4.0 International Public License (`CC-BY-4.0`)**.

Canonical licence information and legal terms are available at:

- Licence deed: <https://creativecommons.org/licenses/by/4.0/>
- Legal code: <https://creativecommons.org/licenses/by/4.0/legalcode.en>

The canonical Creative Commons legal code governs use of material covered by this documentation licence.

## 2. Covered documentation

This licence applies to Thin-Pod-created written, drawn and photographic documentation for **Thin-Pod Gateway rev 0.1**, unless a file carries a different or additional notice.

| Covered material | Description |
|---|---|
| `README.md` | Gateway release overview, scope and repository navigation |
| `docs/certification-scope.md` | Authoritative OSHWA boundary for the certified Gateway carrier PCB |
| `docs/gateway-bring-up-and-verification-protocol.md` | Physical and firmware-stage verification procedure |
| `docs/system-interface-control-document.md` | Interface definition linking Thin-Pod rev 0.1 and Gateway rev 0.1 without merging certification scopes |
| `docs/footprint-provenance.md` | CAD provenance and publication boundary |
| Creator-authored diagrams and tables | Explanatory content prepared for the Gateway project |
| Creator-owned photographs and test images | Board/assembly/measurement photographs made by the project author, unless otherwise marked |
| `oshwa/application-record.md` | Public non-sensitive application and issued-certification record |

## 3. Permission granted under CC BY 4.0

Subject to the Creative Commons Attribution 4.0 International licence terms, covered documentation may be:

- copied and redistributed in any medium or format;
- remixed, transformed and built upon;
- used for commercial or non-commercial purposes; and
- incorporated into downstream open-hardware documentation.

The required condition is attribution. Where covered material is redistributed or adapted, attribution must identify the creator, indicate that the material is licensed under CC BY 4.0, provide an appropriate licence link and indicate where changes have been made.

## 4. Recommended attribution

A suitable attribution for reused or adapted Gateway documentation is:

```text
Thin-Pod Gateway rev 0.1 documentation
Copyright © 2026 Neil Thomson / Pealie
Licensed under CC BY 4.0
Source: https://github.com/Pealie/Thin-Pod-Gateway-rev-0.1
```

Where the material has been changed, add an indication such as:

```text
Modified from the original Thin-Pod Gateway rev 0.1 documentation.
```

Attribution must not imply that Neil Thomson, Pealie or the Thin-Pod project endorses a modified work, product or use.

## 5. Relationship to hardware licensing

Documentation and hardware source are licensed separately.

| Material category | Licence treatment |
|---|---|
| Creator-designed Gateway hardware source, including editable schematic and PCB material | `CERN-OHL-W-2.0`, as recorded in `LICENSE-HARDWARE.md` |
| Creator-authored documentation, diagrams and creator-owned images | `CC-BY-4.0`, as recorded in this file |
| Project-authored firmware/software | `MIT`, as recorded in `LICENSE-SOFTWARE.md` |
| Third-party CAD/library dependencies | Remain under their recorded applicable terms |
| Purchased modules and vendor documentation | Not relicensed by Thin-Pod |

Thin-Pod rev 0.1 and Thin-Pod Gateway rev 0.1 are independently certified under UIDs `UK000091` and `UK000092`. Each documentation set preserves its own hardware and evidence boundary.

## 6. Excluded or separately licensed material

This documentation licence covers only material for which the copyright holder has authority to grant rights. It does not relicense, unless expressly stated and permitted:

| Excluded / separately treated material | Treatment |
|---|---|
| STMicroelectronics `NUCLEO-N657X0-Q` product imagery, schematics or documentation | Referenced where needed; remains under STMicroelectronics terms |
| Qorvo `DWM3001-CDK` product imagery, documentation, CAD or branding | Referenced where needed; remains under Qorvo terms |
| Seeed Studio `XIAO ESP32-C6` product imagery, documentation or branding | Referenced where needed; remains under Seeed Studio terms |
| Third-party CAD footprints, symbols or 3D models | Retain applicable source licences/notices |
| KiCad library materials | Remain under the KiCad library licence where copied or redistributed |
| SnapEDA / SnapMagic design files | Remain under the applicable SnapMagic/SnapEDA terms where included |
| OSHWA, CERN, Creative Commons or manufacturer marks and logos | Trade marks and brand rights remain with their respective holders |
| Third-party screenshots or reproduced extracts | Must be individually identified and used only under their applicable permission or limitation |

Where a repository document includes both Thin-Pod-created content and a separately licensed or referenced third-party element, the third-party element is excluded from the CC-BY-4.0 grant unless it is expressly identified as included under compatible terms.

## 7. Documentation status and evidential caution

The Gateway rev 0.1 documentation records design intent, module connections, test procedures, measured bring-up results, host-interface validation, one-way RF evidence and the issued OSHWA certification.

Claims remain limited to the evidence recorded in the repository. The documentation does not convert the carrier-board certification into a claim for complete vibration-window transport, field-qualified RF performance, DSP/TinyML performance, security assurance or regulatory approval.

This evidential boundary keeps measured results, certification scope and future system objectives distinct.

## 8. Open-hardware publication purpose

This documentation is published to support independent understanding, modification, manufacture and critical examination of the creator-designed Gateway carrier PCB.

The documentation supports a modular project structure in which:

```text
Thin-Pod rev 0.1 sensor-node PCB       OSHWA UID UK000091
Thin-Pod Gateway rev 0.1 carrier PCB   OSHWA UID UK000092
Thin-Pod Gateway rev 0.3               later SMT/chip-down direction, deferred
```

Documentation under this licence may be reused to build upon the Gateway concept, provided the CC BY 4.0 terms are followed and separately licensed materials remain properly distinguished.

## 9. No warranty or endorsement

The covered documentation is provided for experimental open-hardware development. It does not provide a warranty, product certification, regulatory approval or assurance of suitability for safety-critical use.

The Creative Commons licence does not grant permission to imply endorsement by the creator or by the organisations and manufacturers referenced in the project documentation.

## 10. Canonical licence reference

The governing licence for the covered documentation is:

**Creative Commons Attribution 4.0 International (`CC-BY-4.0`)**<br>
Licence deed: <https://creativecommons.org/licenses/by/4.0/><br>
Legal code: <https://creativecommons.org/licenses/by/4.0/legalcode.en>

This file identifies the covered documentation and its licensing boundary. The canonical CC BY 4.0 legal code governs use of the covered material.
