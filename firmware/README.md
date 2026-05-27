# Thin-Pod Firmware Workspace

**Project context:** Thin-Pod rev 0.1 sensor node and Thin-Pod Gateway rev 0.1  
**Firmware workspace status:** Prepared for initial acquisition and host-interface experiments; no completed firmware implementation claimed  
**Document date:** 27 May 2026  
**Related hardware status:** Thin-Pod rev 0.1 OSHWA application submitted; Thin-Pod Gateway rev 0.1 PCB ordered and awaiting bring-up

## Purpose

This directory is the firmware-development workspace for proving the first Thin-Pod measurement-data path:

```text
external ADXL1005 analogue vibration signal
        ↓
node-side DWM3001-CDK / nRF52833 firmware
bounded raw-window acquisition and later UWB transmission
        ↓
Gateway-side DWM3001-CDK / nRF52833 firmware
communications termination, validation and host presentation
        ↓
STM32 NUCLEO-N657X0-Q firmware
record ingestion, preservation, DSP and later TinyML evaluation
```

The immediate purpose of the firmware work is not to claim a complete condition-monitoring product. It is to remove the two largest implementation uncertainties:

1. whether the existing Thin-Pod node can create a useful digital raw window from the external ADXL1005 signal; and
2. whether the Gateway-side Qorvo subsystem can expose a complete validated record to the NUCLEO through the firmware-defined host interface intended by Gateway rev 0.1.

## Relationship to hardware releases and OSHWA scope

The firmware workspace must not blur the hardware certification boundaries.

|Hardware artefact|Current position|Firmware relationship|
|-|-|-|
|**Thin-Pod rev 0.1** sensor-node carrier PCB|Separate OSHWA application submitted on 26 May 2026|This workspace may test firmware using that hardware, but it does not retrospectively expand the submitted hardware scope|
|**Thin-Pod Gateway rev 0.1** carrier PCB|Separate future OSHWA candidate; PCB ordered and bring-up pending|Firmware in this workspace may later support the Gateway release and must be separately licensed if published as part of that release|
|**Gateway rev 0.3** SMT/chip-down direction|Deferred|Not implemented in this workspace at present|

The Gateway hardware licence and documentation licence do not automatically license firmware. Before public firmware release or any Gateway OSHWA submission that includes supplied software, add a suitable `LICENSE-SOFTWARE.md`, apply appropriate source-file notices and ensure the OSHWA software-licence declaration matches the released firmware scope.

## Architectural responsibility split

|Firmware area|Hardware target|Immediate responsibility|Deliberately deferred responsibility|
|-|-|-|-|
|[`node-dwm/`](node-dwm/)|Node-side Qorvo DWM3001-CDK / nRF52833 connected to Thin-Pod|SAADC characterisation of external ADXL1005 path and bounded raw-record creation|Full event-trigger logic, multi-node policy and production-grade radio behaviour|
|[`gateway-dwm/`](gateway-dwm/)|Gateway-side Qorvo DWM3001-CDK / nRF52833|Present status and a golden synthetic record to the NUCLEO through the proposed host protocol|Full UWB reception until wired host hand-off succeeds|
|[`nucleo-host/`](nucleo-host/)|STM32 NUCLEO-N657X0-Q|Read, validate and preserve complete raw records; later perform minimal DSP|TinyML, cloud/backhaul reporting and complex user interface|
|[`test-vectors/`](test-vectors/)|Shared development assets|Provide deterministic record bytes and CRC reference behaviour|Dataset/training material|

The project’s governing data-path principle is:

```text
qualified raw measurement windows from the Pod;
validated UWB/host transfer through the DWM3001 subsystem;
features and later models on the Gateway.
```

## Directory structure

```text
firmware/
├── README.md
│
├── node-dwm/
│   ├── README.md
│   └── acquisition-characterisation/
│       └── README.md
│
├── gateway-dwm/
│   ├── README.md
│   └── tphip-golden-record/
│       └── README.md
│
├── nucleo-host/
│   ├── README.md
│   └── tphip-reader/
│       └── README.md
│
└── test-vectors/
    ├── README.md
    ├── golden-window-v1.bin              # to be generated from the reference script
    ├── golden-window-v1-metadata.md      # to be added with generated vector
    └── crc-reference.py                  # to be added as executable vector generator/checker
```

The README files define the intended implementation and evidence structure. The binary vector and reference script are next implementation assets; they are not implied to exist merely because their target paths are documented here.

## Controlling architecture documents

These firmware folders implement the architecture described in the repository documentation:

|Document|Controls|
|-|-|
|[`../docs/node-acquisition-and-sampling-contract.md`](../docs/node-acquisition-and-sampling-contract.md)|ADC qualification, sample-rate/bandwidth position and the later dedicated-ADC decision gate|
|[`../docs/dwm-to-nucleo-host-interface-protocol.md`](../docs/dwm-to-nucleo-host-interface-protocol.md)|`TPHIP-1` request/response, record layout, golden vector and ownership behaviour|
|[`../docs/data-path-flow-control-and-buffering.md`](../docs/data-path-flow-control-and-buffering.md)|Credit-based admission, DWM and NUCLEO queue policy and overrun prevention|
|[`../docs/window-throughput-and-airtime-budget.md`](../docs/window-throughput-and-airtime-budget.md)|Calculated record/transfer budget and later measurement tables|
|[`../docs/data-path-fault-injection-and-recovery-tests.md`](../docs/data-path-fault-injection-and-recovery-tests.md)|Failure tests needed to demonstrate visible error handling|
|[`../docs/measurement-record-and-provenance-model.md`](../docs/measurement-record-and-provenance-model.md)|Traceability from raw record through DSP and later TinyML|
|[`../docs/system-interface-control-document.md`](../docs/system-interface-control-document.md)|Node/Gateway responsibility boundary|

