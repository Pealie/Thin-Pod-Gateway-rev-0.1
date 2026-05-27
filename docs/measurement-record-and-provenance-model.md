# Thin-Pod System: Measurement Record and Provenance Model

**Document identifier:** `TP-SYS-PROV-0001`  
**Document revision:** Draft A  
**Applies to:** Thin-Pod rev 0.1 node, Thin-Pod Gateway rev 0.1 and later processing experiments  
**Document status:** Data-governance and measurement-traceability design specification  
**Document date:** 27 May 2026  
**Certification boundary:** This document supports system development and later Gateway documentation. It does not expand the submitted OSHWA scope of Thin-Pod rev 0.1.

## 1. Purpose

This document defines how a Thin-Pod vibration measurement should remain traceable from physical sensing through acquisition, wireless transport, Gateway receipt, DSP processing and later TinyML inference.

The project’s value is not merely that a vibration record can be transmitted. It is that an eventual result can be examined in context:

```text
What was sensed?
How was it sampled?
How was it transported?
Was the raw record valid and preserved?
What processing was applied?
Which hardware, firmware and model versions produced the result?
```

A derived feature or inference without a preserved provenance chain is not sufficient evidence for a measurement-integrity platform.

## 2. Core principles

1. Raw measurement windows are primary evidence; derived features and classifications are secondary records.
2. A valid raw window is immutable once accepted into Gateway ownership.
3. Every derived result refers back to the exact raw record from which it was calculated.
4. Transport integrity is recorded separately from measurement calibration and analytic interpretation.
5. A CRC detects accidental corruption but is not proof of authenticity.
6. Cryptographic hashes should identify preserved/exported artifacts; later signatures/authentication may protect trust boundaries.
7. Missing, rejected or dropped records are themselves part of the evidential record.
8. Hardware certification scope and system-data provenance are related but not identical: OSHWA publication of hardware does not by itself prove a validated measurement or model.

## 3. Provenance layers

A complete traceability chain contains six layers.

| Layer | Record type | Question answered |
|---|---|---|
| Physical hardware | Device/build manifest | Which node, sensor, Gateway and modules were involved? |
| Acquisition | Acquisition configuration and raw-window header | How was the signal sampled? |
| Transport | Window transport event record | How did the record move and was integrity preserved? |
| Ingestion | Gateway ownership/validation record | When did NUCLEO accept the raw record? |
| Processing | DSP result record | Which transformations generated derived features? |
| Inference/reporting | Model/result/report record | Which model or outward message used which input evidence? |

## 4. Identifier scheme

Identifiers should be stable, human-readable where useful and unambiguous in data files.

### 4.1 Hardware identifiers

| Item | Suggested identifier format | Example |
|---|---|---|
| Node hardware family/revision | `TP-NODE-R<major>.<minor>` | `TP-NODE-R0.1` |
| Individual node instance | `TPN-<serial>` | `TPN-0001` |
| Gateway hardware family/revision | `TP-GW-R<major>.<minor>` | `TP-GW-R0.1` |
| Individual Gateway instance | `TPG-<serial>` | `TPG-0001` |
| Sensor channel | `<node_id>-S<channel>` | `TPN-0001-S01` |
| Qorvo subsystem instance | `<owner_id>-DWM<index>` | `TPG-0001-DWM01` |

### 4.2 Firmware and protocol identifiers

| Item | Suggested field | Example |
|---|---|---|
| Node firmware build | `node_firmware_build_id` plus repository commit/tag | `0x00000001`, `node-fw-v0.1.0-test1` |
| Gateway DWM firmware build | `gateway_dwm_firmware_build_id` | `dwm-gw-fw-v0.1.0-test1` |
| NUCLEO firmware build | `nucleo_firmware_build_id` | `nucleo-fw-v0.1.0-test1` |
| Host protocol | `host_protocol_version` | `TPHIP-1` |
| Window-record format | `record_version` | `1` |
| UWB transport framing version | `uwb_transport_version` | To be assigned when implemented |
| DSP feature-set version | `dsp_feature_set_version` | `DSP-V0.1` |
| TinyML model ID | `model_id` / `model_hash` | Assigned only when inference work begins |

### 4.3 Record identifiers

A raw window is uniquely identified by:

```text
node_id + window_sequence + record_crc32
```

For exported/archive material, add a cryptographic file hash:

```text
raw_record_sha256
```

The sequence number describes order; the CRC describes accidental-integrity checking; the SHA-256 hash identifies the archived binary artifact.

