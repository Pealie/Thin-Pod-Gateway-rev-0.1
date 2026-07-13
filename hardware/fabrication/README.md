# Thin-Pod Gateway rev 0.1 fabrication outputs

This directory contains the derived fabrication release for the Thin-Pod Gateway rev 0.1 carrier PCB.

## Published package

```text
hardware/fabrication/rev0.1/Thin-Pod_Gateway_rev0.1_fabrication.zip
SHA-256: 3e51d68cb120c55aa06f386f4c433a48845a45d1cb986b2b08ac874ea5ec2d4d
Size: 114,303 bytes
```

The archive contains 14 manufacturing files: top and bottom copper, solder mask, paste and silkscreen Gerbers; the board profile; PTH and NPTH drill files; PTH and NPTH drill maps; and a Gerber job file.

## Fabrication metadata

The release Gerber job records:

| Property | Value |
|---|---|
| Generator | KiCad Pcbnew 10.0.1 |
| Generation date | 13 July 2026, 12:29:39 +01:00 |
| Project revision | `0.1` |
| Board size | 160.05 mm × 145.05 mm |
| Copper layers | 2 |
| Board thickness | 1.6 mm |
| Dielectric | FR4, 1.51 mm recorded |
| Copper thickness | 0.035 mm per outer layer |
| Minimum track width/spacing | 0.2 mm |
| PTH drill hits | 244 |
| NPTH drill hits | 9 |

The release package was regenerated after the schematic and PCB title-block revision was set to `0.1` and the KiCad output path was made repository-relative. The release freeze should still record ERC, DRC and Gerber-viewer inspection, together with confirmation that these metadata and portability changes introduced no intentional electrical or geometric change from the manufactured rev 0.1f carrier.

## Source relationship

The corresponding native KiCad source is published under:

```text
hardware/source/kicad/rev0.1/
```

The editable KiCad project is the normative design source. These Gerber and drill files are derived manufacturing outputs supplied for convenience and reproducibility.

See [`RELEASE-MANIFEST.md`](RELEASE-MANIFEST.md) for hashes and [`../source/README.md`](../source/README.md) for source status and remaining validation work.