## First implementation sequence

The firmware should be developed in an evidence-first order.

|Phase|Target|Work|Completion evidence|
|-:|-|-|-|
|F0|Shared test assets|Generate the golden 4160-byte `SYNTHETIC\_WINDOW` record and CRC checker|Binary vector, metadata and deterministic script|
|F1|Gateway DWM|Implement minimal `TPHIP-1` capabilities/status presentation|Repeatable host-readable status record|
|F2|NUCLEO host|Implement SPI/IRQ reader for capabilities/status|Stable protocol discovery and status logging|
|F3|Gateway DWM + NUCLEO|Transfer the golden record over the wired host route|Exact reconstructed binary and CRC match|
|F4|Gateway DWM + NUCLEO|Withhold acknowledgement and exercise credit policy|Record-retention and no-overwrite evidence|
|F5|Node DWM|Configure SAADC and capture external ADXL1005 characterisation windows|Raw rest and mechanical-response records, marked uncalibrated|
|F6|Qorvo node/Gateway subsystems|Transfer a known record over UWB and deliver it into NUCLEO memory|RF-to-host record with integrity evidence|
|F7|Full node-to-Gateway chain|Deliver an actual ADXL1005-derived raw window|Accepted measurement record plus provenance|
|F8|NUCLEO|Compute RMS and peak, followed later by further DSP|Results linked to retained raw input|
|F9|Later|Consider TinyML only after repeatable datasets exist|Versioned model work linked to provenance|

Node acquisition and wired host-interface work can proceed in parallel where hardware availability permits. Neither requires the Gateway PCB to have arrived before useful preparatory progress begins.

## Initial record and buffer policy

The commissioning record defined by `TPHIP-1` is:

|Quantity|Initial value|
|-|-:|
|Message type|`SYNTHETIC\_WINDOW`, later `VIBRATION\_WINDOW`|
|Sample representation|Signed 16-bit little-endian|
|Samples per record|2048|
|Raw payload bytes|4096|
|Fixed header bytes|64|
|Complete record bytes|4160|
|Golden vector CRC-32|`0x0B1D48B2`|
|Gateway DWM complete-record slots|2 initially|
|NUCLEO raw-record slots|32 initially|
|Commissioning admission|One complete window at a time|

The 25.6 ksample/s value in the golden synthetic record is protocol metadata for deterministic testing; it is not a claim that a 25.6 ksample/s acquisition mode preserves the full ADXL1005 analogue bandwidth. Actual node acquisition requires the separate qualification process described in `node-acquisition-and-sampling-contract.md`.

## Evidence and record discipline

Each firmware experiment should produce identifiable evidence rather than transient console output.

Suggested working evidence pattern:

```text
verification/
├── firmware/
│   ├── node-acquisition/
│   ├── tphip-host-interface/
│   ├── uwb-transport/
│   └── dsp/
└── datasets/
    └── <session-id>/
        ├── session-manifest.json
        ├── raw/
        ├── ingestion/
        ├── dsp/
        └── evidence/
```

Every completed experiment should record the hardware revision/build, fitted modules, firmware build identities, protocol version, test configuration, raw record or exact test vector, integrity result and explicit pass/fail conclusion.

## Firmware toolchain position

The precise firmware toolchains and SDK/project templates should be recorded when the first projects are created. The likely development boundaries are:

|Target|Expected environment to confirm and record|
|-|-|
|DWM3001-CDK / DWM3001C nRF52833|Qorvo-supported DWM3001CDK/DWM3001C firmware environment and Nordic-compatible development/debug route|
|STM32 NUCLEO-N657X0-Q|STM32Cube ecosystem and NUCLEO-supported debugging/programming route|
|Shared test vector generation|Python reference tooling used only for deterministic test/evidence generation|

No dependency version should be treated as fixed until the corresponding firmware project is created, built, flashed and recorded.

## Non-claims

This directory does not yet claim:

* working node-side digital capture of the external ADXL1005 signal;
* working DWM-to-NUCLEO SPI protocol implementation;
* successful UWB transmission of vibration records;
* proven DSP performance;
* TinyML inference or predictive-maintenance capability;
* a final firmware licence; or
* inclusion of firmware in either the submitted Thin-Pod node certification or a submitted Gateway certification application.

## Immediate next action

The best first firmware actions are:

```text
1. Generate and commit the TPHIP-1 golden record test assets.
2. Establish the node-side DWM3001-CDK programming/build route.
3. Capture the first external-ADXL1005-derived bounded ADC record.
4. Establish the Gateway-side DWM ↔ NUCLEO minimum status exchange off-board where practical.
5. Use the arriving Gateway PCB to replace temporary wiring only after the firmware boundary is already understood.
```

A credible outcome before the Gateway PCB arrives would be one digitised ADXL1005 characterisation window, a committed golden test vector and at least the beginnings of a deterministic DWM-to-NUCLEO host-interface proof.

