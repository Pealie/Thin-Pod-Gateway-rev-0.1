# Thin-Pod Gateway rev 0.1: Next Architecture Additions and Acquisition Qualification Gate

**Document identifier:** `TP-GW-ARCH-0002`  
**Document revision:** Draft A  
**Product context:** Thin-Pod rev 0.1 sensor node and Thin-Pod Gateway rev 0.1  
**Document status:** Planning and architecture-development record  
**Document date:** 27 May 2026  
**Certification boundary:** This document supports future Gateway work and does not expand the submitted OSHWA scope of Thin-Pod rev 0.1.

## 1\. Purpose

The Gateway data-path and flow-control architecture now addresses how accepted vibration-window records should be transferred, buffered and protected against silent loss. The next major engineering question occurs earlier in the chain:

> \*\*Can the Thin-Pod node create a diagnostically credible digital vibration window from the external ADXL1005 analogue signal before that record is transported to the Gateway?\*\*

A successful UWB transport path is not sufficient on its own. Raw records delivered perfectly to Gateway memory remain of limited value if the initial sampling process is clipped, aliased, excessively noisy, poorly timed or inadequately described.

The next phase of the project should therefore qualify acquisition before placing emphasis on larger communications throughput, DSP feature claims or TinyML experiments.

## 2\. Principal conclusion

The system architecture remains sound, but its wording should be strengthened:

> \*\*Raw measurement windows from a qualified Pod acquisition chain; UWB transport handled by the DWM3001 subsystem; analysis performed on the Gateway.\*\*

This replaces any premature assumption that the nRF52833 ADC route is already sufficient for the full intended vibration-measurement use case. The node-side DWM3001 subsystem remains a valid candidate for acquisition and transport; its acquisition quality must now be tested rather than presumed.

## 3\. Why acquisition qualification is now the critical missing layer

The intended measurement path is:

```text
External ADXL1005 analogue vibration output
        ↓
Thin-Pod analogue conditioning / ADC node
        ↓
node-side DWM3001-CDK / DWM3001C nRF52833 acquisition firmware
        ↓
framed raw vibration window
        ↓
UWB transport
        ↓
Gateway-side DWM3001 subsystem
        ↓
NUCLEO-owned raw-window memory
        ↓
DSP and later TinyML
```

Existing Thin-Pod prototype evidence has already established analogue proof of life after the pre-release ground-return correction: the switched sensor supply was measured at approximately 3.35 V, the filtered analogue output was approximately 1.76 V at rest, and mechanical tapping produced a visible oscilloscope response.

What remains unestablished is whether the analogue signal can be **digitised with measurement quality appropriate to vibration analysis**.

## 4\. Relevant component facts

Analog Devices specifies the ADXL1005 as a single-axis analogue-output accelerometer with a ±100 g full-scale range, typical linear frequency response from DC to 23 kHz at the −3 dB point, and typical noise density of 75 µg/√Hz. Analog Devices also states that the EVAL-ADXL1005Z evaluation board provides a simple RC low-pass output filter with an approximately 20 kHz −3 dB bandwidth.

Qorvo specifies that the DWM3001C includes a Nordic nRF52833 BLE SoC and a motion sensor, and supports UWB data rates of 850 kbps and 6.8 Mbps. Nordic specifies the nRF52833 as including 128 KB RAM and a 12-bit ADC capability.

These specifications support a serious acquisition experiment. They do not by themselves establish ADC noise performance, analogue input-range margin, sampling timing quality or anti-alias suitability in the Thin-Pod assembly.

## 5\. External measurement channel versus internal module sensor

The system must distinguish between the intended measurement channel and the motion sensor integrated in the Qorvo module.

|Sensing element|Proposed role in Thin-Pod architecture|Measurement status|
|-|-|-|
|External ADXL1005-based Thin-Pod analogue channel|Primary vibration-measurement path for raw diagnostic windows|Must be digitally qualified|
|DWM3001C integrated motion sensor|Possible later wake-up, motion-presence, orientation or enclosure-health function|Not a substitute for the external high-bandwidth vibration channel|

The external ADXL1005 is the sensor associated with the project’s condition-monitoring and high-bandwidth vibration-analysis ambition. Any internal low-power motion sensor in the communications module should remain auxiliary unless separately characterised for a defined purpose.

## 6\. New document required: node acquisition and sampling contract