## 5. Hardware and session manifest

Before a measurement session, create a manifest describing the physical and software state.

### 5.1 Session manifest fields

| Field | Purpose | Required at first measurement proof? |
|---|---|---:|
| `session_id` | Unique test/measurement session identity | Yes |
| `session_start_utc` | Date/time reference | Yes where available |
| `operator` | Person conducting test | Yes for engineering evidence |
| `purpose` | Bring-up, shaker test, motor test, protocol test, etc. | Yes |
| `node_hardware_revision` | Node board design identity | Yes |
| `node_instance_id` | Physical node used | Yes |
| `gateway_hardware_revision` | Gateway board design identity | Yes |
| `gateway_instance_id` | Physical Gateway used | Yes |
| `node_qorvo_module_identity` | Commercial module/serial information where recorded | Recommended |
| `gateway_qorvo_module_identity` | Commercial module/serial information where recorded | Recommended |
| `sensor_identity` | ADXL1005 channel/board identity | Yes for actual measurement |
| `node_firmware_build` | Acquisition/transmit implementation identity | Yes |
| `gateway_dwm_firmware_build` | UWB receive/host interface identity | Yes |
| `nucleo_firmware_build` | Ingestion/DSP implementation identity | Yes |
| `host_protocol_version` | DWM-to-NUCLEO format/behaviour version | Yes |
| `uwb_transport_configuration` | Channel/PHY/security/transport parameters | Required once UWB used |
| `notes` | Deviations or setup detail | Recommended |

### 5.2 Proposed manifest example

```json
{
  "session_id": "TPS-2026-0001",
  "session_start_utc": "YYYY-MM-DDThh:mm:ssZ",
  "operator": "Neil Thomson",
  "purpose": "Gateway synthetic-record commissioning",
  "node_hardware_revision": null,
  "node_instance_id": null,
  "gateway_hardware_revision": "TP-GW-R0.1",
  "gateway_instance_id": "TPG-0001",
  "sensor_identity": null,
  "node_firmware_build": null,
  "gateway_dwm_firmware_build": "dwm-gw-fw-v0.1.0-test1",
  "nucleo_firmware_build": "nucleo-fw-v0.1.0-test1",
  "host_protocol_version": "TPHIP-1",
  "uwb_transport_configuration": null,
  "notes": "Synthetic DWM-to-NUCLEO host-interface proof; no sensor or UWB data."
}
```

Null fields are acceptable only where a session deliberately does not use that layer, such as a wired-only synthetic host-interface proof.

## 6. Raw vibration-window record

The primary evidence object is the immutable complete raw record described by TPHIP-1.

### 6.1 Raw record structure

| Record component | Purpose |
|---|---|
| 64-byte header | Identifies origin, sequence, sample interpretation and record CRC |
| Raw sample payload | Preserved acquired or generated sample values |
| `record_crc32` embedded in header | Detects accidental corruption across header metadata and raw payload |

Initial commissioning record:

```text
64-byte header + 4096-byte raw payload = 4160 bytes
```

### 6.2 Minimum metadata required for a valid actual measurement window

| Field | Requirement |
|---|---|
| `message_type` | Must be `VIBRATION_WINDOW`, not a synthetic/test type |
| `node_id` | Must identify physical source node |
| `window_sequence` | Must be present and monotonic within session, with gaps recorded |
| `sample_rate_hz` | Must be known |
| `sample_count` | Must match payload |
| `sample_format` | Must be interpretable |
| `sensor_id` | Must identify external measurement channel |
| `calibration_version` | May initially identify uncalibrated/characterisation state, but must not be misleading |
| `acquisition_flags` | Must indicate clipping, test source, incomplete sample state or unknown calibration where applicable |
| `node_firmware_build_id` | Must identify acquisition implementation |
| `record_crc32` | Must pass before record is accepted into valid raw queue |

A record missing essential metadata may remain in a diagnostic archive but should not be presented as a valid calibrated measurement.

## 7. Acquisition provenance

The raw record header cannot by itself contain every important acquisition setting. Each session or configuration revision should include an acquisition configuration record.

