# Thin-Pod Gateway rev 0.1

**Open-hardware carrier PCB for Thin-Pod UWB telemetry, host supervision and onward-networking experiments**

## Status at 13 July 2026

The manufactured `rev 0.1f` carrier board has completed stable PoE-powered bring-up with the STM32 NUCLEO-N657X0-Q and Gateway-side DWM3001-CDK. The repository records:

- verified 5 V and 3.3 V distribution and common-ground continuity;
- corrected marginal DWM header solder joints affecting ground and RESET;
- valid RESET and READY rail levels;
- a repeatable bidirectional NUCLEO-to-DWM `TPHIP GET_CAPABILITIES` exchange across the carrier PCB;
- physical DW3110 identity and initialisation on Gateway and Node DWM3001-CDKs;
- a matched 20-frame Gateway-to-Node one-way UWB exchange with retained raw RTT evidence and automated validation;
- published native KiCad project, schematic and PCB source for revision `0.1`;
- published Gerber, drill and fabrication ZIP outputs generated from the revision-labelled source.

The carrier design is a credible OSHWA candidate. The editable-source and fabrication-publication gates are now cleared. Remaining release gates concern clean-environment KiCad validation, ERC and DRC evidence, Gerber inspection, CAD provenance, fitted-BOM reconciliation, public repository visibility and an immutable release tag.

No Gateway OSHWA application has been submitted. The OSHWA mark must remain unused for this Gateway until certification is granted.

## Relationship to the certified Thin-Pod node

