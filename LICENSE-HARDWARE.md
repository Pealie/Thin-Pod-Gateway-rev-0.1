# Thin-Pod Gateway rev 0.1: Hardware Licence

**Project:** Thin-Pod Gateway  
**Hardware release path:** rev 0.1  
**Release status:** Open-hardware candidate; PCB ordered and bring-up pending  
**Copyright holder:** Copyright © 2026 Neil Thomson / Pealie  
**Hardware licence:** CERN Open Hardware Licence Version 2 — Weakly Reciprocal  
**SPDX licence identifier:** `CERN-OHL-W-2.0`  

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

The intended source locations are:

```text
hardware/
├── source/
│   ├── Thin-Pod_Gateway_rev0.1.kicad_pro
│   ├── Thin-Pod_Gateway_rev0.1.kicad_sch
│   ├── Thin-Pod_Gateway_rev0.1.kicad_pcb
│   ├── fp-lib-table
│   ├── sym-lib-table
│   └── footprints/
│
└── fabrication/
    ├── gerbers/
    ├── drills/
    ├── Thin-Pod_Gateway_rev0.1_fabrication_outputs.zip
    └── RELEASE-MANIFEST.md
```

## 3. Hardware certified-item boundary

This repository prepares a later, separate OSHWA certification candidate for:

> **Thin-Pod Gateway rev 0.1: an open-hardware Gateway carrier PCB for receiving, supervising and forwarding Thin-Pod vibration-window data using commercially supplied development modules.**

The Gateway is a separate hardware product from **Thin-Pod rev 0.1**, the sensor-node carrier PCB already submitted for OSHWA certification. Nothing in this repository expands or changes the submitted certification boundary of the Thin-Pod node.

No Gateway OSHWA certification application is claimed as submitted unless and until the repository record is updated after physical bring-up, release freeze and a separate application.

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

Before a Gateway OSHWA submission, any non-standard published footprint or symbol required by the release must be listed in `docs/footprint-provenance.md`, with its source and licence treatment recorded. Third-party CAD files should either be published under their own compatible terms or replaced with independently authored carrier-interface representations.

## 6. Documentation and software are licensed separately

This hardware licence does not apply to all repository material indiscriminately.

| Material category | Intended treatment |
|---|---|
| Creator-authored documentation, diagrams and creator-owned images | Licensed under `CC-BY-4.0`, as recorded in `LICENSE-DOCUMENTATION.md` |
| Firmware or software | No firmware/software licence is assigned by this file; any later released software must carry its own compatible licence statement |
| Third-party CAD/library dependencies | Retain their own recorded terms |
| Commercial modules and vendor documents | Not relicensed by this repository |

For a later Gateway OSHWA application, the software licence choice must reflect the actual release: `No software` is appropriate only where the certified Gateway release does not supply or require creator-provided firmware as part of its certified claim.

## 7. Modification and release discipline

The ordered Gateway PCB may carry a fabrication-build identifier such as `rev 0.1f`. It is treated as a pre-release build within the **Gateway rev 0.1** development path.

Before any OSHWA submission for this Gateway:

1. the received PCB must be physically inspected and brought up;
2. the final editable source, BOM and Gerbers must be reconciled with the intended release;
3. all published non-standard CAD dependencies must have a documented licence/provenance position;
4. required pre-release electrical or layout corrections must be incorporated into the first clean Gateway rev 0.1 release source; and
5. the release state should be frozen using a tag such as `v0.1-oshwa-submission`.

The later planned SMT/chip-down direction is reserved for a subsequent Gateway hardware revision, presently described as **rev 0.3**, and is not licensed here as if already implemented.

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
- completed UWB node-to-Gateway transport;
- validated Gateway DSP or TinyML operation; or
- open licensing of the commercial modules interfaced by the carrier board.

## 10. Canonical licence reference

The governing licence for the covered hardware material is:

**CERN Open Hardware Licence Version 2 — Weakly Reciprocal (`CERN-OHL-W-2.0`)**  
Canonical licence text: <https://ohwr.org/cern_ohl_w_v2.txt>

This file identifies the covered work and its licensing boundary. The canonical CERN-OHL-W-2.0 legal text governs use of the covered hardware material.
