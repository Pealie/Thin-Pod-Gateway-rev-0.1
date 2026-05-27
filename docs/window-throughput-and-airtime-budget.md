# Thin-Pod Gateway rev 0.1: Window Throughput and Airtime Budget

**Document identifier:** `TP-GW-BUD-0001`  
**Document revision:** Draft A  
**Product context:** Thin-Pod rev 0.1 node and Thin-Pod Gateway rev 0.1  
**Document status:** Calculated commissioning budget and measurement template; no end-to-end throughput claim yet made  
**Document date:** 27 May 2026  
**Related protocol:** `TPHIP-1` defined in `docs/dwm-to-nucleo-host-interface-protocol.md`

## 1. Purpose

This document establishes a quantitative budget for transporting bounded raw vibration windows from the Thin-Pod node to Gateway-host memory.

The budget separates four distinct quantities:

1. analogue acquisition time at the node;
2. record size and host-interface transfer cost;
3. UWB application-level payload/fragment cost; and
4. measured end-to-end behaviour, which remains to be established experimentally.

The purpose is not to imply performance from headline radio rates. It is to define what must be measured before the architecture can support claims about usable window rates, multiple-node scaling, energy cost or DSP/TinyML responsiveness.

## 2. Source facts and design assumptions

### 2.1 Documented component facts

| Fact | Source relevance |
|---|---|
| ADXL1005 is a single-axis analogue-output accelerometer with ±100 g full-scale range and typical linear frequency response to 23 kHz at the −3 dB point | Defines the eventual high-bandwidth measurement ambition |
| EVAL-ADXL1005Z includes an approximately 20 kHz −3 dB RC low-pass output filter | Sets an anti-alias/sampling qualification constraint when the evaluation-board path is used |
| DWM3001C integrates a DW3110-based UWB function and Nordic nRF52833 BLE SoC; Qorvo states UWB data rates of 850 kbps and 6.8 Mbps | Establishes transport-rate cases worth measuring, not application throughput |
| nRF52833 provides 128 KB RAM, ADC capability and SPI/HS-SPI interfaces | Supports node/Gateway DWM experimentation while keeping local buffering modest |
| STM32N657X0 provides 4.2 MB contiguous SRAM and six SPI peripherals | Supports a deeper NUCLEO raw-window queue and later analysis work |

### 2.2 Provisional commissioning assumptions

| Design parameter | Initial value | Status |
|---|---:|---|
| Channels per measurement window | 1 | Project design basis |
| Sample format | Signed 16-bit little-endian | TPHIP-1 decision |
| Samples per raw window | 2048 | TPHIP-1 commissioning decision |
| Raw payload bytes | 4096 | Calculated |
| Fixed record header bytes | 64 | TPHIP-1 decision |
| Complete record bytes | 4160 | Calculated |
| Gateway DWM complete-record slots | 2 | Initial buffer policy |
| NUCLEO raw-record slots | 32 | Initial buffer policy |
| First SPI payload chunk size | 256 bytes | Commissioning decision |
| First SPI frequency | 1 MHz, then 2 MHz after basic proof | To be tested |
| Commissioning credit mode | One admitted window at a time | Initial safety policy |
| UWB application fragment envelope overhead used for scenarios | 24 bytes per fragment | Modelling assumption only; not yet final protocol |
| Radio/MAC/security/ACK/retry overhead | Excluded from calculated lower-bound tables | Must be measured/defined later |

## 3. Raw window size

The initial complete record defined by TPHIP-1 is:

```text
64-byte record header, including record CRC field
+ 2048 signed 16-bit samples = 4096-byte raw payload
= 4160 bytes per complete window record
```

| Quantity | Calculation | Result |
|---|---:|---:|
| Bytes per sample | 16 bits / 8 | 2 bytes |
| Raw payload | 2048 × 2 | 4096 bytes |
| Header and record metadata | Fixed | 64 bytes |
| Complete record | 4096 + 64 | 4160 bytes |
| Complete record bits | 4160 × 8 | 33,280 bits |

## 4. Acquisition-time budget

A 2048-sample window spans the following acquisition durations:

| Sample rate | Nyquist frequency | 2048-sample window duration | Continuous raw payload generation rate |
|---:|---:|---:|---:|
| 12.8 ksample/s | 6.4 kHz | 160 ms | 204.8 kbit/s |
| 25.6 ksample/s | 12.8 kHz | 80 ms | 409.6 kbit/s |
| 51.2 ksample/s | 25.6 kHz | 40 ms | 819.2 kbit/s |
| 102.4 ksample/s | 51.2 kHz | 20 ms | 1.6384 Mbit/s |

