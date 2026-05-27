# Thin-Pod: Node Acquisition and Sampling Contract

**Document identifier:** `TP-NODE-ACQ-0001`  
**Document revision:** Draft A  
**Applies to:** Thin-Pod rev 0.1 node and later Thin-Pod acquisition revisions  
**Related Gateway hardware:** Thin-Pod Gateway rev 0.1  
**Document status:** Acquisition-qualification specification prepared before digital sampling firmware implementation  
**Document date:** 27 May 2026  
**Certification boundary:** This document supports continuing system development and does not expand the submitted OSHWA scope of Thin-Pod rev 0.1.

## 1\. Purpose

This document defines the qualification gate for converting Thin-Pod’s external ADXL1005 analogue vibration signal into a credible digital raw-window record.

The Thin-Pod architecture now provides a defensible modular route:

```text
external ADXL1005 analogue vibration signal
        ↓
qualified node-side digital acquisition
        ↓
DWM3001-managed UWB transport of bounded raw windows
        ↓
Gateway-side DWM3001 communications subsystem
        ↓
STM32 NUCLEO-N657X0-Q analytic supervisor
        ↓
DSP and later TinyML evaluation
```

The unresolved question is not whether a buffer can be sent in principle. It is whether the initial node-side acquisition route can digitise the external ADXL1005 signal with adequate bandwidth, timing stability, input-range margin and noise performance for useful vibration analysis.

The central engineering rule is therefore:

> \*\*Raw measurement windows must originate from a qualified Pod acquisition chain. Transport and analysis claims follow from measurement quality; they do not substitute for it.\*\*

## 2\. Principal assessment

The nRF52833 SAADC path is suitable for proving Thin-Pod’s digital acquisition concept and may be adequate for a deliberately bandwidth-limited vibration-analysis mode.

It must not yet be treated as a clean final acquisition solution for exploiting the full approximately 20–23 kHz diagnostic bandwidth available from the ADXL1005-based analogue path.

The decisive concerns are:

* effective resolution across the ADXL1005’s large ±100 g dynamic range;
* anti-alias filtering appropriate to the intended bandwidth;
* sensor-supply and ADC-reference behaviour in a ratiometric signal chain;
* interference from local digital/radio/switching activity;
* sampling-clock and buffer integrity; and
* mechanical mounting quality at high vibration frequencies.

The acquisition work should therefore proceed in two layers:

```text
Layer 1:
    use the nRF52833 SAADC to prove digitisation, bounded raw-window creation
    and later UWB/Gateway transport.

Layer 2:
    determine experimentally whether the SAADC path qualifies for a stated
    useful measurement bandwidth, or whether a later node revision requires
    a dedicated higher-performance ADC/front end.
```

## 3\. Measurement-channel identity

The primary Thin-Pod vibration-measurement channel is the **external ADXL1005-based analogue signal path**.

The Qorvo DWM3001C module also contains an integrated motion sensor. That module sensor is not the primary measurement channel for the Thin-Pod condition-monitoring objective.

|Sensing element|Intended system role|Qualification position|
|-|-|-|
|External ADXL1005-based Thin-Pod analogue channel|Primary raw vibration-measurement channel for DSP and later TinyML evidence|Must be digitally qualified|
|DWM3001C integrated motion sensor|Possible later wake-up, orientation, movement-presence or enclosure-health sensor|Not a substitute for the external high-bandwidth vibration channel|

Any digital window described as a Thin-Pod vibration measurement must identify the external ADXL1005 channel or explicitly state that it is a test/auxiliary-sensor record.

## 4\. Confirmed component specification basis

### 4.1 nRF52833 SAADC

The Qorvo DWM3001C incorporates a Nordic nRF52833 SoC alongside the DW3110-based UWB function. Nordic’s nRF52833 Product Specification defines a Successive Approximation Analogue-to-Digital Converter (`SAADC`) suitable for timer-controlled embedded acquisition.

