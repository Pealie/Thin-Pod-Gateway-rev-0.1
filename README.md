# Thin-Pod Gateway rev 0.1

**Open-hardware carrier PCB for Thin-Pod UWB telemetry, host supervision and onward-networking experiments**

## Status at 13 July 2026

The manufactured `rev 0.1f` carrier board has completed stable powered bring-up with the STM32 NUCLEO-N657X0-Q and Gateway-side DWM3001-CDK. The repository records:

- verified 5 V and 3.3 V distribution and common-ground continuity;
- corrected marginal DWM header solder joints affecting ground and RESET;
- valid RESET and READY rail levels;
- repeatable bidirectional NUCLEO-to-DWM `TPHIP GET_CAPABILITIES` exchange;
- physical DW3110 identity and initialisation on Gateway and Node DWM3001-CDKs;
- a matched 20-frame Gateway-to-Node UWB exchange;
- published KiCad project, schematic and PCB source for revision `0.1`;
- published Gerber, drill and fabrication ZIP outputs;
- clean schematic ERC and PCB DRC reports;
- recorded Gerber and drill inspection;
- recorded symbol and footprint provenance;
- a reconciled design-basis BOM with builder-selected compatible assembly hardware.

The remaining certification-release steps are repository checks, public visibility, immutable tagging and submission of a separate Gateway OSHWA application.

No Gateway OSHWA application has yet been submitted. The OSHWA mark must remain unused for this Gateway until certification is granted.

## Relationship to the certified Thin-Pod node

The companion [`Pealie/Thin-Pod-rev0.1`](https://github.com/Pealie/Thin-Pod-rev0.1) sensor-node carrier PCB is separately certified as OSHWA Open Source Hardware, UID [`UK000091`](https://certification.oshwa.org/uk000091.html).

That UID applies only to the Thin-Pod sensor-node carrier PCB. Thin-Pod Gateway rev 0.1 is a distinct hardware artefact requiring its own application and UID.

## Hardware scope

Thin-Pod Gateway rev 0.1 is a two-layer carrier PCB connecting:

| Reference | Module | Role |
|---|---|---|
| `U1` | STMicroelectronics `NUCLEO-N657X0-Q` | Main host and prospective analysis supervisor |
| `U2` | Qorvo `DWM3001-CDK` | Gateway-side UWB subsystem |
| `U3` | Seeed Studio `XIAO ESP32-C6` | Optional onward-networking subsystem |

The creator-controlled open-hardware contribution is the carrier PCB: power distribution, local decoupling, pull-ups, module interfaces, SPI/control routing, test access, mounting geometry, editable source and derived fabrication outputs.

The commercial modules remain third-party products whose internal designs are outside the Gateway open-hardware claim.

## Verified evidence

| Milestone | Result | Evidence |
|---|---|---|
| Stable carrier-board bring-up | PASS | [`docs/bringup/Gateway_rev0_1_Hardware_Bringup_Note.md`](docs/bringup/Gateway_rev0_1_Hardware_Bringup_Note.md) |
| NUCLEO-to-DWM physical host interface | PASS | [`docs/firmware/TPHIP_GET_CAPABILITIES_Physical_Validation.md`](docs/firmware/TPHIP_GET_CAPABILITIES_Physical_Validation.md) |
| DW3110 identity and initialisation | PASS | [`docs/validation/DW3110_Physical_Identity_Initialisation_Validation.md`](docs/validation/DW3110_Physical_Identity_Initialisation_Validation.md) |
| Matched 20-frame one-way RF exchange | PASS | [`docs/validation/DW3110_One_Way_RF_Exchange_Validation.md`](docs/validation/DW3110_One_Way_RF_Exchange_Validation.md) |
| Native KiCad source | PUBLISHED | [`hardware/source/README.md`](hardware/source/README.md) |
| Fabrication package and hashes | PUBLISHED | [`hardware/fabrication/RELEASE-MANIFEST.md`](hardware/fabrication/RELEASE-MANIFEST.md) |
| Schematic ERC | PASS | [`docs/validation/cad/rev0.1/Thin-Pod_Gateway_rev0.1_ERC.rpt`](docs/validation/cad/rev0.1/Thin-Pod_Gateway_rev0.1_ERC.rpt) |
| PCB DRC | PASS | [`docs/validation/cad/rev0.1/Thin-Pod_Gateway_rev0.1_DRC.rpt`](docs/validation/cad/rev0.1/Thin-Pod_Gateway_rev0.1_DRC.rpt) |
| Gerber and drill inspection | PASS | [`docs/validation/cad/rev0.1/Gerber_Drill_Inspection_Record.md`](docs/validation/cad/rev0.1/Gerber_Drill_Inspection_Record.md) |
| Symbol and footprint provenance | RECORDED | [`docs/footprint-provenance.md`](docs/footprint-provenance.md) |
| Assembly BOM | RECONCILED | [`hardware/bom/Thin-Pod_Gateway_rev0.1_BOM.md`](hardware/bom/Thin-Pod_Gateway_rev0.1_BOM.md) |

## Design and fabrication identity

The manufactured board may carry `rev 0.1f`. This is retained as the physical fabrication-build identity within the Thin-Pod Gateway rev 0.1 release path.

Published native source:

```text
hardware/source/kicad/rev0.1/
```

Published manufacturing outputs:

```text
hardware/fabrication/rev0.1/
```

The release Gerber job records project revision `0.1`. The regenerated outputs preserve the intended electrical connectivity, board outline, mounting geometry, module-interface positions, test-point arrangement and layer content of the manufactured rev 0.1f carrier.

## Assembly interface treatment

Builder-selected pin headers and/or socket strips compatible with the NUCLEO-N657X0-Q ST Morpho interfaces, DWM3001-CDK J10 interface, and optional XIAO ESP32-C6 carrier footprint are suitable for assembly. Header brand, plating, socket style and standoff selection are assembly choices outside the carrier-PCB certification scope.

## Licences

| Material | Licence |
|---|---|
| Creator-designed hardware source and derived fabrication outputs | `CERN-OHL-W-2.0` |
| Project-authored firmware, scripts and software examples | `MIT` |
| Creator-authored documentation and owned images | `CC-BY-4.0` |
| Third-party modules, software and CAD | Original applicable terms |

See [`LICENSE.md`](LICENSE.md), [`LICENSE-HARDWARE.md`](LICENSE-HARDWARE.md), [`LICENSE-SOFTWARE.md`](LICENSE-SOFTWARE.md), [`LICENSE-DOCUMENTATION.md`](LICENSE-DOCUMENTATION.md), [`THIRD_PARTY_NOTICES.md`](THIRD_PARTY_NOTICES.md) and [`docs/footprint-provenance.md`](docs/footprint-provenance.md).

## OSHWA submission gate

A Gateway application should be submitted after all of the following are complete:

1. run `scripts/check_oshwa_readiness.py` and the existing release-artifact checks successfully;
2. make the repository public;
3. create an immutable public release tag;
4. submit a separate Gateway OSHWA application using the licence selections above.

## Current limitations

The release does not claim Node-to-Gateway vibration-window transport, ranging, field-qualified RF performance, calibrated transmit power, antenna/coexistence performance, authenticated radio records, secure boot enforcement, production OTA, Gateway DSP/TinyML performance, EMC, radio, electrical-safety, CE or UKCA compliance.