### Interpretation

A 25.6 ksample/s test mode is useful for early digital-path commissioning or intentionally lower-bandwidth experiments, but it cannot preserve an analogue channel that remains approximately 20 kHz wide without a correspondingly narrower anti-alias filter.

A 51.2 ksample/s-class mode is a minimum plausible experimental candidate for the EVAL-ADXL1005Z’s approximately 20 kHz output filter, because it places Nyquist at 25.6 kHz. It still requires measured anti-alias, noise and timing qualification before being treated as a reliable vibration-instrument operating mode.

## 5. NUCLEO raw-memory budget

Using the initial 4160-byte record size:

| Raw-record slots | Memory consumed | Continuous-equivalent history at 25.6 ksample/s | Continuous-equivalent history at 51.2 ksample/s |
|---:|---:|---:|---:|
| 8 | 33,280 bytes | 0.64 s | 0.32 s |
| 16 | 66,560 bytes | 1.28 s | 0.64 s |
| 32 | 133,120 bytes | 2.56 s | 1.28 s |
| 64 | 266,240 bytes | 5.12 s | 2.56 s |

The commissioning choice of 32 records consumes 133,120 bytes, leaving substantial SRAM headroom on the STM32N657X0 for firmware state, DSP workspaces, future model buffers and logging. The actual final allocation should be based on link timing and analytic workload measurement, not simply available memory.

## 6. Gateway DWM local-memory budget

The DWM-side nRF52833 has far less RAM than the NUCLEO and must also support communications firmware and protocol state. Local queueing should remain shallow until actual memory use is known.

| Complete-record slots | Record-buffer bytes only | Intended use |
|---:|---:|---|
| 1 | 4160 bytes | Minimal stop-and-wait; no overlap between reception and host hand-off |
| 2 | 8320 bytes | Recommended first implementation: one receive/reassembly slot and one host-ready slot |
| 4 | 16,640 bytes | Later optimisation only after measured firmware-memory headroom |

The initial architectural choice is two complete-record slots. The DWM subsystem is a communications terminator and short queue, not the long-term raw data store.

## 7. DWM-to-NUCLEO SPI host-transfer budget

### 7.1 Host protocol transfer accounting

TPHIP-1 uses:

- a 16-byte host request frame;
- a 24-byte response header;
- a 32-byte nominal status payload;
- a 64-byte descriptor read;
- payload reads in chunks; and
- an acknowledgement transaction after acceptance.

For one 4160-byte window, a commissioning exchange consists of:

```text
GET_STATUS request + response
READ_DESCRIPTOR request + 64-byte response payload
READ_PAYLOAD request/response transactions for 4096 payload bytes
ACK_WINDOW request + empty response
```

### 7.2 SPI transfer cost by payload chunk size

| Payload chunk size | Payload transactions required | Approximate clocked bytes per accepted window | Protocol overhead over 4160-byte record |
|---:|---:|---:|---:|
| 128 bytes | 32 | 5592 bytes | 34.4% |
| 256 bytes | 16 | 4952 bytes | 19.0% |
| 512 bytes | 8 | 4632 bytes | 11.3% |
| 1024 bytes | 4 | 4472 bytes | 7.5% |
| 4096 bytes | 1 | 4352 bytes | 4.6% |

The first implementation should use 256-byte payload chunks because they simplify commissioning and DWM-side response buffering. Larger chunks should be considered after the protocol works reliably and DWM firmware memory behaviour is known.

### 7.3 Ideal SPI transfer times for 256-byte chunk policy

The following figures are arithmetic lower bounds for the 4952 SPI-clocked bytes in the initial 256-byte-chunk exchange. They exclude firmware latency, IRQ response, response preparation, task scheduling and retries.

| SPI clock | Ideal host-transfer time per accepted record |
|---:|---:|
| 1 MHz | 39.616 ms |
| 2 MHz | 19.808 ms |
| 4 MHz | 9.904 ms |
| 8 MHz | 4.952 ms |

### Interpretation

At a conservative 1 MHz commissioning SPI rate, a complete host hand-off is of the same order as a 2048-sample, 51.2 ksample/s acquisition window. That is acceptable for stop-and-wait proof, not for concluding sustained performance.

At 2–4 MHz, host hand-off is unlikely to be the principal limiting factor for selected-window operation, provided the implemented SPI-peripheral firmware and assembled Gateway route operate reliably. This must be proven with CRC-checked synthetic records.

