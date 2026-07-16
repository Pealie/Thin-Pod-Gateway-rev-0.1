# Thin-Pod Gateway rev 0.1: Hardware Licence

**Project:** Thin-Pod Gateway<br>
**Hardware release path:** rev 0.1<br>
**Release status:** OSHWA-certified public rev 0.1 hardware baseline, UID `UK000092`<br>
**Copyright holder:** Copyright © 2026 Neil Thomson / Pealie<br>
**Hardware licence:** CERN Open Hardware Licence Version 2 — Weakly Reciprocal<br>
**SPDX licence identifier:** `CERN-OHL-W-2.0`<br>

## 1. Licence declaration

The creator-controlled hardware design material in this repository is made available under the **CERN Open Hardware Licence Version 2 — Weakly Reciprocal (`CERN-OHL-W-2.0`)**.

The canonical licence text is available from the Open Hardware Repository at:

<https://ohwr.org/cern_ohl_w_v2.txt>

Use, modification, manufacture and redistribution of covered hardware design material must comply with the terms of that licence.

## 2. Covered Thin-Pod Gateway material

Unless a file or folder states otherwise, this hardware licence applies to the Thin-Pod-created hardware design source for **Thin-Pod Gateway rev 0.1**, including:

| Covered material | Description |
|---|---|
| Gateway schematic source | Creator-designed circuit and interconnection definition for the rev 0.1 Gateway carrier PCB |
| Gateway PCB layout source | Creator-designed copper, solder-mask, silkscreen, board-outline, drill and mechanical-placement definition |
| Project-authored symbols and footprints | Custom hardware-source files created specifically for the Gateway carrier-board design |
| Fabrication outputs | Gerber and drill outputs generated from the covered creator-designed PCB source, to the extent rights are held by the licensor |
| Release manifests and hardware-source metadata | Technical material that identifies and supports reproduction of the hardware source, where it is part of the hardware design package |
| Modifications contributed by the licensor | Later creator-authored changes to the covered rev 0.1 hardware source, unless expressly released separately |

The released source locations are:

```text
hardware/
├── source/
│   └── kicad/
│       └── rev0.1/
│           ├── Thin-Pod Gateway.kicad_pro
│           ├── Thin-Pod Gateway.kicad_sch
│           └── Thin-Pod Gateway.kicad_pcb
│
└── fabrication/
    ├── RELEASE-MANIFEST.md
    └── rev0.1/
        ├── Thin-Pod Gateway-*.gbr
        ├── Thin-Pod Gateway-*.drl
        ├── Thin-Pod Gateway-job.gbrjob
        └── Thin-Pod_Gateway_rev0.1_fabrication.zip
```

## 3. Hardware certified-item boundary

