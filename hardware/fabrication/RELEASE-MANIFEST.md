# Thin-Pod Gateway rev 0.1 fabrication manifest

**Hardware family:** Thin-Pod Gateway rev 0.1  
**Fabrication-build identity:** rev 0.1 / manufactured-board marking rev 0.1f where present  
**Release archive:** `hardware/fabrication/rev0.1/Thin-Pod_Gateway_rev0.1_fabrication.zip`  
**Archive SHA-256:** `3e51d68cb120c55aa06f386f4c433a48845a45d1cb986b2b08ac874ea5ec2d4d`  
**Archive size:** 114,303 bytes  
**Generator recorded by package:** KiCad Pcbnew 10.0.1  
**Generation timestamp:** 2026-07-13T12:29:39+01:00  
**Project revision recorded by Gerber job:** `0.1`

## File inventory

| File | Bytes | SHA-256 | Function |
|---|---:|---|---|
| `Thin-Pod Gateway-F_Cu.gbr` | 31,714 | `d53ecf1dcd7c5785a7a7fc6143ffa8b5bdc0befad1e07c39fe1dc3aee664d8ae` | Top copper |
| `Thin-Pod Gateway-B_Cu.gbr` | 318,882 | `a2cdf633cf8ac7402bfa648f41ed1f6e2865463d733e7ed4d25d74827f5d0cc4` | Bottom copper |
| `Thin-Pod Gateway-F_Mask.gbr` | 8,792 | `4006df558baa3d8b32d5b2e37b87dd932b5ddfca58e697babdd12722668dd043` | Top solder mask |
| `Thin-Pod Gateway-B_Mask.gbr` | 7,497 | `7e397fec2e413123703999820a5e582f58c4baf0796576630afe5329f3108224` | Bottom solder mask |
| `Thin-Pod Gateway-F_Paste.gbr` | 475 | `877ed2a93545d4eabdc8f0e34bf01fb03dcf937f09013d3f81f8fc6e5426fb52` | Top solder paste |
| `Thin-Pod Gateway-B_Paste.gbr` | 475 | `d02550a28dab085ef9a43730f1a463dc945aebc4ab5feac473a34ed8cbe1b76b` | Bottom solder paste |
| `Thin-Pod Gateway-F_Silkscreen.gbr` | 54,296 | `43ccb841c19d5442a63f25fede90486f6809dc8b6f8b45a0b0914ab151bc64a2` | Top legend |
| `Thin-Pod Gateway-B_Silkscreen.gbr` | 476 | `9df9ca770cfdaf16d5be41a48b017ca21640a0c1776d81031d473013db6641cb` | Bottom legend |
| `Thin-Pod Gateway-Edge_Cuts.gbr` | 880 | `70209bca86ac737d28adab44c9e2b96f6b5566bc91eb5fdad739d55ac1248ea6` | Board profile |
| `Thin-Pod Gateway-PTH.drl` | 4,985 | `8dfbf0d564c35edac91340e6aa42c1717f36a19cf655086a7c6fcc7eef35f243` | Plated drill data |
| `Thin-Pod Gateway-NPTH.drl` | 561 | `fe3a21e4f43f74fa8db504e04c258274c24c9a537797563c814edfbabe1a87b6` | Non-plated drill data |
| `Thin-Pod Gateway-PTH-drl_map.gbr` | 134,923 | `492f01252aa097a44e9ac1c7e79720999adaf1112be062e6ec2c6b3435db5c1b` | PTH drill map |
| `Thin-Pod Gateway-NPTH-drl_map.gbr` | 41,673 | `fda6dc4ef0539f90b781fd966e27fa389fdfca0385531bcc9b7161dfa6558271` | NPTH drill map |
| `Thin-Pod Gateway-job.gbrjob` | 2,896 | `795904a9500c31edf20cb09ecccc22a65c2a70b8135fdc3cffeed287f6c986bf` | Gerber job metadata |

## Native source inventory

| File | Bytes | SHA-256 |
|---|---:|---|
| `hardware/source/kicad/rev0.1/Thin-Pod Gateway.kicad_pro` | 15,837 | `d93b70403ad736d2e7d07ddffba312bf5544fadb74fdcd4fb3fcb8de90163d13` |
| `hardware/source/kicad/rev0.1/Thin-Pod Gateway.kicad_sch` | 186,604 | `6cfe02408b4071275b44bfcb8ee031cb30589e9aa85f7ce8fe3c587f5354a685` |
| `hardware/source/kicad/rev0.1/Thin-Pod Gateway.kicad_pcb` | 511,755 | `50ca43bfd5fa709f13785a92ca706bf4c4f41bb3bd32f28f012f2a7ffb4db47d` |

## Drill summary

### Plated holes

| Diameter | Hits |
|---:|---:|
| 0.300 mm | 24 |
| 0.700 mm | 10 |
| 0.800 mm | 14 |
| 0.889 mm | 14 |
| 1.000 mm | 151 |
| 1.016 mm | 28 |
| 3.000 mm | 3 |
| **Total** | **244** |

### Non-plated holes

| Diameter | Hits |
|---:|---:|
| 3.000 mm | 5 |
| 3.200 mm | 4 |
| **Total** | **9** |

## Audit result

The native KiCad source, individual Gerber and drill files, Gerber job and binary fabrication archive are now committed together. The job metadata records revision `0.1`, resolving the earlier `rev?` release defect.

Before the public release freeze:

1. open the project in a clean KiCad 10 environment;
2. complete and record schematic ERC and PCB DRC;
3. inspect the release package in a Gerber viewer;
4. document symbol and footprint provenance;
5. reconcile the BOM with the fitted assembly and exact module-header/socket choices;
6. confirm that the regenerated release package preserves the intended electrical and geometric design of the manufactured rev 0.1f board;
7. freeze the reconciled source, BOM and fabrication package under one public release tag.