## 8. UWB application-level fragmentation scenarios

The exact UWB fragment payload must be chosen after the Qorvo firmware environment, packet format, PHY configuration, integrity/security choices and retransmission policy are defined. The table below is therefore a **scenario calculation**, not a radio specification.

For illustration, assume each transport fragment carries a 24-byte Thin-Pod application envelope in addition to record data.

| Available record bytes per fragment | Fragments needed for 4160-byte record | Application bytes sent including assumed 24-byte envelope per fragment | Application-envelope overhead |
|---:|---:|---:|---:|
| 128 | 33 | 4952 bytes | 19.0% |
| 256 | 17 | 4568 bytes | 9.8% |
| 512 | 9 | 4376 bytes | 5.2% |
| 1024 | 5 | 4280 bytes | 2.9% |

These values exclude lower-layer UWB preamble/PHY/MAC framing, FiRa/implementation details, security/STS treatment, acknowledgements, inter-frame intervals, retransmissions and firmware turnaround.

## 9. PHY-rate lower-bound scenarios

Qorvo states UWB data rates of 850 kbps and 6.8 Mbps for the DWM3001C. Applying those rates only to the application byte counts above produces ideal lower bounds, not achieved throughput.

| Record-fragment payload scenario | Application bytes | Ideal duration at 850 kbps | Ideal duration at 6.8 Mbps |
|---|---:|---:|---:|
| 128-byte record fragments | 4952 | 46.61 ms | 5.83 ms |
| 256-byte record fragments | 4568 | 42.99 ms | 5.37 ms |
| 512-byte record fragments | 4376 | 41.19 ms | 5.15 ms |
| 1024-byte record fragments | 4280 | 40.28 ms | 5.04 ms |

### Architectural consequence

For a 2048-sample record acquired at 51.2 ksample/s, the capture duration is 40 ms. Even the application-level-only lower bounds at the 850 kbps PHY are approximately 40–47 ms before any lower-layer overhead, acknowledgement or retransmission cost. Therefore, indefinite raw transfer at that sampling mode should not be expected to work at 850 kbps.

The 6.8 Mbps mode provides much more credible margin for selected bounded windows, but actual admitted-window performance remains to be measured. The correct product position remains:

```text
bounded, scheduled or triggered raw windows;
not unqualified continuous high-bandwidth streaming.
```

## 10. First single-node operational budget

### 10.1 Stop-and-wait commissioning mode

The first end-to-end system should permit only one admitted window at a time:

```text
Grant → acquire/generate → transmit → reassemble/validate
→ host-transfer → host-validate → ACK → next grant
```

The maximum commissioning window rate is therefore determined by the sum of:

| Latency component | Status |
|---|---|
| Node record creation/acquisition | To be measured |
| UWB transfer and validation | To be measured |
| Gateway DWM queue/IRQ preparation | To be measured |
| DWM-to-NUCLEO host transfer | Calculated lower bounds available; to be measured |
| NUCLEO CRC and ACK latency | To be measured |
| Scheduling guard time | To be selected after measurement |

No sustained rate should be asserted before this latency chain is measured with record counters and CRC validation.

### 10.2 Measured-results table

| Operating mode | Record size | Sample rate metadata | UWB settings | SPI setting | Windows attempted | Windows accepted | CRC failures | Busy/withheld admissions | Mean end-to-end latency | Maximum latency |
|---|---:|---:|---|---|---:|---:|---:|---:|---:|---:|
| Synthetic DWM-to-NUCLEO only | 4160 | 25.6 ksample/s test value | N/A |  |  |  |  |  |  |  |
| Known UWB test window | 4160 | 25.6 ksample/s test value |  |  |  |  |  |  |  |  |
| Actual vibration window, low-band qualification | 4160 |  |  |  |  |  |  |  |  |  |
| Actual vibration window, high-band candidate | 4160 | 51.2 ksample/s candidate |  |  |  |  |  |  |  |  |

## 11. Multi-node scheduling budget

Multiple Pods should be treated as scheduled producers, not independent uncontrolled broadcasters. The Gateway should grant window opportunities according to available DWM and NUCLEO preservation capacity.

### 11.1 Round-robin first model

```text
Gateway grants Node A one window
    → record reaches NUCLEO ownership
Gateway grants Node B one window
    → record reaches NUCLEO ownership
...
```

