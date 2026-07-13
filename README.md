# Thin-Pod Gateway rev 0.1

**Open-hardware carrier PCB for Thin-Pod UWB telemetry, host supervision and onward-networking experiments**

## Status at 13 July 2026

The manufactured `rev 0.1f` carrier board has completed stable PoE-powered bring-up with the STM32 NUCLEO-N657X0-Q and Gateway-side DWM3001-CDK. The repository records:

- verified 5 V and 3.3 V distribution and common-ground continuity;
- corrected marginal DWM header solder joints affecting ground and RESET;
- valid RESET and READY rail levels;
- a repeatable bidirectional NUCLEO-to-DWM `TPHIP GET_CAPABILITIES` exchange across the carrier PCB;
- physical DW3110 identity and initialisation on Gateway and Node DWM3001-CDKs;
- a matched 20-frame Gateway-to-Node one-way UWB exchange with retained raw RTT evidence and automated validation.

The carrier design is a credible OSHWA candidate, although the repository is **not yet ready for submission**. The decisive open gate is publication and reconciliation of the exact editable KiCad source used for the manufactured board. The attached Gerber/drill package has been audited and manifested, but derived fabrication outputs do not replace native modifiable design source.

No Gateway OSHWA application has been submitted. The OSHWA mark must not be used for this Gateway until certification is granted.

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
| Fabrication package inventory and hashes | AUDITED | [`hardware/fabrication/RELEASE-MANIFEST.md`](hardware/fabrication/RELEASE-MANIFEST.md) |
| Exact editable KiCad source | OPEN GATE | [`hardware/source/README.md`](hardware/source/README.md) |

## Fabrication-build identity

The manufactured board may carry `rev 0.1f`. This is treated as the fabrication-build identity within the Thin-Pod Gateway rev 0.1 release path.

The supplied package records:

```text
Generator:       KiCad Pcbnew 10.0.1
Generated:       21 May 2026
Board size:      160.05 mm × 145.05 mm
Copper layers:   2
Board thickness: 1.6 mm
PTH drill hits:  244
NPTH drill hits: 9
Archive SHA-256: 671fdf24f704971b09d9159b12aac790b0bf3fda13ec090275211d718cfb28aa
```

Its Gerber job metadata contains `rev?`. The final native design source must set a definite revision, then regenerate or reconcile the fabrication outputs before release freeze.

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
  bom/             Bill of materials
  fabrication/     Audited rev 0.1f fabrication metadata and manifest
  source/          Native KiCad source gate; exact source still required
firmware/           Zephyr applications, protocol code and hardware proofs
docs/
  bringup/          Physical bring-up records
  firmware/         Build, interface and RF procedures
  validation/       Measured validation records
  oshwa/            Certification preparation and application draft
logs/                Raw and interpreted engineering evidence
scripts/             Parsers and automated evidence checks
images/              Physical hardware and measurement evidence
```

## OSHWA submission gate

A Gateway application should be submitted only after all of the following are complete:

1. commit the exact editable KiCad project and all required project-local libraries;
2. resolve footprint and symbol provenance;
3. reconcile source, BOM and the manufactured `rev 0.1f` fabrication package;
4. commit the binary Gerber/drill archive and regenerated release outputs;
5. run and record ERC, DRC, Gerber-viewer and release-artifact checks;
6. update stale status statements in historical release-facing documents;
7. make the repository public;
8. create an immutable public release tag;
9. submit a separate Gateway OSHWA application using the licence selections above.

## Current limitations

The release does not claim Node-to-Gateway vibration-window transport, ranging, field-qualified RF performance, calibrated transmit power, antenna/coexistence performance, authenticated radio records, secure boot enforcement, production OTA, Gateway DSP/TinyML performance, EMC, radio, electrical-safety, CE or UKCA compliance.
