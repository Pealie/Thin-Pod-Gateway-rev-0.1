# Thin-Pod Gateway rev 0.1

<p align="left">
  <a href="https://certification.oshwa.org/uk000092.html">
    <img src="images/certification/oshwa-uk000092.png" width="260" alt="Open Source Hardware Association certification mark: UK000092">
  </a>
</p>
<p align="left">
  <strong>OSHWA Certified Open Source Hardware · UK000092</strong><br>
  Certified 16 July 2026 · <a href="https://certification.oshwa.org/uk000092.html">View certification record</a>
</p>

**Open-hardware carrier PCB for Thin-Pod UWB telemetry, host supervision and onward-networking experiments**

## Status at 16 July 2026

Thin-Pod Gateway rev 0.1 is certified by the Open Source Hardware Association under UID [`UK000092`](https://certification.oshwa.org/uk000092.html). The certified public baseline is the `v0.1` release, with post-certification status and directory references maintained on `main`.

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

## Relationship to the certified Thin-Pod node

The companion [`Pealie/Thin-Pod-rev0.1`](https://github.com/Pealie/Thin-Pod-rev0.1) sensor-node carrier PCB is separately certified as OSHWA Open Source Hardware, UID [`UK000091`](https://certification.oshwa.org/uk000091.html), certified 28 May 2026.

The two certifications describe distinct hardware artefacts:

| Hardware artefact | OSHWA UID | Certification date |
|---|---|---|
| Thin-Pod rev 0.1 sensor-node carrier PCB | [`UK000091`](https://certification.oshwa.org/uk000091.html) | 28 May 2026 |
| Thin-Pod Gateway rev 0.1 carrier PCB | [`UK000092`](https://certification.oshwa.org/uk000092.html) | 16 July 2026 |

The node UID applies only to the sensor-node carrier PCB. The Gateway UID applies only to the Gateway carrier PCB and its declared certification package.

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

The manufactured board may carry `rev 0.1f`. This is retained as the physical fabrication-build identity within the certified Thin-Pod Gateway rev 0.1 release record.

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

These are the licence selections recorded for OSHWA certification UID `UK000092`.

See [`LICENSE.md`](LICENSE.md), [`LICENSE-HARDWARE.md`](LICENSE-HARDWARE.md), [`LICENSE-SOFTWARE.md`](LICENSE-SOFTWARE.md), [`LICENSE-DOCUMENTATION.md`](LICENSE-DOCUMENTATION.md), [`THIRD_PARTY_NOTICES.md`](THIRD_PARTY_NOTICES.md) and [`docs/footprint-provenance.md`](docs/footprint-provenance.md).

## OSHWA certification record

| Item | Record |
|---|---|
| Project | Thin-Pod Gateway |
| Hardware release | rev 0.1 |
| OSHWA UID | [`UK000092`](https://certification.oshwa.org/uk000092.html) |
| Certification date | 16 July 2026 |
| Canonical release tag | `v0.1` |
| Public application record | [`oshwa/application-record.md`](oshwa/application-record.md) |
| Certification scope | [`docs/certification-scope.md`](docs/certification-scope.md) |
| Certification completion checklist | [`docs/oshwa/Gateway_OSHWA_Preparation_Checklist.md`](docs/oshwa/Gateway_OSHWA_Preparation_Checklist.md) |

The OSHWA certification confirms the declared open-source hardware, software and documentation licensing position. It does not constitute EMC, radio, electrical-safety, CE, UKCA, industrial-performance or product-safety approval.

## Current limitations

The release does not claim Node-to-Gateway vibration-window transport, ranging, field-qualified RF performance, calibrated transmit power, antenna/coexistence performance, authenticated radio records, secure boot enforcement, production OTA, Gateway DSP/TinyML performance, EMC, radio, electrical-safety, CE or UKCA compliance.