Add the following file to the wider project or Gateway documentation set:

```text
docs/node-acquisition-and-sampling-contract.md
```

Its governing question should be:

> Can the node-side DWM3001/nRF52833 acquisition route digitise the external ADXL1005 analogue signal as a repeatable, unclipped and diagnostically useful digital vibration window?

The document should specify and then record measurements in four areas.

|Qualification area|Required definition or measurement|
|-|-|
|Measurement source|Confirm external ADXL1005 path as the raw vibration channel; identify exact input net and node-side ADC pin/interface|
|ADC electrical compatibility|Measure rest bias, maximum expected excursion, supply/reference relation, configured ADC gain/range, clipping margin and any required protection or scaling|
|Sampling contract|Define initial sample rates, sample count, timer/PPI/DMA or equivalent sampling method, buffer ownership, missing-sample detection and metadata|
|Signal-quality qualification|Measure shorted/quiet noise floor where feasible, resting-sensor noise, known-frequency response, clipping behaviour, mechanical-excitation response and later comparison with a reference capture|

## 7\. ADC electrical-compatibility questions

Before treating the node as a digital vibration instrument, the following must be established.

|Question|Evidence required|
|-|-|
|Which nRF52833 ADC input receives the Thin-Pod conditioned signal?|Verified schematic/pin-map entry and firmware pin assignment|
|What is the sensor output bias at rest?|Measured value recorded under known supply conditions|
|What ADC input range and gain are configured?|Firmware configuration and calculated full-scale limits|
|Does the expected ADXL1005 excursion remain inside the ADC range?|Calculation followed by measured mechanical stimulus tests|
|Does the input require attenuation, buffering or clamp protection?|Review against maximum expected signal and measured clipping behaviour|
|Is the ADC referenced in a way compatible with the sensor’s ratiometric output?|Supply/reference stability and scaling analysis|
|Is there any unacceptable loading of the analogue filter/output node?|Static and dynamic measurement comparison with and without ADC attachment where practical|

## 8\. Sampling-rate and anti-alias qualification

The sampling-rate decision should not be conflated with the transport commissioning rate.

For communications and buffer-transfer testing, synthetic records may use any convenient nominal metadata. For actual ADXL1005 measurement, the sample-rate and analogue bandwidth must be mutually credible.

|Measurement condition|Consequence|
|-|-|
|EVAL-ADXL1005Z output filter has approximately 20 kHz −3 dB bandwidth|A 25.6 ksample/s acquisition mode cannot preserve the full analogue bandwidth without aliasing risk|
|ADXL1005 itself has typical −3 dB response to 23 kHz|A high-bandwidth measurement mode requires a sample rate above twice the intended usable band, with practical anti-alias margin|
|51.2 ksample/s candidate mode|Suitable as a minimum-class experimental candidate for a roughly 20 kHz filtered channel, but still requires alias/noise/timing evaluation|
|Lower sample-rate mode|Acceptable only if the analogue filter bandwidth is reduced correspondingly or the measurement claim is intentionally narrower|

A prudent staged position is:

```text
Transport commissioning:
    synthetic windows first; sample-rate metadata is a protocol test field.

Acquisition commissioning:
    begin with deliberately bandwidth-limited tests that avoid ambiguous aliasing.

High-bandwidth measurement qualification:
    investigate a 51.2 ksample/s-class or higher acquisition mode,
    with measured noise, timing and anti-alias behaviour.
```

The final node architecture may therefore need either a revised analogue filter target or a more capable/dedicated acquisition route, depending on measured results.

## 9\. Necessary architectural fork

The acquisition qualification gate should lead to an explicit engineering decision.

```text
If the nRF52833 ADC acquisition route is sufficiently clean and stable:
    retain the DWM3001 subsystem as the node-side acquisition
    and UWB communications endpoint.

If the nRF52833 ADC acquisition route is not sufficiently clean or
does not preserve the required vibration bandwidth:
    retain the DWM3001 subsystem as the UWB communications endpoint,
    but move precision vibration acquisition to a dedicated ADC /
    analogue-front-end path in a later Thin-Pod hardware revision.
```

Failure of the nRF52833 ADC route would **not** invalidate the modular UWB/Gateway architecture. It would demonstrate that the high-bandwidth sensor path merits a dedicated acquisition subsystem. That would be a technically serious outcome rather than a project failure.

## 10\. Second addition: DWM-to-NUCLEO host-interface protocol