|nRF52833 SAADC property|Specification / design relevance|
|-|-|
|Native resolution|Configurable 8, 10 or 12 bit conversion|
|Oversampling|Supported; potentially useful for lower-bandwidth noise reduction, not a substitute for wideband resolution|
|Maximum sampling rate|200 ksample/s|
|Input channels|Up to eight configured analogue input channels|
|Sampling support|Event/task and EasyDMA-compatible operation suitable for bounded buffered acquisition|
|Reference configurations|Internal reference and supply-relative reference options are available under SAADC configuration|
|Gain/range configuration|Programmable gain permits matching the input full-scale range to the analogue signal|

The current Thin-Pod concept routes its conditioned sensor node to the CDK/nRF52833 ADC-capable interface; the exact firmware channel and SAADC configuration must be confirmed against the released schematic and implemented firmware before qualification measurements begin.

### 4.2 ADXL1005 analogue sensor path

Analog Devices specifies the ADXL1005 as a wideband, single-axis analogue accelerometer intended for vibration sensing.

|ADXL1005 property|Specification / relevance|
|-|-|
|Measurement range|±100 g|
|Output type|Analogue, ratiometric to sensor supply|
|Typical sensitivity at 5.0 V|20 mV/g|
|Zero-g output|Approximately `VDD / 2`|
|Typical noise density at 5.0 V|75 µg/√Hz over the stated measurement region|
|Noise density specified at 3.0 V|125 µg/√Hz over the stated measurement region|
|Typical −3 dB frequency response|23 kHz|
|Typical sensor resonance|42 kHz|
|Output amplifier small-signal bandwidth|70 kHz|
|Internal clock concern|Datasheet identifies approximately 200 kHz clock-related output content requiring alias/filter consideration|
|Supply range|3.0 V to 5.25 V|

### 4.3 Existing Thin-Pod rev 0.1 analogue evidence

The current Thin-Pod rev 0.1 prototype work has already recorded:

|Prototype observation|Result|
|-|-:|
|Switched sensor supply rail|Approximately 3.35 V|
|Filtered analogue output at rest|Approximately 1.76 V|
|Mechanical excitation response|Visible oscilloscope response to tapping after prototype ground-return correction|

These results establish analogue proof of life. They do not yet establish digital acquisition noise, frequency response, alias control or calibrated acceleration accuracy.

## 5\. Current Thin-Pod analogue filter position

The existing rev 0.1 signal-conditioning path records a series/filter network of:

```text
R = 8 kΩ
C = 680 pF
```

The nominal single-pole corner frequency is:

```text
f\_c = 1 / (2πRC) ≈ 29.3 kHz
```

This was adequate for bringing up and observing the analogue path. It should be treated as a **bring-up filter**, not as a final anti-alias design for a high-fidelity wideband digitiser.

Approximate attenuation from one 29.3 kHz low-pass pole is:

|Undesired region|Approximate single-pole attenuation|
|-:|-:|
|42 kHz sensor-resonance region|4.8 dB|
|200 kHz internal-clock-related region|16.8 dB|

This attenuation is not enough, by itself, to establish clean high-bandwidth digitisation. A defined measurement bandwidth requires a correspondingly designed analogue low-pass strategy and measured spectral behaviour.

## 6\. Electrical input-range compatibility

At the measured Thin-Pod switched sensor supply of approximately 3.35 V, the ratiometric ADXL1005 nominal sensitivity scales from its 5 V value as follows:

```text
20 mV/g × (3.35 V / 5.0 V) ≈ 13.4 mV/g
```

The nominal zero-g output is:

```text
3.35 V / 2 ≈ 1.675 V
```

Across the nominal ±100 g range, the expected output range is approximately:

```text
1.675 V ± (100 × 13.4 mV)
≈ 0.335 V to 3.015 V
```

The prototype resting output of approximately 1.76 V is broadly consistent with a mid-rail analogue output under real board conditions; it should not be treated as a calibrated offset measurement without controlled test conditions.

### 6.1 Candidate SAADC full-scale configurations

Candidate SAADC configurations must be checked against the Nordic Product Specification and implemented firmware. Two broad full-scale approaches are technically relevant:

