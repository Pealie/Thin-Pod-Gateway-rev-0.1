# Node Acquisition Characterisation Workspace

**Directory:** `firmware/node-dwm/acquisition-characterisation/`  
**Purpose:** Preserve configuration, captures and evidence for digitising the external ADXL1005 analogue signal through the node-side nRF52833 SAADC  
**Status:** Prepared for first acquisition experiments; no digital measurement record yet claimed  
**Document date:** 27 May 2026

## Purpose

This directory is the working evidence area for the first serious acquisition question in Thin-Pod:

> **Can the external ADXL1005 analogue path be captured digitally through the node-side DWM3001/nRF52833 route as a stable and useful raw vibration record?**

The work here precedes claims about UWB transport, Gateway DSP or TinyML. It establishes whether the measurement record being transported is credible.

## Scope

This workspace covers:

- ADC pin and firmware-configuration confirmation;
- raw ADC captures of the external ADXL1005 channel;
- resting/noise/mechanical-response characterisation;
- radio-interference comparison;
- sampling-rate and anti-alias investigation;
- record metadata and archive hashes; and
- the decision whether nRF52833 acquisition is qualified for a stated band or should later be replaced/supplemented by a dedicated ADC.

It does not treat the DWM3001C module’s integrated motion sensor as the main Thin-Pod measurement channel.

## Controlling document

The controlling specification is:

```text
../../../docs/node-acquisition-and-sampling-contract.md
```

The first captures must be labelled:

```text
CHARACTERISATION / UNCALIBRATED
```

unless and until a later qualification/calibration record supports a stronger description.

## Suggested evidence structure

```text
acquisition-characterisation/
├── README.md
│
├── configurations/
│   ├── ACQ-CONFIG-0001.md
│   └── ACQ-CONFIG-0001.json
│
├── captures/
│   └── <session-id>/
│       ├── session-manifest.json
│       ├── rest-window-0001.tpwn
│       ├── background-window-0001.tpwn
│       ├── tap-window-0001.tpwn
│       ├── radio-idle-window-0001.tpwn
│       ├── radio-active-window-0001.tpwn
│       └── hashes.sha256
│
├── analysis/
│   └── <session-id>/
│       ├── first-look.md
│       ├── basic-statistics.csv
│       └── spectrum-observations.md
│
└── evidence/
    └── <session-id>/
        ├── bench-notes.md
        ├── oscilloscope/
        └── screenshots/
```

The `.tpwn` extension is reserved for Thin-Pod complete raw-window records conforming to a recorded record version. Before that binary format is implemented, early temporary capture files must be clearly identified as preliminary and mapped into the final record model later.

## Acquisition configuration record

Create one acquisition-configuration record before relying on any captured data.

### Minimum configuration fields

| Field | Value to record |
|---|---|
| Configuration ID | e.g. `ACQ-CONFIG-0001` |
| Date | Test date |
| Node hardware revision | `Thin-Pod rev 0.1` |
| Node instance ID | Assign when testing begins |
| Sensor channel | External ADXL1005 path |
| Sensor supply voltage | Measured during test |
| Analogue resting voltage | Measured during test |
| Analogue filter | `8 kΩ / 680 pF`, nominal approximately 29.3 kHz corner for rev 0.1, unless changed |
| DWM/CDK identity | Record unit used |
| nRF52833 ADC input/channel | Confirm against firmware and schematic |
| SAADC reference/gain/range | Exact firmware setting |
| Acquisition time / oversampling | Exact firmware setting |
| Sampling method | Timer/event/DMA behaviour |
| Sample rate | Exact configured value |
| Sample count | Initially 2048 where using record target |
| Record/sample format | ADC code representation |
| Radio state | Inactive, active, or controlled comparison |
| Qualification state | Initially `CHARACTERISATION / UNCALIBRATED` |
| Firmware build ID | Commit/build identity |
| Known limitations | Filtering, mounting, clipping or uncertainty notes |

## Test sequence

### A. Electrical sanity

