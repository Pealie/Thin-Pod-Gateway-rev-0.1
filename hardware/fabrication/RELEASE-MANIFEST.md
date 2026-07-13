# Thin-Pod Gateway rev 0.1f fabrication manifest

**Hardware family:** Thin-Pod Gateway rev 0.1  
**Fabrication-build identity:** rev 0.1f  
**Source archive:** `ThinPodGatewayFabricationPullupsCouplingCapsRev01f.zip`  
**Archive SHA-256:** `671fdf24f704971b09d9159b12aac790b0bf3fda13ec090275211d718cfb28aa`  
**Archive size:** 114,532 bytes  
**Generator recorded by package:** KiCad Pcbnew 10.0.1  
**Generation timestamp:** 2026-05-21T16:43:01+01:00

## File inventory

| File | Bytes | SHA-256 | Function |
|---|---:|---|---|
| `Thin-Pod Gateway-F_Cu.gbr` | 32,024 | `7ce11d80560ed5170f7e6e2e5933961218ee58cc14fcbf6244ca58d16e26bf15` | Top copper |
| `Thin-Pod Gateway-B_Cu.gbr` | 309,610 | `169ef69f7a9a26bd193159fec0bd02f635377917626288fb155cceb8e35f24cc` | Bottom copper |
| `Thin-Pod Gateway-F_Mask.gbr` | 8,793 | `8067e7a07bcc5134c42a4fee32745c98c3cc66a532cc0665a36dbff882e03fae` | Top solder mask |
| `Thin-Pod Gateway-B_Mask.gbr` | 7,498 | `7d3f692f3830f252001702ba75704a969f76ae7aad34c07fe3c6e3e37cd90f3b` | Bottom solder mask |
| `Thin-Pod Gateway-F_Paste.gbr` | 476 | `8ae967fa0bc1626a19b66ed42efe40a70b045e063f60fc4a37a5b105d7d2a9a4` | Top solder paste |
| `Thin-Pod Gateway-B_Paste.gbr` | 476 | `8724f6da1f1c4e4bf66bd1d2f891fabacb3bf0ee3e1a4fe6ee45d6869ff699eb` | Bottom solder paste |
| `Thin-Pod Gateway-F_Silkscreen.gbr` | 54,297 | `bb6631dce678719449306f11b1a36bdc75c15eda23bfe2f69d7c055c0742e351` | Top legend |
| `Thin-Pod Gateway-B_Silkscreen.gbr` | 477 | `cfe0680559bd70cf3d66a93a3ad95d600073a4e73e179a9d2d1de135464ff468` | Bottom legend |
| `Thin-Pod Gateway-Edge_Cuts.gbr` | 881 | `6b3e2bf569b48e6c0a17776a842d3372a3df39797f0f5e0b7cde346c686098c5` | Board profile |
| `Thin-Pod Gateway-PTH.drl` | 4,985 | `340d57f6955cf85f55c941bd0b7c30a05f77388c4d5cfb273ff5258fe96da593` | Plated drill data |
| `Thin-Pod Gateway-NPTH.drl` | 561 | `1c55192885fb70c8e26aadac8f2454e61a7a612aa35e20e9a06ff274711adf1e` | Non-plated drill data |
| `Thin-Pod Gateway-PTH-drl_map.gbr` | 134,924 | `0659a6b580be94d4dce8520a93dde7308db891df78fd205f7a655f2d9e685137` | PTH drill map |
| `Thin-Pod Gateway-NPTH-drl_map.gbr` | 41,674 | `db3467c5df6f8c99ccb8ddb704660a7577cbb475765414f9ba3fd82b1609afea` | NPTH drill map |
| `Thin-Pod Gateway-job.gbrjob` | 2,897 | `47c23ccc42f8ae054f2417b5349571d479fb8e5c96d90c9eb9e549d402e90683` | Gerber job metadata |

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

The package is internally coherent as a two-layer KiCad Gerber/drill set and records a 160.05 mm × 145.05 mm, 1.6 mm FR4 board. All 14 files are text-readable and individually hashed above.

The release remains incomplete for OSHWA submission because the corresponding preferred editable KiCad source is absent from the repository. Before release freeze:

1. commit the exact `.kicad_pro`, `.kicad_sch`, `.kicad_pcb`, project-local library tables and required local symbols/footprints;
2. set a definite source revision instead of `rev?`;
3. regenerate or byte-compare the fabrication package from that source;
4. run ERC, DRC and Gerber-viewer inspection and record the results;
5. freeze the reconciled source, BOM and fabrication package under one public release tag.