|Candidate approach|Approximate input span relevance|Qualification question|
|-|-|-|
|Supply-relative reference arrangement with compatible gain|May preserve ratiometric cancellation where ADC reference tracks sensor supply|Do sensor supply and ADC reference genuinely track in the fitted CDK/carrier arrangement?|
|Internal-reference arrangement with gain selected for approximately 3.3–3.6 V full-scale|Provides a fixed full-scale acquisition range|Does supply variation create unacceptable conversion/scaling error in a ratiometric sensor path?|

Analog Devices recommends using the accelerometer supply as the digitising system’s reference where possible because the output is ratiometric. The actual Thin-Pod/DWM3001-CDK implementation must determine whether a supply-relative conversion strategy is practical and sufficiently clean.

### 6.2 Required electrical checks

|Check|Evidence required|
|-|-|
|ADC pin mapping|Schematic and firmware assignment for the external sensor signal|
|Sensor rail voltage during capture|Recorded measurement with radio idle and later with radio activity|
|ADC reference and gain settings|Firmware configuration plus calculated full-scale input span|
|Zero-g/resting code|Raw ADC capture corresponding to measured analogue rest voltage|
|Positive/negative signal margin|Known excitation or injected signal showing distance from clipping|
|Input loading|Comparison showing ADC connection does not materially disturb analogue signal|
|Overvoltage/transient safety|Review of expected maximum sensor excursion and protection needs|

## 7\. Resolution and noise budget

A 12-bit conversion spanning approximately 3.35–3.6 V has an ideal least-significant-bit size of roughly:

|Assumed ADC full-scale input span|Ideal LSB size|
|-:|-:|
|3.35 V|0.818 mV/count|
|3.60 V|0.879 mV/count|

At approximately 13.4 mV/g sensor sensitivity, one ideal ADC count corresponds to:

|Assumed ADC full-scale input span|Acceleration per ideal count|
|-:|-:|
|3.35 V|Approximately 61 mg/count|
|3.60 V|Approximately 66 mg/count|

Ideal quantisation noise, modelled as one LSB divided by `√12`, corresponds approximately to:

|Assumed ADC full-scale input span|Ideal quantisation noise expressed as acceleration|
|-:|-:|
|3.35 V|Approximately 17.6 mg RMS|
|3.60 V|Approximately 18.9 mg RMS|

For comparison, integration of the ADXL1005 quoted noise density over a 20 kHz bandwidth gives:

|ADXL1005 noise-density condition|Approximate integrated sensor noise over 20 kHz|
|-|-:|
|75 µg/√Hz typical at 5.0 V|Approximately 10.6 mg RMS|
|125 µg/√Hz specified at 3.0 V|Approximately 17.7 mg RMS|

### 7.1 Consequence

In a full-scale ±100 g, approximately 3.3 V acquisition arrangement, ideal 12-bit quantisation noise is already comparable to the ADXL1005’s broadband sensor noise at lower-voltage operation, before adding:

* SAADC conversion noise and effective-number-of-bits limitations;
* reference and supply noise;
* switching-regulator residue;
* UWB/radio-current interference;
* PCB coupling;
* analogue filter noise; and
* mounting/environmental vibration contamination.

This does not prevent useful experiments. It does mean that the nRF52833 SAADC is unlikely to exploit the full low-noise performance of the ADXL1005 across its full ±100 g dynamic range without careful characterisation or signal-range optimisation.

## 8\. Sampling rate and anti-alias position

Sampling-rate choice must be tied to the actual intended measurement bandwidth and analogue filtering.

### 8.1 Critical constraint

The ADXL1005 wideband analogue path presents more than an in-band signal:

* typical sensor response extends to approximately 23 kHz at −3 dB;
* typical resonance occurs near 42 kHz;
* output amplifier small-signal bandwidth extends to approximately 70 kHz; and
* Analog Devices identifies approximately 200 kHz internal-clock-related content requiring filtering/alias consideration.

The nRF52833 SAADC maximum stated sample rate of 200 ksample/s is sufficient for many bounded acquisition experiments, but it is not a licence to digitise the existing wide analogue path uncritically.

### 8.2 Sample-rate modes

