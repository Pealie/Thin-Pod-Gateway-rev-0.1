# NUCLEO Host Firmware: Gateway Analytic Supervisor

**Firmware area:** `firmware/nucleo-host/`<br>
**Hardware target:** STM32 NUCLEO-N657X0-Q mounted on Thin-Pod Gateway rev 0.1<br>
**Status:** Ingestion architecture specified; implementation and physical Gateway verification pending<br>
**Document date:** 27 May 2026<br>
**Primary role:** Authoritative admission control, raw-window ownership and later analysis

## Purpose

This directory is for firmware running on the **STM32 NUCLEO-N657X0-Q** in the Thin-Pod Gateway architecture.

The NUCLEO is intended to act as the analytic supervisor, not as a direct controller of the internal DW3110 radio. Its immediate job is to prove that a complete record exposed by Gateway-side DWM firmware can be pulled into NUCLEO-owned memory, checked for integrity, preserved and acknowledged correctly.

```text
Gateway-side DWM firmware
    complete record queued and ready
        ↓  TPHIP-1 SPI/IRQ host interface
NUCLEO host firmware
    admission control and complete raw-window buffer ownership
        ↓
minimal DSP sanity checks
        ↓
later feature extraction and TinyML evaluation
```

## Responsibility boundary

| NUCLEO owns | NUCLEO does not initially own |
|---|---|
| Host-interface command/read behaviour | DW3110 radio timing/control internals |
| Available raw-record buffer capacity | Node-side ADC qualification |
| Credit granting and backpressure decision | Uncontrolled incoming UWB streaming |
| Host-side CRC/metadata acceptance | Gateway DWM’s internal UWB reassembly implementation |
| Retention and processing scheduling | Optional XIAO reporting as a prerequisite for measurement receipt |
| Later DSP and TinyML execution | Any early predictive-maintenance performance claim |

The NUCLEO is the **authoritative admission controller** because it knows whether preserved raw-window capacity remains available for incoming measurements and downstream processing.

## Relationship to hardware and certification

Thin-Pod Gateway rev 0.1 is OSHWA-certified as `UK000092`, separately from the Thin-Pod rev 0.1 node certified as `UK000091`. Thin-Pod-authored NUCLEO firmware is licensed under MIT through `LICENSE-SOFTWARE.md` and is part of the declared Gateway software record. The commercial NUCLEO hardware and vendor software remain outside the Thin-Pod licence grants.

The commercial NUCLEO board is included as a dependency of the open Gateway carrier PCB; it is not claimed as creator-authored open hardware.

## Controlling documents

| Document | NUCLEO firmware requirement |
|---|---|
| `../../docs/dwm-to-nucleo-host-interface-protocol.md` | Implement `TPHIP-1` host/master transactions and golden record |
| `../../docs/data-path-flow-control-and-buffering.md` | Implement raw-record pool, credits and high-water policy |
| `../../docs/data-path-fault-injection-and-recovery-tests.md` | Implement observable handling of corruption/backpressure/reset faults |
| `../../docs/window-throughput-and-airtime-budget.md` | Measure host-transfer/queue/DSP timing |
| `../../docs/measurement-record-and-provenance-model.md` | Attach record/build/status identity to evidence |
| `../../docs/node-acquisition-and-sampling-contract.md` | Interpret actual sensor records cautiously and correctly |

## Directory structure

```text
nucleo-host/
├── README.md
└── tphip-reader/
    └── README.md
```

Potential implementation expansion:

```text
nucleo-host/
├── README.md
├── tphip-reader/
│   ├── README.md
│   └── evidence/
├── app/
│   ├── host-interface/
│   ├── raw-window-pool/
│   ├── dsp/
│   └── logging/
├── tests/
└── build-notes/
```

## Initial physical interface role

Gateway rev 0.1 connects the NUCLEO-side intended host route as follows:

| NUCLEO function | Gateway net | Purpose |
|---|---|---|
| SPI5 clock output | `SPI5_SCK` | Clock DWM host-interface transactions |
| SPI5 MOSI output | `SPI5_MOSI` | Send requests and dummy bytes during reads |
| SPI5 MISO input | `SPI5_MISO` | Receive status/header/payload bytes |
| GPIO chip select | `DWM_CS` | Select Gateway DWM host endpoint |
| GPIO interrupt input | `DWM_IRQ` | Observe record/status-ready notification |
| GPIO reset output | `DWM_RESET` | Controlled subsystem reset/recovery |

The intended first protocol uses NUCLEO as SPI host/master. Settings such as SPI mode, initial frequency and IRQ polarity remain commissioning values until measured on the firmware endpoints and assembled/temporary-wired hardware.

## First NUCLEO objective

The first NUCLEO objective is:

> **Read and validate the fixed 4160-byte `TPHIP-1` golden synthetic record from Gateway-side DWM firmware, then acknowledge ownership only after the bytes and CRC are accepted.**

This establishes the key modular boundary before actual UWB or ADXL1005 data is introduced.

## Host-interface task model

The NUCLEO firmware should separate ingress from all slower processing.