| Acquisition-provenance field | Purpose |
|---|---|
| `sensor_model` | Identifies ADXL1005-based external measurement source |
| `sensor_board_or_interface` | Identifies evaluation board or later integrated sensor implementation |
| `analogue_filter_definition` | Resistor/capacitor values or characterised filter response |
| `adc_device` | Identifies nRF52833 ADC route or later dedicated ADC |
| `adc_input_pin` | Connects schematic to firmware configuration |
| `adc_reference_configuration` | Required for interpreting voltage/code conversion |
| `adc_gain_or_input_range` | Required for clipping margin and acceleration conversion |
| `sample_rate_hz` | Defines timing/frequency content |
| `sampling_method` | Timer/PPI/DMA/interrupt or later implementation description |
| `window_sample_count` | Defines window duration |
| `dropped_sample_detection` | States how timing discontinuity is detected |
| `clipping_threshold` | Defines how clipping flag is generated |
| `calibration_version` | Links record to acceleration scaling/state |
| `qualification_status` | `UNQUALIFIED`, `CHARACTERISATION`, `QUALIFIED_FOR_BAND`, etc. |

### 7.1 Acquisition qualification states

| State | Meaning |
|---|---|
| `UNQUALIFIED` | Digital acquisition exists but measurement quality has not been established |
| `CHARACTERISATION` | Used to collect noise, clipping, timing and frequency-response evidence |
| `QUALIFIED_FOR_BAND` | Demonstrated for a stated bandwidth/sample-rate/configuration range |
| `CALIBRATED` | Scaling/calibration record exists for stated setup |
| `INVALIDATED` | Previously used configuration no longer accepted due to discovered issue/change |

The first node-to-Gateway raw sensor transfer should initially be labelled `CHARACTERISATION`, not silently presented as calibrated condition-monitoring data.

## 8. Transport provenance

The Gateway must record how the raw record reached NUCLEO ownership.

| Transport field | Purpose |
|---|---|
| `uwb_transport_version` | Identifies fragmentation/admission protocol |
| `uwb_channel` / PHY configuration | Makes radio setup explicit |
| `uwb_data_rate` | Records selected operating mode |
| `security_or_sts_configuration` | Distinguishes integrity/security state where implemented |
| `credit/grant_id` | Identifies admitted transmission opportunity |
| `fragment_count_expected` | Defines complete transport unit |
| `fragment_count_received` | Allows incomplete delivery analysis |
| `retransmission_count` | Quantifies transport recovery cost |
| `gateway_dwm_receive_tick` | Receive/reassembly timing |
| `gateway_dwm_validation_status` | Record CRC/reassembly outcome |
| `gateway_dwm_firmware_build` | Identifies receiving implementation |
| `host_transfer_status` | Accepted/rejected/timeout/reset outcome |
| `host_protocol_version` | Identifies DWM-to-NUCLEO interface contract |
| `nucleo_ingest_tick` | Records host ownership timing |
| `nucleo_validation_status` | Records host-side CRC/metadata acceptance |

## 9. Ingestion record

For each raw record presented to the NUCLEO, create a Gateway ingestion event.

### 9.1 Ingestion-event schema

| Field | Example / meaning |
|---|---|
| `session_id` | Session manifest link |
| `node_id` | Source node |
| `window_sequence` | Raw record order |
| `record_crc32` | Embedded record integrity value |
| `record_status` | `VALID_RAW`, `INVALID_CRC`, `INCOMPLETE`, `REJECTED`, `DUPLICATE`, `MISSING_GAP_RECORDED` |
| `dwm_receive_status` | Gateway communications-subsystem decision |
| `host_transfer_status` | SPI/IRQ interface decision |
| `nucleo_buffer_slot` | Diagnostic runtime slot identifier, where useful |
| `queue_occupancy_at_acceptance` | Supports throughput/backpressure analysis |
| `credit_state` | Admission capacity at event time |
| `retry_count` | Transport retry context |
| `error_flags` | Explicit error markers |
| `gateway_dwm_firmware_build` | Provenance |
| `nucleo_firmware_build` | Provenance |
| `ingest_timestamp_or_tick` | Timing |
| `raw_record_file` | Archive filename, once exported |
| `raw_record_sha256` | Stable artifact hash after export/storage |

### 9.2 Validity rule

Only ingestion records with:

```text
record_status = VALID_RAW
```

may enter the normal DSP-processing queue. Other records remain useful diagnostic evidence but are not valid analytic inputs.

## 10. DSP result provenance

Derived features must remain connected to a valid raw record.

### 10.1 Initial DSP result record