The Gateway already has a documented data-path flow-control concept. The next firmware-enabling document should be:

```text
docs/dwm-to-nucleo-host-interface-protocol.md
```

Its function is to convert the Gateway-side DWM-to-NUCLEO route from an architectural intention into a byte-level implementation contract.

|Protocol area|Required content|
|-|-|
|Electrical role|NUCLEO as SPI host/master; Gateway DWM firmware as peripheral/slave; `DWM\_IRQ` as level-signalled notification|
|Commissioning rate|Conservative initial SPI rate, increased only after repeated integrity-tested transfer|
|Command set|`GET\_CAPABILITIES`, `GET\_STATUS`, `SET\_CREDITS`, `READ\_DESCRIPTOR`, `READ\_PAYLOAD`, `ACK\_WINDOW`, `REJECT\_WINDOW`, `GET\_COUNTERS`, `RESET\_INTERFACE`|
|Binary representation|Opcode values, byte order, field offsets, lengths, alignment and CRC coverage|
|Golden record|Fixed synthetic 4160-byte test record with known payload and expected integrity result|
|Reset/error behaviour|Defined recovery after DWM reset, NUCLEO reset, withheld acknowledgement, corrupted record or host timeout|
|Versioning|Protocol version independent of hardware revision where appropriate|

Without this file the carrier-board route is plausible but underspecified. With it, firmware implementation becomes an interoperability exercise with explicit pass/fail criteria.

## 11\. Third addition: throughput and airtime budget

Add:

```text
docs/window-throughput-and-airtime-budget.md
```

The purpose of this document is not to predict performance from headline PHY rates, but to measure the actual relationship between acquisition window size, UWB transfer, host transfer and analytic workload.

|Quantity|Initial design value|Measured value|
|-|-:|-:|
|Samples per raw window|2048|Pending|
|Raw payload bytes at 16-bit single-axis format|4096|Pending confirmation|
|Initial complete record bytes including fixed header|4160|Pending confirmation|
|Communications synthetic-test metadata rate|Defined for protocol tests only|Pending|
|Initial actual-acquisition sample rate|To be qualified|Pending|
|High-bandwidth acquisition candidate|51.2 ksample/s-class or higher, subject to qualification|Pending|
|Gateway DWM-to-NUCLEO host-transfer latency|Not yet claimed|Pending|
|UWB window-transfer latency|Not yet claimed|Pending|
|Sustainable admitted window rate for one node|Not yet claimed|Pending|
|Sustainable admitted window rate for multiple nodes|Not yet claimed|Pending|
|Record loss, retry and busy rates|Not yet claimed|Pending|

Qorvo’s documented 850 kbps and 6.8 Mbps UWB data rates establish that bounded-window transport is a credible experiment. Actual usable throughput depends on fragmentation, firmware scheduling, framing, integrity/security overhead, acknowledgements, retransmissions, UWB configuration and node scheduling; it must be measured.

This quantitative record would support the project’s longer research question:

> How much fault-relevant vibration information is retained per joule consumed and per byte transmitted as window length, sampling rate, transport configuration and analysis strategy vary?

## 12\. Fourth addition: fault-injection and recovery verification

Add:

```text
docs/data-path-fault-injection-and-recovery-tests.md
```

The flow-control and buffering policy must eventually be proven by controlled failure, not merely described.

|Injected condition|Expected behaviour|
|-|-|
|NUCLEO withholds `ACK\_WINDOW`|Gateway DWM retains completed record and does not overwrite it|
|NUCLEO raw queue reaches high-water level|New measurement credits withheld; event counter incremented|
|Synthetic record contains deliberately incorrect CRC|Record rejected and never submitted to DSP|
|One UWB fragment omitted|Window marked incomplete and not exposed as valid raw data|
|Fragment or sequence duplicated|Duplicate handled deterministically and recorded|
|DWM reset occurs during host transfer|Interface returns through explicit recovery/error state|
|NUCLEO becomes unavailable|Gateway DWM stops granting new measurement windows|
|XIAO reporting task is delayed or held busy|Measurement ingress continues or is prioritised; backhaul does not block raw receipt|
|Node attempts transmission without credit|Protocol violation or busy rejection recorded|

This is the proof layer for the project’s measurement-integrity position: missing or invalid data must become visible evidence, not silent absence.