|Acquisition mode|Nyquist frequency|Appropriate interpretation|
|-|-:|-|
|12.8 ksample/s|6.4 kHz|Low-band commissioning only, with suitably restricted analogue bandwidth|
|25.6 ksample/s|12.8 kHz|Communications/sample-buffer commissioning or restricted-band measurement; cannot preserve an approximately 20 kHz analogue channel without alias risk|
|51.2 ksample/s|25.6 kHz|Minimum-class candidate for an approximately 20 kHz filtered measurement band, but only with appropriate analogue attenuation above band and measured alias/noise behaviour|
|100 ksample/s|50 kHz|Better temporal margin for moderate/wideband characterisation but still vulnerable to higher-frequency content and spur aliasing without filtering|
|200 ksample/s|100 kHz|Maximum SAADC-class characterisation mode; still requires handling of approximately 200 kHz clock-related content and practical analogue filtering|

### 8.3 Practical interpretation of Analog Devices filtering guidance

Analog Devices’ ADXL1005 data-sheet guidance emphasises that aliased out-of-band signals cannot be removed digitally after conversion. Its examples indicate:

|Intended band / filtering case|Design implication for Thin-Pod|
|-|-|
|Lower-band operation around 0–5 kHz with appropriate simple filtering|Strong candidate for nRF52833 SAADC qualification|
|Operation around 0–10 kHz|Plausible but demanding; a defined multi-pole filter and high SAADC sampling rate are needed|
|Full or near-full approximately 20–23 kHz ADXL1005 use|Must not be assumed clean on the nRF52833 path; requires more demanding filter/ADC testing and may favour a later dedicated ADC|

The rev 0.1 29.3 kHz single-pole filter should therefore remain recorded as successful prototype signal-path evidence, not as the final anti-alias solution.

## 9\. Radio and supply interference risk

A principal advantage of transmitting bounded windows is that acquisition can be time-separated from radio activity.

Preferred first acquisition/transport sequence:

```text
UWB radio quiet or inactive
        ↓
capture one ADC raw vibration window
        ↓
freeze and validate capture metadata
        ↓
transmit the completed window over UWB
        ↓
repeat only under admission control
```

This approach should be compared experimentally with simultaneous radio activity.

|Test condition|Purpose|
|-|-|
|Sensor capture with radio inactive|Establish base combined sensor/ADC/supply noise|
|Sensor capture during radio activity|Identify UWB/digital/supply contamination|
|Sensor capture before and after radio burst|Detect settling or rail-recovery effects|
|Sensor capture under alternative supply/decoupling conditions where feasible|Identify dominant coupling source|

Where radio activity materially raises noise or produces spectral artefacts, time-separated capture/transmit operation should become part of the qualified acquisition contract.

## 10\. Mechanical qualification risk

At low frequencies, qualitative tap testing is useful evidence of signal-path function. At high frequencies, the mechanical interface becomes part of the instrument.

A credible measurement-band claim must record:

|Mechanical factor|Why it matters|
|-|-|
|Sensor attachment method|Adhesive, magnetic or loose mounting may attenuate/distort high-frequency response|
|Mounting surface and mass|Alters coupling and resonant behaviour|
|Cable/module movement|Can create artefacts unrelated to the measured machine|
|Repeatable excitation method|Required for meaningful frequency-response comparison|
|Reference measurement arrangement|Needed where claiming a useful diagnostic bandwidth|

No ADC-path qualification can, by itself, validate high-frequency condition-monitoring performance without a repeatable mechanical test arrangement.

## 11\. Qualification decision matrix

|Objective|nRF52833 SAADC position|Required evidence|
|-|-|-|
|Digital ADC sampling proof|Appropriate and recommended|Raw stable capture from known DC/rest/test input|
|Synthetic/raw-window data-path proof|Independent of ADC quality; proceed|TPHIP-1 golden record and transport tests|
|Large vibration event/tap response in digital samples|Strongly plausible|Recorded time-domain samples and correlation with analogue observation|
|Initial RMS/peak/crest-factor DSP experiments|Plausible under stated uncalibrated/characterisation conditions|Repeatable captures, known sample settings and integrity records|
|Qualified 0–5 kHz vibration mode|Strong candidate|Filter definition, ADC noise/range tests and known-frequency response|
|Qualified 0–10 kHz vibration mode|Possible but demanding|Multi-pole filtering, high sample rate, alias and SNR measurement|
|Full approximately 20–23 kHz low-noise diagnostic use|Not to be assumed with current SAADC/filter path|Persuasive measured result or later dedicated ADC/front-end design|

