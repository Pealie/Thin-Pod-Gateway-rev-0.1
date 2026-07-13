# Editable hardware source

This directory contains the preferred modifiable source for Thin-Pod Gateway rev 0.1.

## Published KiCad project

The release source is stored at:

```text
hardware/source/kicad/rev0.1/
├── Thin-Pod Gateway.kicad_pro
├── Thin-Pod Gateway.kicad_sch
└── Thin-Pod Gateway.kicad_pcb
```

The schematic and PCB title blocks identify the design revision as `0.1`. The KiCad project and PCB fabrication-output paths use the repository-relative location:

```text
../../../fabrication/rev0.1/
```

This removes the private Windows/OneDrive path that was present in the working project before publication. No schematic connectivity, component placement, routing or board geometry was intentionally changed by that portability correction.

## Source status

The native KiCad source and corresponding Gerber/drill release package were committed on 13 July 2026. The earlier missing-source publication gate is therefore cleared.

The remaining release-verification work is:

1. open the project in a clean KiCad 10 environment;
2. confirm that no unavailable private symbol or footprint library is required;
3. run schematic ERC and PCB DRC;
4. inspect the generated Gerbers and drill data in a Gerber viewer;
5. record symbol and footprint provenance;
6. reconcile the fitted BOM and module-header/socket choices;
7. freeze the validated source and fabrication package under a public release tag.

The editable KiCad project is the normative design source. Gerbers, drill data and the fabrication ZIP are derived convenience outputs.