| Node count | Windows per complete round | Per-node observation interval if one accepted window takes `T` ms end-to-end |
|---:|---:|---:|
| 1 | 1 | `T` |
| 2 | 2 | `2T` |
| 4 | 4 | `4T` |
| 8 | 8 | `8T` |

The first measured value needed for multi-node planning is therefore `T`, the record-admission-to-NUCLEO-ownership latency under chosen operating conditions.

### 11.2 Later priority policy

A future scheduler may distinguish:

| Window class | Treatment |
|---|---|
| Routine scheduled measurement | Normal round-robin admission |
| Node health/status record | Low bandwidth, regular admission |
| Event-triggered candidate record | Priority request, but still subject to explicit capacity and loss accounting |
| Calibration/test record | Operator-authorised or maintenance-mode admission |

No priority policy should silently discard raw records already accepted into Gateway ownership.

## 12. Energy and information-retention research budget

A later measured evaluation should combine:

| Metric | Meaning |
|---|---|
| Energy per acquired window | Node acquisition and local processing cost |
| Energy per transmitted accepted window | Radio and protocol cost for a successfully delivered record |
| Bytes transmitted per accepted raw record | Communication efficiency |
| Record acceptance / failure / retry rate | Reliability cost |
| DSP feature repeatability | Whether received windows retain analytic information |
| Fault-relevant information retained per byte | Research-oriented utility metric |
| Fault-relevant information retained per joule | Research-oriented efficiency metric |

This is not a rev 0.1 certification claim. It is a credible later research direction once acquisition and transport are verified.

## 13. Required measurement instrumentation

| Measurement target | Minimum evidence |
|---|---|
| SPI transaction correctness | Golden-record byte/CRC match; logic-analyser trace where possible |
| Host transfer latency | Firmware timestamps around descriptor/payload/ACK sequence |
| UWB record latency | Node transmit timestamp and Gateway complete-record/host-ownership timestamps |
| Queue pressure | DWM and NUCLEO occupancy/high-water counters |
| Retry/loss behaviour | Sequence, CRC, timeout and busy counters |
| Energy per window | Current/energy measurement instrumentation in a later controlled experiment |
| DSP retention | Stored raw record plus versioned DSP output |

## 14. Acceptance thresholds for first proofs

| Milestone | Acceptance criterion |
|---|---|
| Wired host-interface proof | 100 consecutive golden 4160-byte synthetic records read into NUCLEO memory with correct CRC and no unexplained sequence/status error |
| Backpressure proof | Withheld ACK or NUCLEO high-water condition prevents further admitted windows without overwriting retained valid records |
| UWB test proof | Known framed record received over UWB and delivered to NUCLEO with matching record identity and CRC |
| First vibration transport proof | Actual sensor-derived record delivered and retained with sample metadata and no unreported transport error |
| DSP proof | Derived feature result references the retained raw record, protocol/build versions and integrity status |

## 15. Open measurements before any performance claim

The following must remain listed as pending until actual measurements exist:

- usable record payload per UWB fragment;
- full radio/MAC/security/acknowledgement overhead;
- retransmission policy and measured loss rate;
- end-to-end latency under chosen PHY/settings;
- nRF52833 RAM headroom with actual DWM firmware;
- maximum stable SPI host-interface rate on assembled Gateway rev 0.1;
- actual node ADC sample-rate/noise/anti-alias performance;
- admitted-window rate for one or more nodes; and
- energy per accepted measurement record.

## 16. References

- Analog Devices, [ADXL1005 product information](https://www.analog.com/en/products/adxl1005.html).
- Qorvo, [DWM3001C product information](https://www.qorvo.com/products/p/DWM3001C).
- Qorvo, [DWM3001CDK development kit product information](https://www.qorvo.com/products/p/DWM3001CDK).
- Nordic Semiconductor, [nRF52833 product information](https://www.nordicsemi.com/Products/nRF52833).
- STMicroelectronics, [STM32N657X0 product information](https://www.st.com/en/microcontrollers-microprocessors/stm32n657x0.html).
- `docs/dwm-to-nucleo-host-interface-protocol.md`.
- `docs/data-path-flow-control-and-buffering.md`.
- `docs/node-acquisition-and-sampling-contract.md` — to be added.

## 17. Engineering position

A 4160-byte bounded raw-window record is small enough to form a rigorous first transport unit, but large enough to force honest engineering decisions about admission, fragmentation, buffering and sample-rate meaning. The project should therefore measure and publish real end-to-end window performance rather than extrapolating from PHY rate, processor speed or memory capacity.