## 12\. Architectural fork after qualification

The modular system architecture remains valid regardless of the SAADC outcome.

```text
If the nRF52833 ADC route is sufficiently clean for a stated band:
    retain DWM3001/nRF52833 as the node-side acquisition
    and UWB communications endpoint for that qualified mode.

If the nRF52833 ADC route is inadequate for the intended bandwidth or noise floor:
    retain DWM3001 as the UWB communications subsystem,
    but introduce a dedicated precision acquisition ADC / analogue front end
    in a later Thin-Pod node revision.
```

A dedicated ADC decision would be an improvement to the measurement chain, not a rejection of the UWB/Gateway architecture.

## 13\. Qualification test programme

The acquisition qualification programme should be conducted separately from, but coordinated with, the Gateway host-interface and UWB transport tests.

### 13.1 Stage A: electrical and configuration validation

|Test ID|Test|Purpose|Required evidence|
|-|-|-|-|
|A001|Confirm ADC pin/net mapping|Connect schematic intent to firmware configuration|Pin map, source reference and firmware assignment|
|A002|Measure sensor supply and rest output|Confirm ratiometric starting point|Multimeter/scope measurements and raw ADC codes|
|A003|Record SAADC reference/gain/acquisition settings|Make conversion interpretable|Firmware configuration record|
|A004|Apply bounded known input where feasible|Confirm full-scale mapping and clipping margin|Input/output code relationship|
|A005|Check analogue loading|Ensure ADC route does not materially disturb sensor output|Comparative signal observation|

### 13.2 Stage B: noise and radio-interference validation

|Test ID|Test|Purpose|Required evidence|
|-|-|-|-|
|B001|Capture resting sensor with radio inactive|Establish baseline noise spectrum and RMS|Raw record, FFT/noise calculation and configuration|
|B002|Capture during UWB activity|Detect radio/supply contamination|Comparative raw record and spectrum|
|B003|Capture immediately following transmit event|Detect settling/recovery artefacts|Time-aligned capture results|
|B004|Repeat under stable mechanical/noise conditions|Establish repeatability|Multiple retained raw windows and statistics|

### 13.3 Stage C: frequency and alias validation

|Test ID|Test|Purpose|Required evidence|
|-|-|-|-|
|C001|Known excitation at low frequency|Confirm basic scaling and record interpretation|Input condition and captured response|
|C002|Sweep through 1 kHz and 5 kHz region|Evaluate candidate low-band qualified mode|Response amplitude/phase or comparative spectrum|
|C003|Extend to 10 kHz where test equipment permits|Assess moderate-band feasibility|Response and alias/noise record|
|C004|Extend towards 15–20 kHz only under controlled setup|Assess wideband ambition|Reference comparison and limitations|
|C005|Investigate spur/alias regions associated with high-frequency content|Determine filter/ADC revision need|Spectrum and explicit decision record|

### 13.4 Stage D: system record validation

|Test ID|Test|Purpose|Required evidence|
|-|-|-|-|
|D001|Create one sensor-derived bounded raw record|Prove acquisition record format|Binary record, header metadata and CRC|
|D002|Transport sensor record through Gateway path|Prove measurement-to-Gateway chain|Accepted raw record and provenance entry|
|D003|Compute RMS and peak from accepted record|First analysis proof|Feature result linked to raw record|
|D004|Repeat under identical condition|Determine repeatability|Comparative feature/raw-record set|

## 14\. Acquisition metadata contract

Each actual sensor-derived raw window must carry or refer to the following acquisition metadata.