| Test ID | Action | Pass condition |
|---|---|---|
| ACQ-A001 | Confirm ADC pin/net mapping | Firmware pin corresponds to external Thin-Pod ADC node |
| ACQ-A002 | Measure sensor rail and resting analogue voltage | Values recorded; input is within configured ADC range |
| ACQ-A003 | Capture resting ADC codes | Digital mean broadly corresponds to measured analogue voltage |
| ACQ-A004 | Check for obvious clipping or stuck values | No unexplained saturation/static behaviour |

### B. First raw windows

| Test ID | Action | Pass condition |
|---|---|---|
| ACQ-B001 | Capture 2048-sample resting record | Record retained with metadata and integrity check |
| ACQ-B002 | Capture stable-background/no-motion record | Initial noise record available |
| ACQ-B003 | Capture controlled tap/mechanical excitation | Digital record shows plausible mechanical response |
| ACQ-B004 | Repeat capture | Response and background behaviour are reproducible enough to continue |

### C. Radio-interference comparison

| Test ID | Action | Pass condition |
|---|---|---|
| ACQ-C001 | Capture with UWB/radio inactive | Baseline record established |
| ACQ-C002 | Capture during or adjacent to radio activity | Difference recorded visibly, whether benign or problematic |
| ACQ-C003 | Decide capture/transmit scheduling position | Time separation adopted if radio activity contaminates acquisition |

### D. Bandwidth and ADC architecture decision

| Test ID | Action | Outcome sought |
|---|---|---|
| ACQ-D001 | Known low-frequency excitation | Validate initial restricted-band response |
| ACQ-D002 | Sweep through a candidate 0–5 kHz mode where equipment permits | Determine whether low-band qualification is credible |
| ACQ-D003 | Examine 10 kHz and higher only with controlled filtering/test arrangement | Assess moderate/high-band feasibility |
| ACQ-D004 | Compare with reference scope/ADC record where practical | Quantify loss/noise/distortion |
| ACQ-D005 | Make retain-or-revise ADC decision | State qualified band or recommend later dedicated ADC |

## First-session manifest template

```json
{
  "session_id": "TP-ACQ-2026-0001",
  "purpose": "First external ADXL1005 digital acquisition characterisation",
  "node_hardware_revision": "TP-NODE-R0.1",
  "node_instance_id": "TO_BE_ASSIGNED",
  "sensor_identity": "External ADXL1005 channel",
  "qualification_state": "CHARACTERISATION_UNCALIBRATED",
  "adc_device": "nRF52833 SAADC",
  "adc_input_channel": "TO_BE_CONFIRMED",
  "adc_configuration": {
    "reference": "TO_BE_CONFIRMED",
    "gain": "TO_BE_CONFIRMED",
    "acquisition_time": "TO_BE_CONFIRMED",
    "oversampling": "TO_BE_CONFIRMED"
  },
  "analogue_filter": {
    "series_resistor_ohms": 8000,
    "capacitor_farads": 6.8e-10,
    "nominal_corner_hz": 29257
  },
  "sample_rate_hz": "TO_BE_CONFIRMED",
  "sample_count": 2048,
  "radio_state_during_capture": "INACTIVE_FOR_BASELINE",
  "node_firmware_build": "TO_BE_RECORDED",
  "notes": ""
}
```

## Initial analysis expectations

Initial characterisation analysis should remain deliberately modest.

| Analysis | Purpose |
|---|---|
| Mean ADC code / corresponding voltage estimate | Confirm bias/rest position |
| Minimum and maximum ADC code | Detect clipping or unexpected range |
| RMS around mean | Baseline noise/energy indicator |
| Time-domain plot | Confirm mechanical response visibly |
| Simple FFT/spectrum | Reveal gross interference, spur or alias behaviour |
| Comparison between radio-idle and radio-active captures | Establish scheduling/contamination decision |

Do not present early feature values as calibrated machine-health results.

## Evidence acceptance statement

The first meaningful success for this directory is:

> A bounded digital record derived from the external ADXL1005 channel has been captured by node-side firmware, stored with explicit ADC/sample/filter metadata, shown to respond to mechanical excitation and labelled accurately as characterisation evidence.

A later qualification decision must state the measurement bandwidth and conditions supported by the evidence. Where the nRF52833 SAADC path proves inadequate for the intended bandwidth/noise goal, this directory should preserve that finding as the justification for a later dedicated-ADC node revision.