| Field | Purpose |
|---|---|
| `session_id` | Experiment/session identity |
| `node_id` | Source |
| `window_sequence` | Links to raw record |
| `raw_record_sha256` | Exact raw input artifact identity |
| `raw_record_crc32` | In-system record check |
| `dsp_feature_set_version` | Defines algorithm set and conventions |
| `nucleo_firmware_build` | Implementation identity |
| `processing_timestamp_or_tick` | Processing order/timing |
| `sample_rate_hz` | Confirms interpretation basis |
| `sample_count` | Confirms window basis |
| `units_or_scaling_state` | Codes, volts, g or uncalibrated state |
| `rms` | Initial DSP feature |
| `peak` / `peak_to_peak` | Initial amplitude sanity feature |
| `crest_factor` | Later initial fault-sensitive feature |
| `kurtosis` | Later initial fault-sensitive feature |
| `band_energy_definition` and values | Later versioned frequency-domain feature |
| `envelope_definition` and values | Later versioned envelope feature |
| `processing_flags` | Clipping, invalid calibration, test-only, etc. |

### 10.2 DSP output rule

A DSP result must state whether values are:

| State | Meaning |
|---|---|
| `RAW_ADC_CODES` | No acceleration calibration claimed |
| `VOLTAGE_SCALED` | ADC conversion to voltage has been applied and configuration is known |
| `ACCELERATION_UNCALIBRATED` | Nominal sensor scaling used without formal calibration |
| `ACCELERATION_CALIBRATED` | Calibration/version record supports stated units |

Early experiments should not imply calibrated acceleration units where only raw codes or nominal conversion exists.

## 11. TinyML inference provenance

TinyML must remain a later consumer of preserved evidence, not a substitute for it.

| Inference field | Purpose |
|---|---|
| `model_id` | Human-readable model identity |
| `model_sha256` | Exact model binary identity |
| `training_dataset_id` | Connects inference model to training evidence |
| `feature_set_version` | Defines model input representation |
| `preprocessing_build_or_version` | Reproduces transformations |
| `raw_record_sha256` or feature-record hash | Links inference to evidence |
| `inference_result` | Output class/score |
| `decision_thresholds` | Defines interpretation |
| `inference_firmware_build` | Identifies deployed code |
| `model_status` | `EXPERIMENTAL`, `VALIDATION`, `DEPLOYABLE` only after justified evidence |
| `limitations` | Known applicability limits |

No TinyML classification should be presented as a condition-monitoring conclusion unless the originating raw record, transport status, acquisition qualification and model provenance are available.

## 12. Integrity, authenticity and trust distinctions

### 12.1 CRC role

`record_crc32` is an application-level accidental-corruption check. It is suitable for:

- detecting changed bytes during transport/reassembly/host transfer;
- supporting golden-vector verification; and
- preventing corrupt data from silently entering DSP.

It is **not** proof that a record came from a trusted node or was not maliciously altered.

### 12.2 SHA-256 artifact role

Once a raw record, session manifest or DSP result is exported/stored, calculate and record SHA-256 hashes. These hashes identify exact retained artifacts and permit later checking that repository/archive evidence has not changed inadvertently.

### 12.3 Future authentication role

Later system security work may introduce:

- node identity keys;
- authenticated firmware;
- authenticated/encrypted transport;
- signed session manifests or evidence bundles;
- secure model/version records; and
- decision audit trails.

Those are future trust-system additions. They should not be implied merely by the presence of CRC or OSHWA-open hardware documentation.

## 13. Suggested evidence archive structure

A measurement/session archive should be organised so that raw evidence remains directly accessible.

```text
datasets/
└── TPS-2026-0001/
    ├── session-manifest.json
    ├── acquisition-configuration.json
    ├── transport-configuration.json
    ├── hashes.sha256
    │
    ├── raw/
    │   ├── TPN-0001-W00000001.tpwn
    │   └── TPN-0001-W00000002.tpwn
    │
    ├── ingestion/
    │   └── ingestion-events.ndjson
    │
    ├── dsp/
    │   ├── dsp-feature-set-definition.md
    │   └── dsp-results.csv
    │
    ├── inference/
    │   ├── model-manifest.json
    │   └── inference-results.ndjson
    │
    └── evidence/
        ├── test-notes.md
        ├── oscilloscope/
        └── logic-analyser/
```

### 13.1 Raw filename format

```text
<node_id>-W<window_sequence_zero_padded>.tpwn
```

Example:

```text
TPN-0001-W00000001.tpwn
```

The `.tpwn` extension denotes a Thin-Pod window record conforming to the relevant recorded `record_version`.

## 14. Suggested JSON ingestion event

