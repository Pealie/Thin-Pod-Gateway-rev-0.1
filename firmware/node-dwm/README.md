# Node DWM Firmware: ADXL1005 Acquisition and Node-Side Window Creation

**Firmware area:** `firmware/node-dwm/`  
**Hardware target:** Node-side Qorvo DWM3001-CDK / DWM3001C nRF52833 interfaced by Thin-Pod rev 0.1  
**Status:** Firmware work planned; digital acquisition not yet demonstrated  
**Document date:** 27 May 2026  
**Measurement role:** Create bounded raw vibration-window records from the external ADXL1005 analogue path

## Purpose

This directory is for firmware running on the **node-side Qorvo DWM3001 subsystem** used with Thin-Pod. Its first responsibility is to determine whether the existing node hardware can digitise the external ADXL1005 analogue output into a stable and useful raw sample window.

The node-side firmware is not initially responsible for sophisticated DSP or TinyML. Its proper early role is:

```text
external ADXL1005 analogue output
        ↓
nRF52833 SAADC acquisition under controlled conditions
        ↓
bounded raw sample window with explicit metadata and integrity field
        ↓
later UWB transmission under Gateway admission control
```

The acquisition work is central because a perfectly transported buffer is not valuable if the source samples are aliased, clipped, noisy or undocumented.

## Hardware and measurement boundary

The primary measurement source is the **external ADXL1005-based signal path on Thin-Pod**, not the lower-bandwidth motion sensor present within the DWM3001C module.

| Source | Firmware treatment |
|---|---|
| External ADXL1005 analogue channel connected through the Thin-Pod ADC node to the DWM/nRF52833 interface | Primary raw vibration-measurement channel to be qualified |
| DWM3001C integrated motion sensor | Auxiliary only; may later support wake-up/orientation/health functions but is not a substitute for the external vibration channel |

Existing analogue bring-up evidence for Thin-Pod rev 0.1 includes an approximately 3.35 V switched sensor supply, approximately 1.76 V resting filtered analogue node and a visible oscilloscope response to tapping. This firmware directory begins the next stage: capturing those signals digitally and assessing their quality.

## Relationship to certification scope

Thin-Pod rev 0.1 has been submitted as an open-hardware sensor-node carrier PCB. Firmware developed in this folder is continuing system-development material and does not retroactively expand that submitted hardware certification scope.

Before any node firmware is publicly released as a formally maintained software deliverable, it must be licensed separately under an appropriate software licence and identified by build/version metadata.

## Controlling specification

The controlling acquisition document is:

```text
docs/node-acquisition-and-sampling-contract.md
```

That document states the engineering gate:

> The nRF52833 SAADC may be used for initial bounded-window characterisation and may qualify for a restricted measurement band, but full use of the ADXL1005’s approximately 20–23 kHz low-noise capability must not be claimed without convincing filtering, timing, noise and alias evidence.

Relevant related documents are:

| Document | Use by node firmware |
|---|---|
| `../../docs/node-acquisition-and-sampling-contract.md` | ADC qualification and retain-or-replace acquisition decision |
| `../../docs/dwm-to-nucleo-host-interface-protocol.md` | Complete-record format eventually consumed by the Gateway |
| `../../docs/data-path-flow-control-and-buffering.md` | Later window-admission and backpressure behaviour |
| `../../docs/measurement-record-and-provenance-model.md` | Metadata and traceability requirements |

## Directory structure

```text
node-dwm/
├── README.md
└── acquisition-characterisation/
    └── README.md
```

As implementation develops, a sensible expanded layout is:

```text
node-dwm/
├── README.md
├── acquisition-characterisation/
│   ├── README.md
│   ├── configurations/
│   ├── captures/
│   └── evidence/
├── app/
│   ├── acquisition/
│   ├── record-format/
│   └── transport/
├── tests/
└── build-notes/
```

The project structure may be adapted to the selected Qorvo/Nordic-compatible build environment, but source responsibility and evidence must remain clear.

## First firmware objective

The first firmware objective is not UWB transmission. It is:

> **Capture a bounded digital record from the external ADXL1005 channel using the node-side nRF52833 SAADC, and preserve it as `CHARACTERISATION / UNCALIBRATED` evidence.**

Minimum first records:

| Record | Condition | Why required |
|---|---|---|
| Resting/DC sanity record | Stationary sensor, known rail voltage, radio inactive | Maps analogue rest output to ADC codes and detects clipping/configuration errors |
| Quiet/background record | Stable resting sensor, radio inactive | Begins combined sensor/ADC/supply noise characterisation |
| Mechanical-response record | Controlled tap or simple excitation | Confirms the digital path records the previously observed analogue response |
| Radio-comparison record | Capture under or near radio activity, once feasible | Assesses whether capture and UWB transmission must be time-separated |

## nRF52833 SAADC qualification concerns

| Qualification issue | Firmware evidence required |
|---|---|
| Correct ADC input routing | Source pin/net and configured SAADC input recorded |
| Reference/gain/input range | Firmware configuration plus conversion calculation |
| Resting code and voltage relation | Paired ADC code and multimeter/oscilloscope observation |
| Clipping margin | Excitation/input tests and flags where clipping detected |
| Sampling stability | Timer/EasyDMA/event configuration and missing-sample checks |
| Noise floor | Captured raw record plus derived RMS/spectrum |
| Aliasing/filter compatibility | Defined analogue filtering and frequency-response testing |
| Radio contamination | Paired captures with radio inactive and active/adjacent activity |

