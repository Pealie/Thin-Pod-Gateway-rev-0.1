# Thin-Pod Gateway rev 0.1 licence map

Copyright © 2026 Neil Thomson / Pealie.

This repository contains hardware design material, software, documentation and references to third-party products and dependencies. Those categories are licensed separately.

| Material | Licence | Governing file |
|---|---|---|
| Thin-Pod-created Gateway schematic, PCB layout, project-local hardware libraries and derived fabrication outputs | CERN Open Hardware Licence Version 2, Weakly Reciprocal (`CERN-OHL-W-2.0`) | [`LICENSE-HARDWARE.md`](LICENSE-HARDWARE.md) |
| Thin-Pod-created firmware, Python utilities, validation scripts and software examples | MIT License (`MIT`) | [`LICENSE-SOFTWARE.md`](LICENSE-SOFTWARE.md) |
| Thin-Pod-created written documentation, diagrams and creator-owned photographs | Creative Commons Attribution 4.0 International (`CC-BY-4.0`) | [`LICENSE-DOCUMENTATION.md`](LICENSE-DOCUMENTATION.md) |
| Third-party software, CAD material, modules, vendor documentation, trade marks and logos | Original applicable terms | [`THIRD_PARTY_NOTICES.md`](THIRD_PARTY_NOTICES.md) and file-specific notices |

## Scope rules

1. A file-specific licence or copyright notice takes precedence over this summary.
2. The MIT grant applies only to software authored by the Thin-Pod project. It does not relicense the Qorvo DW3 SDK, Zephyr, nRF Connect SDK, STM32 software, vendor libraries or other external material.
3. The CERN-OHL-W-2.0 grant applies only to creator-controlled Gateway hardware source and its derived fabrication outputs. Commercial development modules and vendor-controlled CAD are outside that grant.
4. The CC-BY-4.0 grant applies only to documentation and images for which the copyright holder has authority to grant the licence.
5. Commercial components may be identified in the BOM for reproducibility without becoming open-hardware contributions of this project.

## OSHWA application declaration

The intended licence selections for a Thin-Pod Gateway rev 0.1 OSHWA application are:

```text
Hardware:       CERN-OHL-W-2.0
Software:       MIT
Documentation:  CC-BY-4.0
```

The Gateway is a separate certification candidate from the OSHWA-certified Thin-Pod rev 0.1 sensor-node carrier PCB, UID `UK000091`.