The companion [`Pealie/Thin-Pod-rev0.1`](https://github.com/Pealie/Thin-Pod-rev0.1) sensor-node carrier PCB is separately certified as OSHWA Open Source Hardware, UID [`UK000091`](https://certification.oshwa.org/uk000091.html).

That UID applies only to the Thin-Pod sensor-node carrier PCB. Thin-Pod Gateway rev 0.1 is a distinct hardware artefact requiring its own application and UID.

## Hardware scope

Thin-Pod Gateway rev 0.1 is a two-layer carrier PCB that connects:

| Reference | Module | Role |
|---|---|---|
| `U1` | STMicroelectronics `NUCLEO-N657X0-Q` | Main host and prospective analysis supervisor |
| `U2` | Qorvo `DWM3001-CDK` | Gateway-side UWB subsystem |
| `U3` | Seeed Studio `XIAO ESP32-C6` | Optional onward-networking subsystem |

The creator-controlled open-hardware contribution is the carrier PCB: power distribution, local decoupling, pull-ups, module interfaces, SPI/control routing, test access, mounting geometry, editable source and derived fabrication outputs.

The commercial modules remain third-party products. Their internal designs are outside the Gateway open-hardware claim.

## Functional architecture

```text
Thin-Pod rev 0.1 sensor node
        ↓  UWB transport under staged validation
Gateway DWM3001-CDK
        ↓  verified TPHIP host interface across carrier PCB
STM32 NUCLEO-N657X0-Q
        ↓
buffering, logging, future DSP and TinyML evaluation
        ↓  optional onward interface
Seeed XIAO ESP32-C6
```

The current RF proof runs Gateway-to-Node. The intended sensor-to-Gateway Node-to-Gateway direction, vibration-window transport, ranging, production security and field RF qualification remain future milestones.

## Verified evidence

| Milestone | Result | Evidence |
|---|---|---|
| Stable carrier-board bring-up | PASS | [`docs/bringup/Gateway_rev0_1_Hardware_Bringup_Note.md`](docs/bringup/Gateway_rev0_1_Hardware_Bringup_Note.md) |
| NUCLEO-to-DWM physical host interface | PASS | [`docs/firmware/TPHIP_GET_CAPABILITIES_Physical_Validation.md`](docs/firmware/TPHIP_GET_CAPABILITIES_Physical_Validation.md) |
| DW3110 identity and initialisation | PASS | [`docs/validation/DW3110_Physical_Identity_Initialisation_Validation.md`](docs/validation/DW3110_Physical_Identity_Initialisation_Validation.md) |
| Matched 20-frame one-way RF exchange | PASS | [`docs/validation/DW3110_One_Way_RF_Exchange_Validation.md`](docs/validation/DW3110_One_Way_RF_Exchange_Validation.md) |
| Native KiCad source | PUBLISHED | [`hardware/source/README.md`](hardware/source/README.md) |
| Fabrication package inventory and hashes | PUBLISHED | [`hardware/fabrication/RELEASE-MANIFEST.md`](hardware/fabrication/RELEASE-MANIFEST.md) |
| ERC, DRC and clean-environment opening | OPEN GATE | Release verification record required |
| CAD symbol and footprint provenance | OPEN GATE | Provenance record required |
| Fitted BOM and module-mounting reconciliation | OPEN GATE | [`hardware/bom/Thin-Pod_Gateway_rev0.1_BOM.md`](hardware/bom/Thin-Pod_Gateway_rev0.1_BOM.md) |

## Design and fabrication identity

The manufactured board may carry `rev 0.1f`. This is retained as the physical fabrication-build identity within the Thin-Pod Gateway rev 0.1 release path.

The published native source is stored at:

```text
hardware/source/kicad/rev0.1/
```

The corresponding published manufacturing outputs are stored at:

```text
hardware/fabrication/rev0.1/
```

The release Gerber job records:

```text
Generator:       KiCad Pcbnew 10.0.1
Generated:       13 July 2026, 12:29:39 +01:00
Project revision: 0.1
Board size:      160.05 mm × 145.05 mm
Copper layers:   2
Board thickness: 1.6 mm
PTH drill hits:  244
NPTH drill hits: 9
Archive SHA-256: 3e51d68cb120c55aa06f386f4c433a48845a45d1cb986b2b08ac874ea5ec2d4d
```

The earlier `rev?` Gerber-job value is resolved. The KiCad project uses a repository-relative fabrication output path, replacing the earlier private Windows/OneDrive location.

## Electrical interface summary

### NUCLEO to DWM3001-CDK

| Signal | NUCLEO | DWM3001-CDK | Directional description |
|---|---|---|---|
| `SPI5_SCK` | `CN15.11 / PE15` | `J10.23 / SPI1_CLK` | Controller clock |
| `SPI5_MOSI` / COPI | `CN15.15 / PG2` | `J10.19 / SPI1_MOSI` | Controller to peripheral |
| `SPI5_MISO` / CIPO | `CN15.13 / PG1` | `J10.21 / SPI1_MISO` | Peripheral to controller |
| `DWM_CS` | `CN15.17 / PA3` | `J10.24 / CS_RPI` | Active-low select |
| `DWM_IRQ` / READY | `CN15.16 / PB9` | `J10.15 / GPIO_RPI` | Peripheral status to host |
| `DWM_RESET` | `CN15.33 / PD0` | `J10.12 / RESET` | Host reset control |

### Power

| Rail | Source | Use |
|---|---|---|
| `5V_GATEWAY` | NUCLEO `CN3.6` | DWM `J10.2 / VRPI` and optional XIAO `5V/VBUS` |
| `3V3_GATEWAY` | NUCLEO `CN3.16` | Pull-ups, auxiliary logic and test access |
| `GND` | Common carrier reference | NUCLEO, DWM, XIAO and passive returns |

## Licences

| Material | Licence |
|---|---|
| Creator-designed hardware source and derived fabrication outputs | `CERN-OHL-W-2.0` |
| Project-authored firmware, scripts and software examples | `MIT` |
| Creator-authored documentation and owned images | `CC-BY-4.0` |
| Third-party modules, software and CAD | Original applicable terms |

See [`LICENSE.md`](LICENSE.md), [`LICENSE-HARDWARE.md`](LICENSE-HARDWARE.md), [`LICENSE-SOFTWARE.md`](LICENSE-SOFTWARE.md), [`LICENSE-DOCUMENTATION.md`](LICENSE-DOCUMENTATION.md) and [`THIRD_PARTY_NOTICES.md`](THIRD_PARTY_NOTICES.md).

## Repository map

```text
hardware/
  bom/                         Bill of materials and release reconciliation
  fabrication/
    rev0.1/                    Published Gerber, drill and fabrication ZIP files
  source/
    kicad/rev0.1/              Published native KiCad project, schematic and PCB
firmware/                      Zephyr applications, protocol code and hardware proofs
docs/
  bringup/                     Physical bring-up records
  firmware/                    Build, interface and RF procedures
  validation/                  Measured validation records
  oshwa/                       Certification preparation and application draft
logs/                          Raw and interpreted engineering evidence
scripts/                       Parsers and automated evidence checks
images/                        Physical hardware and measurement evidence
```

## OSHWA submission gate

A Gateway application should be submitted after all of the following are complete:

1. open the published project in a clean KiCad 10 environment;
2. record schematic ERC and PCB DRC results;
3. inspect and record the Gerber and drill release in a Gerber viewer;
4. resolve symbol and footprint provenance;
5. reconcile the BOM with the fitted assembly and exact header/socket choices;
6. confirm the regenerated release outputs preserve the intended electrical and geometric design of the manufactured rev 0.1f carrier;
7. clear `scripts/check_oshwa_readiness.py` and the existing release-artifact checks;
8. make the repository public;
9. create an immutable public release tag;
10. submit a separate Gateway OSHWA application using the licence selections above.

## Current limitations

The release does not claim Node-to-Gateway vibration-window transport, ranging, field-qualified RF performance, calibrated transmit power, antenna/coexistence performance, authenticated radio records, secure boot enforcement, production OTA, Gateway DSP/TinyML performance, EMC, radio, electrical-safety, CE or UKCA compliance.