```text
DWM_IRQ GPIO event
        ↓
minimal interrupt handler sets service flag
        ↓
high-priority TPHIP receive task reads status/record
        ↓
record copied into allocated raw-window slot
        ↓
CRC/metadata/sequence checked
        ↓
ACK_WINDOW or REJECT_WINDOW sent
        ↓
accepted record placed in VALID_RAW queue
        ↓
DSP task consumes later
```

The interrupt handler must not perform bulk SPI reads, FFT calculation, TinyML inference, long logging operations or optional network reporting.

## Initial raw-window pool

The initial NUCLEO buffer policy is:

| Quantity | Initial value |
|---|---:|
| Complete record bytes | 4160 |
| Raw-window slots | 32 |
| Raw-record memory allocation | 133,120 bytes |
| Protected reserve | 4 free slots |
| First admitted-window policy | One outstanding credit at a time |

### Raw-window slot states

```text
FREE
    available for admitted record

FILLING
    SPI transfer in progress

VALID_RAW
    integrity/metadata accepted; available to downstream consumer

PROCESSING
    DSP task currently consuming record

PROCESSED_RETAIN
    retained because it supports evidence/result

RELEASED
    returned deliberately to free pool

INVALID_QUARANTINED
    rejected record retained for diagnosis as policy permits
```

### High-water admission policy

| Slots in use | State | Credit action |
|---:|---|---|
| 0–15 | Normal | Permit credit according to scheduler |
| 16–23 | Elevated occupancy | Continue cautiously and log latency |
| 24–27 | High-water warning | Withhold or severely restrict new credits |
| 28–31 | Protected reserve | No new measurement credit |
| 32 | Full | Reject unsolicited arrival as fault; never overwrite a valid raw record |

## TPHIP-1 host implementation stages

| Stage | Host-side feature | Pass criterion |
|---:|---|---|
| NH-0 | Establish build/debug route and firmware identity | NUCLEO build can run and identify itself |
| NH-1 | Configure GPIO and SPI host route | Safe idle state and observable intended signals |
| NH-2 | Read `GET_CAPABILITIES` / `GET_STATUS` | Stable valid responses from Gateway DWM |
| NH-3 | Service level `DWM_IRQ` outside lengthy ISR work | Pending status/record cannot be missed |
| NH-4 | Read golden descriptor/payload into one slot | Exact 4160-byte record reconstructed |
| NH-5 | Validate CRC and issue `ACK_WINDOW` | CRC equals `0x0B1D48B2`, DWM frees only after ACK |
| NH-6 | Deliberately withhold ACK | DWM retains unresolved record |
| NH-7 | Implement credit/high-water state | Admission stops before overwrite |
| NH-8 | Receive known UWB test record | RF-to-memory record accepted |
| NH-9 | Receive actual characterisation window | Sensor-derived raw record preserved |
| NH-10 | Calculate initial RMS/peak | Result links to retained raw record |

## Initial DSP boundary

DSP should begin only after a record has entered `VALID_RAW`.

| Processing stage | Purpose | Requirement |
|---|---|---|
| Mean/min/max/raw-range check | Detect offset/range/clipping anomalies | First actual characterisation record |
| RMS and peak / peak-to-peak | Basic time-domain sanity | Valid retained raw record |
| Crest factor and kurtosis | Initial impulsive-content measure | Repeatable acquisition/transport |
| FFT/band energy | Frequency-domain evidence | Sample rate/filter configuration qualified |
| Envelope work | Later fault-oriented analysis | Appropriate bandwidth and measurement data |
| TinyML | Much later model evaluation | Versioned dataset and provenance chain |

Early actual sensor records must remain labelled as characterisation/uncalibrated unless acquisition qualification supports stronger wording.

## Optional XIAO boundary

The XIAO ESP32-C6 is optional downstream reporting hardware. It is not required for the first NUCLEO host-interface or measurement-ingress proof.

For rev 0.1 testing:

```text
first ingress proof:     XIAO absent or inactive
later reporting test:    XIAO operations yield to DWM record servicing
```

Any XIAO task must not block reception or preservation of a validated measurement record.

## Evidence record requirements

| Evidence item | Record |
|---|---|
| Gateway hardware/build identity | Gateway PCB build or temporary-wired proof arrangement |
| NUCLEO hardware identity | NUCLEO board used |
| Firmware build identity | Source/build/tag |
| SPI/IRQ/reset configuration | Exact settings |
| TPHIP protocol version | `TPHIP-1` initially |
| Vector/record identity | Golden/synthetic/UWB/measurement and sequence |
| Queue state/counters | Before and after transfer/test |
| Integrity output | CRC and binary comparison where applicable |
| DSP output | Only where linked to valid raw record |
| Pass/fail | Explicit conclusion and noted limitations |

## Immediate next action

Before the Gateway PCB arrives, a strong NUCLEO task is to create the smallest host project that can:

1. configure the proposed SPI/GPIO route or a temporary-wired equivalent;
2. read a fixed capabilities/status response from Gateway-side DWM firmware; and
3. prepare the 4160-byte raw-buffer allocation and CRC validation needed for the golden-record transfer.

No DSP sophistication is required until the host-interface record hand-off is reliable.