The node’s existing approximately 29.3 kHz single-pole analogue filter is suitable bring-up evidence but not yet a final anti-alias strategy for high-bandwidth measurement.

## Sampling strategy

### Synthetic versus measured records

Synthetic records may use sample-rate fields solely to test protocol behaviour. Real ADXL1005-derived records must state the actual acquisition configuration and qualification status.

| Record type | Permitted purpose | Measurement claim |
|---|---|---|
| `SYNTHETIC_WINDOW` | Byte/CRC/protocol verification | No physical measurement claim |
| `UWB_TEST_WINDOW` | Radio/transport proof | No sensor-quality claim unless sourced from qualified acquisition |
| `VIBRATION_WINDOW` with `CHARACTERISATION` state | First actual ADXL1005 captures | Uncalibrated engineering evidence only |
| `VIBRATION_WINDOW` with qualified-band configuration | Later stated-band experiments | Only after supporting evidence exists |

### Candidate acquisition modes

| Sample-rate class | Initial role |
|---:|---|
| Low/restricted-band configuration | First measured captures where filtering and interpretation remain unambiguous |
| 51.2 ksample/s-class mode | Candidate for moderate/high-band characterisation, not automatically qualified |
| Up to 200 ksample/s single-channel characterisation | Candidate for examining alias/spur/noise behaviour within nRF52833 SAADC capability |

The final selection depends on measured ADC behaviour and appropriate analogue filtering.

## Initial raw-window record target

Where the acquisition record is aligned with the Gateway protocol commissioning format:

| Field | Initial target |
|---|---:|
| Sample format | Signed 16-bit little-endian representation of ADC values |
| Samples per record | 2048 |
| Raw payload size | 4096 bytes |
| Header size | 64 bytes |
| Total record size | 4160 bytes |
| Message type for first sensor captures | `VIBRATION_WINDOW` |
| Calibration state | `CHARACTERISATION` / uncalibrated |
| Required flags | State clipping, test source, incomplete capture or unknown calibration where applicable |

A raw ADC record may initially preserve ADC codes rather than calibrated `g` units. That is the correct evidence position until scaling and calibration have been established.

## Later node-to-Gateway transport role

After digital acquisition is demonstrated, node firmware may implement bounded-window UWB transfer governed by Gateway admission.

Intended sequence:

```text
Gateway grants one window opportunity
        ↓
Node captures or selects one complete bounded record
        ↓
Node frames and transmits the record over UWB
        ↓
Gateway-side DWM validates and presents it to NUCLEO
```

For first proof, node firmware should not transmit uncontrolled continuous raw streams and should not implement event-triggered queuing before the simpler admitted-window path has been demonstrated.

## Evidence record requirements

Every actual sensor capture retained for evaluation should identify:

| Required item | Example or status |
|---|---|
| Node hardware revision | `TP-NODE-R0.1` |
| Physical node instance | Assign when first digital test is made |
| DWM/CDK instance | Record module identity where practical |
| Node firmware build | Commit/build/tag |
| ADC input/reference/gain configuration | Exact firmware setting |
| Sample rate and sample count | Exact values |
| Analogue filter version/values | Thin-Pod rev 0.1 path or later revision |
| Sensor/channel identity | External ADXL1005 channel |
| Radio state during capture | Inactive, active or adjacent transmit event |
| Qualification state | `CHARACTERISATION`, initially |
| Raw record CRC/hash | Integrity and archive identity |
| Notes | Mounting, excitation and anomalies |

## Acceptance gate for first success

The first node-side firmware milestone passes when:

1. the external ADXL1005 ADC path is mapped unambiguously into firmware;
2. one 2048-sample digital raw record is captured from the external sensor path;
3. rest and mechanical-response records are retained with acquisition settings;
4. no unexplained clipping or buffer/timing failure occurs; and
5. the evidence is labelled as characterisation data rather than calibrated diagnostic measurement.

This does not prove full-bandwidth measurement quality. It proves that Thin-Pod can create real candidate raw data for subsequent transport and qualification.

## Deferred work

Do not prioritise the following until first characterisation records exist:

- TinyML on the node;
- large DSP feature sets on the node;
- event-triggered local queueing;
- multi-node scheduling;
- continuous streaming;
- firm claims about 20–23 kHz measurement performance; or
- a decision to eliminate the possibility of a later dedicated ADC.

## Immediate development checklist

| Task | Status |
|---|---|
| Establish and record DWM3001-CDK/nRF52833 build and flash route | Pending |
| Confirm external ADC pin and SAADC channel | Pending |
| Record first reference/gain/sample-rate configuration | Pending |
| Capture rest/DC record | Pending |
| Capture mechanical-response record | Pending |
| Capture baseline-noise record | Pending |
| Compare capture during radio activity when available | Pending |
| Save raw records and acquisition metadata | Pending |
| Decide candidate restricted-band qualification plan | Pending |