|Field|Required status|
|-|-|
|Node hardware revision and instance|Required|
|Sensor identity / external ADXL1005 channel identity|Required|
|ADC device and input pin|Required|
|ADC reference/gain/full-scale configuration|Required|
|Sample rate and sample count|Required|
|Analogue filter identity or revision|Required|
|Calibration status|Required, even where the value is `UNQUALIFIED` or `CHARACTERISATION`|
|Clipping indication|Required where detectable|
|Radio state during acquisition|Required during qualification work|
|Node firmware build|Required|
|Raw record CRC / preserved artifact hash|Required at transport/archive stages|

Early samples should be labelled:

```text
CHARACTERISATION / UNCALIBRATED
```

until an explicit qualified-band or calibration record exists.

## 15\. Relationship to other Gateway documents

|Document|Relationship to this acquisition contract|
|-|-|
|`docs/data-path-flow-control-and-buffering.md`|Controls how completed windows are admitted and protected against overrun|
|`docs/dwm-to-nucleo-host-interface-protocol.md`|Controls host transfer of completed records into NUCLEO memory|
|`docs/window-throughput-and-airtime-budget.md`|Quantifies time/byte cost once acquisition record parameters are fixed|
|`docs/data-path-fault-injection-and-recovery-tests.md`|Proves invalid/incomplete/blocked records remain visible|
|`docs/measurement-record-and-provenance-model.md`|Preserves traceability from acquisition configuration into analysis|
|`docs/system-interface-control-document.md`|Defines subsystem roles and architecture boundary|

## 16\. Design judgement for rev 0.1 and later revisions

The correct position for the current hardware is:

|Hardware stage|Acquisition role|
|-|-|
|Thin-Pod rev 0.1|Analogue sensor-path proof, SAADC characterisation and first transported raw-window experiments|
|Thin-Pod Gateway rev 0.1|Receive, retain and analyse qualification records without overclaiming bandwidth|
|Later Thin-Pod node revision if SAADC is adequate for stated band|Refine filter, power and packaging around demonstrated restricted-band capability|
|Later Thin-Pod node revision if wideband fidelity is required|Add dedicated higher-performance ADC/front end while retaining modular UWB transport|
|Later Gateway revision|Continue communications/analysis modularity; ADC decision is primarily node-side|

## 17\. References

* Analog Devices, **ADXL1005 Low Noise, Wide Bandwidth, MEMS Accelerometer** data sheet and product information:  
[https://www.analog.com/en/products/adxl1005.html](https://www.analog.com/en/products/adxl1005.html)  
[https://www.analog.com/media/en/technical-documentation/data-sheets/adxl1005.pdf](https://www.analog.com/media/en/technical-documentation/data-sheets/adxl1005.pdf)
* Qorvo, **DWM3001C UWB Module with BLE SoC and Motion Sensor** product information:  
[https://www.qorvo.com/products/p/DWM3001C](https://www.qorvo.com/products/p/DWM3001C)
* Qorvo, **DWM3001CDK Development Kit** product information:  
[https://www.qorvo.com/products/p/DWM3001CDK](https://www.qorvo.com/products/p/DWM3001CDK)
* Nordic Semiconductor, **nRF52833 Product Specification** and product information:  
[https://docs.nordicsemi.com/bundle/ps\_nrf52833/page/keyfeatures\_html5.html](https://docs.nordicsemi.com/bundle/ps_nrf52833/page/keyfeatures_html5.html)  
[https://www.nordicsemi.com/Products/nRF52833](https://www.nordicsemi.com/Products/nRF52833)
* Thin-Pod rev 0.1, public release documentation and prototype bring-up record.
* Thin-Pod Gateway rev 0.1 supporting architecture documents.

## 18\. Engineering decision

The nRF52833 SAADC is to be used as the first node-side digital acquisition path because it is integrated with the selected DWM3001 communications subsystem and is adequate to test real bounded vibration windows, transport, Gateway ingestion and initial DSP.

Its use does not pre-approve a full-bandwidth production measurement chain. The first acquisition outcome sought is a qualified, explicitly stated measurement band. A successful low- or moderate-band result would establish a serious experimental Thin-Pod instrument. Where later work requires most of the ADXL1005’s full high-bandwidth, low-noise capability, a dedicated ADC/front-end path remains an appropriate later node-revision decision.