```json
{
  "session_id": "TPS-2026-0001",
  "node_id": "TPN-0001",
  "window_sequence": 1,
  "record_version": 1,
  "record_crc32": "0x0B1D48B2",
  "record_status": "VALID_RAW",
  "message_type": "SYNTHETIC_WINDOW",
  "host_protocol_version": "TPHIP-1",
  "gateway_hardware_revision": "TP-GW-R0.1",
  "gateway_dwm_firmware_build": "dwm-gw-fw-v0.1.0-test1",
  "nucleo_firmware_build": "nucleo-fw-v0.1.0-test1",
  "queue_occupancy_at_acceptance": 1,
  "credit_state_after_acceptance": 0,
  "raw_record_file": "raw/TPN-0001-W00000001.tpwn",
  "raw_record_sha256": "TO_BE_CALCULATED_ON_ARCHIVE_WRITE",
  "notes": "Golden synthetic host-interface commissioning record; not a physical vibration measurement."
}
```

## 15. Required records by development milestone

| Milestone | Raw record | Session/acquisition manifest | Transport event | DSP record | Model record |
|---|---:|---:|---:|---:|---:|
| DWM-to-NUCLEO synthetic buffer proof | Yes, marked synthetic | Session and protocol manifest required; acquisition not applicable | Host-interface event required | Not required | Not allowed/required |
| UWB known-test-record proof | Yes, marked UWB test | Session and transport configuration required | Required | Not required | Not allowed/required |
| First actual vibration window | Yes, marked measurement/characterisation | Acquisition configuration required | Required | Optional first RMS/peak proof | Not required |
| DSP baseline | Yes, retained | Acquisition and processing configuration required | Required | Required | Not required |
| TinyML exploration | Yes or explicitly linked feature-source records retained | Full chain required | Required | Required where preprocessing uses DSP | Required |

## 16. Publication and privacy boundary

Public project repositories may publish:

- hardware design files and documentation;
- synthetic golden records;
- test methodology and anonymised/appropriate measurement evidence;
- firmware/version references;
- DSP method definitions and representative results.

Before publishing acquired datasets, review whether any test context, location, asset identity or recorded metadata should remain private or be generalised. Open hardware does not require publication of every operational dataset or private installation detail.

## 17. Relationship to OSHWA documentation

For a future OSHWA certification application for Thin-Pod Gateway rev 0.1, this document supports the design narrative and evidential discipline. It is not necessary to claim complete DSP/TinyML operation as part of a carrier-PCB certification scope.

The Gateway documentation should remain precise:

| Statement | Appropriate status before system proof |
|---|---|
| Gateway carrier PCB is designed for modular window receipt and analytic supervision | Appropriate design claim |
| Raw-window provenance model and protocol are published | Appropriate documentation claim |
| DWM-to-NUCLEO synthetic-buffer path has passed | State only after measured evidence exists |
| UWB vibration-window transfer has passed | State only after measured evidence exists |
| DSP or TinyML results are valid for monitoring | State only after full supporting evidence exists |

## 18. Documented record-retention decision

During initial commissioning:

| Record type | Retention policy |
|---|---|
| Golden synthetic records | Retain representative input vector, expected CRC and pass/fail evidence |
| Invalid/corrupt/fault-injected records | Retain enough examples and event logs to prove failure behaviour |
| First actual vibration records | Retain raw binary record, metadata, processing output and test context |
| Records associated with any later alert/classification | Retain raw evidence and full processing/model provenance |
| Routine later records | Policy to be defined against storage and research objectives |

## 19. References

- `docs/dwm-to-nucleo-host-interface-protocol.md`.
- `docs/data-path-flow-control-and-buffering.md`.
- `docs/data-path-fault-injection-and-recovery-tests.md`.
- `docs/window-throughput-and-airtime-budget.md`.
- `docs/node-acquisition-and-sampling-contract.md` — to be added.
- `docs/system-interface-control-document.md`.
- Analog Devices, [ADXL1005 product information](https://www.analog.com/en/products/adxl1005.html).
- Qorvo, [DWM3001C product information](https://www.qorvo.com/products/p/DWM3001C).
- STMicroelectronics, [STM32N657X0 product information](https://www.st.com/en/microcontrollers-microprocessors/stm32n657x0.html).

## 20. Engineering position

Thin-Pod should treat raw vibration windows as evidence-bearing records. Transport success, DSP outputs and later model decisions are valuable only where their relationship to the original measurement remains visible. The provenance model therefore belongs at the architectural core of the project, not as post-hoc reporting added after a model appears to work.