This repository publishes the hardware certified by OSHWA under UID [`UK000092`](https://certification.oshwa.org/uk000092.html), issued 16 July 2026:

> **Thin-Pod Gateway rev 0.1: an open-hardware Gateway carrier PCB for receiving, supervising and forwarding Thin-Pod vibration-window data using commercially supplied development modules.**

The Gateway is a separate hardware product from **Thin-Pod rev 0.1**, the sensor-node carrier PCB certified under UID `UK000091`. UID `UK000092` does not expand or change the node certification boundary, and UID `UK000091` does not extend to the Gateway.

## 4. Commercial third-party hardware not covered by this licence

The Gateway carrier PCB is intended to interface with commercially supplied modules and components. These items may be specified in the BOM because they are required to reproduce or operate the assembly; their inclusion does **not** license them under CERN-OHL-W-2.0 or claim that they are Thin-Pod-created open hardware.

| Commercial item | Intended role in Gateway rev 0.1 | Licence treatment |
|---|---|---|
| STM32 `NUCLEO-N657X0-Q` | Main Gateway host and prospective analysis supervisor | Commercial STMicroelectronics development board; not covered by this Thin-Pod licence |
| Qorvo `DWM3001-CDK` | Gateway-side UWB development subsystem | Commercial Qorvo development board; not covered by this Thin-Pod licence |
| Seeed Studio `XIAO ESP32-C6` | Optional onward-networking subsystem | Commercial Seeed Studio development module; not covered by this Thin-Pod licence |
| Standard resistors, capacitors, headers, connectors and test hardware | Passive/interconnect implementation | Commercial components; listed for reproducibility, not relicensed |
| Vendor product documentation and datasheets | Identification, sourcing and interface reference | Remain subject to their respective owners’ terms |

The open-hardware contribution is the creator-designed **Gateway carrier board and its declared creator-controlled source material**, not the purchased modules fitted to it.

## 5. Third-party CAD, library and mechanical dependencies

Files not authored by Thin-Pod remain under their original applicable terms. In particular, this hardware licence does not override the licence of:

- CAD symbols, footprints or 3D models obtained from a manufacturer, component library or third-party service;
- official KiCad library content copied into or referenced by the project;
- SnapEDA / SnapMagic design files, where used;
- manufacturer-provided development-board geometry, models or documentation; or
- third-party logos, trade marks or branding.

The certified rev 0.1 release records each non-standard published footprint or symbol in `docs/footprint-provenance.md`, together with its source and licence treatment. Third-party CAD files retain their original terms; project-authored carrier-interface representations are identified separately.

## 6. Documentation and software are licensed separately

This hardware licence does not apply to all repository material indiscriminately.

| Material category | Intended treatment |
|---|---|
| Creator-authored documentation, diagrams and creator-owned images | Licensed under `CC-BY-4.0`, as recorded in `LICENSE-DOCUMENTATION.md` |
| Firmware or software | Project-authored software is separately licensed under `MIT`, as recorded in `LICENSE-SOFTWARE.md` |
| Third-party CAD/library dependencies | Retain their own recorded terms |
| Commercial modules and vendor documents | Not relicensed by this repository |

The OSHWA record for UID `UK000092` declares `MIT` for project-authored software. External SDKs, vendor firmware and other third-party software retain their own terms.

## 7. Modification and release discipline

The manufactured Gateway PCB may carry the fabrication-build identifier `rev 0.1f`. The certified repository record reconciles that physical marking with the public **Gateway rev 0.1** source and fabrication package.

Post-certification revision control requires:

1. preservation of public tag `v0.1` as the certified rev 0.1 hardware baseline;
2. explicit recording of documentation-only corrections;
3. a later hardware revision for electrical, mechanical, copper or interface design changes;
4. continued provenance records for non-standard CAD dependencies; and
5. continued separation between Gateway UID `UK000092` and node UID `UK000091`.

The planned SMT/chip-down direction remains reserved for a subsequent Gateway hardware revision, presently described as **rev 0.3**, and is not licensed here as if already implemented.

## 8. Source notice for covered hardware files

Creator-authored hardware source files may include a concise header or metadata notice in the following form where the file format supports it:

```text
Copyright © 2026 Neil Thomson / Pealie
SPDX-License-Identifier: CERN-OHL-W-2.0
Thin-Pod Gateway rev 0.1 hardware source.
Licensed under the CERN Open Hardware Licence Version 2 — Weakly Reciprocal.
```

Where a file is third-party material rather than Thin-Pod-created source, it must not receive this notice unless the applicable rights and attribution requirements permit that treatment.

## 9. No warranty or regulatory claim

Thin-Pod Gateway rev 0.1 is experimental open-hardware development material. Publication under CERN-OHL-W-2.0 does not represent a warranty of fitness, safety, regulatory compliance or verified system performance.

In particular, this repository does not claim:

- EMC, radio, electrical-safety, CE or UKCA compliance;
- safe deployment on industrial or safety-critical equipment;
- complete Node-to-Gateway vibration-window transport;
- validated Gateway DSP or TinyML operation; or
- open licensing of the commercial modules interfaced by the carrier board.

## 10. Canonical licence reference

The governing licence for the covered hardware material is:

**CERN Open Hardware Licence Version 2 — Weakly Reciprocal (`CERN-OHL-W-2.0`)**<br>
Canonical licence text: <https://ohwr.org/cern_ohl_w_v2.txt>

This file identifies the covered work and its licensing boundary. The canonical CERN-OHL-W-2.0 legal text governs use of the covered hardware material.