## 13\. Fifth addition: measurement record and provenance model

Add:

```text
docs/measurement-record-and-provenance-model.md
```

Each retained vibration window should eventually be traceable through the hardware, firmware, transport and analysis chain.

|Provenance element|Purpose|
|-|-|
|Node hardware revision|Identifies physical measurement platform|
|Node firmware build|Identifies acquisition and transmission behaviour|
|Sensor identity|Identifies the actual measurement source|
|ADC configuration|Defines gain, range, reference and sample conversion assumptions|
|Sample rate and sample count|Preserves temporal/frequency meaning|
|Analogue filter configuration|Defines bandwidth and anti-alias context|
|Calibration state/version|Distinguishes characterised from uncharacterised measurement|
|Window sequence and timing|Preserves ordering and acquisition context|
|UWB transport integrity status|Records successful validation, retries, drops or failures|
|Gateway hardware revision|Identifies receiving carrier implementation|
|Gateway DWM firmware build|Identifies receive/reassembly/host-interface behaviour|
|NUCLEO firmware and DSP build|Identifies processing path|
|DSP feature-set version|Makes derived values reproducible|
|TinyML model version, where later used|Makes inference provenance explicit|

The measurement record should retain enough information to answer:

```text
What was sensed?
How was it sampled?
How was it transported?
What transformations were applied?
Which version of the system made the interpretation?
```

## 14\. Recommended implementation order

|Priority|Document or activity|Reason|
|-:|-|-|
|1|`node-acquisition-and-sampling-contract.md`|Tests the largest unverified assumption: creation of a useful digital vibration window|
|2|`dwm-to-nucleo-host-interface-protocol.md`|Converts the Gateway copper route into an implementable firmware contract|
|3|`data-path-fault-injection-and-recovery-tests.md`|Demonstrates that overrun and integrity failure cannot become silent data corruption|
|4|`window-throughput-and-airtime-budget.md`|Establishes quantitative scaling and later research credibility|
|5|`measurement-record-and-provenance-model.md`|Completes the traceability chain needed for DSP/TinyML evidence|

## 15\. Immediate recommendation

The next document to compose should be:

```text
docs/node-acquisition-and-sampling-contract.md
```

It should set an acquisition gate before any later claim of transported measurement windows:

> Before claiming that Thin-Pod transports vibration windows, establish that the external ADXL1005 signal can be digitised at the node with adequate bandwidth, input-range margin, timing stability, anti-alias treatment and noise performance. If the DWM3001/nRF52833 ADC path proves insufficient, preserve the modular UWB/Gateway architecture and introduce a dedicated acquisition ADC in a later node revision.

This is the most important addition because it treats Thin-Pod as a measurement instrument rather than only a communications demonstrator.

## 16\. References

* Analog Devices, **ADXL1005: Low Noise, Wide Bandwidth, MEMS Accelerometer** product information and evaluation-board description: [https://www.analog.com/en/products/adxl1005.html](https://www.analog.com/en/products/adxl1005.html)
* Qorvo, **DWM3001C UWB Module with BLE SoC and Motion Sensor** product information: [https://www.qorvo.com/products/p/DWM3001C](https://www.qorvo.com/products/p/DWM3001C)
* Qorvo, **DWM3001CDK Development Kit** product information: [https://www.qorvo.com/products/p/DWM3001CDK](https://www.qorvo.com/products/p/DWM3001CDK)
* Nordic Semiconductor, **nRF52833 Advanced Bluetooth Multiprotocol SoC** product information: [https://www.nordicsemi.com/Products/nRF52833](https://www.nordicsemi.com/Products/nRF52833)
* Thin-Pod Gateway rev 0.1, `docs/data-path-flow-control-and-buffering.md`.
* Thin-Pod Gateway rev 0.1, `docs/gateway-bring-up-and-verification-protocol.md`.
* Thin-Pod Gateway rev 0.1, `docs/system-interface-control-document.md`.

## 17\. Closing position

Thin-Pod now has a plausible and disciplined modular route from analogue vibration sensing to Gateway-side DSP and later TinyML. The remaining risk is not primarily whether a buffer can be sent: it is whether the source buffer faithfully represents a useful vibration measurement.

The next phase should therefore qualify the acquisition path, specify the host protocol, test failure behaviour, measure throughput and preserve provenance. That sequence would turn a sound modular architecture into an instrument whose later analytic claims can be defended.

