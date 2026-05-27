# Thin-Pod Gateway rev 0.1: Data-Path Flow Control and Buffering

**Document identifier:** `TP-GW-DATA-0001`  
**Document revision:** Draft A  
**Product:** Thin-Pod Gateway  
**Hardware release path:** rev 0.1  
**Related node hardware:** Thin-Pod rev 0.1  
**Document status:** Pre-bring-up architecture and commissioning specification  
**Document date:** 27 May 2026  
**Certification boundary:** Gateway rev 0.1 is a separate future OSHWA candidate; this document does not extend the submitted OSHWA scope of Thin-Pod rev 0.1.

## 1\. Purpose

This document defines how Thin-Pod Gateway rev 0.1 should avoid being overrun by vibration-window data received through the Qorvo communications subsystem.

The design objective is not merely to move bytes quickly. It is to preserve an inspectable measurement chain in which complete raw vibration windows, their metadata and their integrity status are retained before DSP or later TinyML processing takes place.

The core rule is:

> \*\*The system admits bounded vibration windows under buffer control. It does not permit uncontrolled producer-driven streaming into Gateway memory.\*\*

The implementation described here is an initial commissioning architecture. It is intended to be conservative, observable and testable on the rev 0.1 hardware. It may later be refined after host-interface, UWB and DSP timing have been measured.

## 2\. Architectural position

The intended modular architecture is:

```text
Thin-Pod rev 0.1 / later node firmware
ADXL1005-based analogue vibration measurement
        ↓
node-side Qorvo DWM3001 subsystem
sample-window capture, framing and UWB transmission
        ↓
UWB transfer of explicitly admitted bounded windows
        ↓
Gateway-side Qorvo DWM3001 subsystem
UWB reception, fragment reassembly, integrity validation and short queueing
        ↓  firmware-defined SPI / IRQ host interface
Thin-Pod Gateway rev 0.1 NUCLEO-N657X0-Q
admission control, raw-window storage, DSP and later TinyML
        ↓  optional downstream consumer only
XIAO ESP32-C6
onward networking / reporting
```

The architectural responsibility split is:

|Subsystem|Owns|Does not own at this stage|
|-|-|-|
|Thin-Pod node carrier PCB|Analogue measurement path and node-side commercial-module interface|Gateway processing or completed radio link claims|
|Node-side Qorvo subsystem|Window capture/framing and UWB transmission firmware|Gateway buffer policy|
|Gateway-side Qorvo subsystem|UWB reception, validation, short queue and host-facing record presentation|DSP/TinyML analysis decisions|
|NUCLEO-N657X0-Q|Authoritative buffer admission, raw-window retention, DSP and later TinyML|Direct control of the DW3110 radio as though the Qorvo module were a transparent radio breakout|
|XIAO ESP32-C6|Optional onward networking after local receipt is secure|Measurement-critical ingress path|

The essential design principle is:

```text
raw vibration windows from the Pod;
validated window transfer through the UWB subsystem;
features and later models on the Gateway.
```

## 3\. Why overrun control is required

There are two independent data-pressure boundaries:

```text
Boundary A:
Node-side Qorvo subsystem  ── UWB ──>  Gateway-side Qorvo subsystem

Boundary B:
Gateway-side Qorvo subsystem  ── SPI / IRQ ──>  NUCLEO
```

Boundary B is not the only risk. Even where the NUCLEO chooses when to clock SPI payloads into its own memory, the Gateway-side Qorvo subsystem may continue receiving UWB records and run out of local queue space unless the upstream radio path also obeys admission control.

The system therefore requires backpressure from the NUCLEO all the way to the transmitting node:

```text
NUCLEO buffer availability
        ↓
Gateway-side Qorvo window-credit allowance
        ↓
Node-side permission to transmit a new measurement window
```

When the Gateway cannot preserve another complete window, the correct outcome is to withhold permission or report a busy condition. The system must not silently overwrite already accepted measurement evidence.

## 4\. Physical host-interface basis on Gateway rev 0.1

Gateway rev 0.1 provides a candidate wired host-interface route between the NUCLEO and the Gateway-side Qorvo DWM3001-CDK:

|Gateway signal|NUCLEO connection|DWM3001-CDK connection|Intended host-interface role|
|-|-|-|-|
|`SPI5\_SCK`|`CN15.11 / PE15`|`J10.23 / SPI1\_CLK`|NUCLEO-clocked serial transfer|
|`SPI5\_MISO`|`CN15.13 / PG1`|`J10.21 / SPI1\_MISO`|DWM-to-NUCLEO payload/status return|
|`SPI5\_MOSI`|`CN15.15 / PG2`|`J10.19 / SPI1\_MOSI`|NUCLEO-to-DWM command/data path|
|`DWM\_CS`|`CN15.17 / PA3`|`J10.24 / CS\_RPI`|DWM host-interface selection|
|`DWM\_IRQ`|`CN15.16 / PB9`|`J10.15 / GPIO\_RPI`|Record-ready or status-event notification|
|`DWM\_RESET`|`CN15.33 / PD0`|`J10.12 / RESET`|Controlled subsystem reset/recovery|

This route is intended for a firmware-defined interface to the DWM3001 subsystem’s exposed processing/GPIO interface. It is not treated as proof of direct NUCLEO access to, or ownership of, the internal DW3110 radio bus.

### Host-interface mode assumption

The first implementation should treat:

```text
NUCLEO:                  SPI host/master and admission controller
Gateway DWM firmware:    SPI peripheral/slave and communications coprocessor
DWM\_IRQ:                 level-signalled record/status notification
```

This arrangement has an important flow-control property: the NUCLEO controls when bulk SPI payload transfer occurs because it initiates and clocks each host-interface transaction.

### SPI-rate caution

The maximum reliable NUCLEO-to-DWM SPI rate is **not assumed in this document**. It depends on the implemented nRF52833 peripheral-mode firmware, the chosen transaction protocol, GPIO configuration, PCB route integrity and measured rev 0.1 bring-up performance.

Commissioning should begin at a conservative SPI clock, for example 1 MHz or 2 MHz, and increase only after repeated byte-accurate synthetic-window transfers have been demonstrated.

## 5\. Window-based measurement unit

The host and wireless protocol should transfer **complete immutable window records**, not an unbounded byte stream.

Each accepted record consists of:

```text
WINDOW\_HEADER
RAW\_SAMPLE\_PAYLOAD
WINDOW\_INTEGRITY\_FIELD
```

A window is exposed to DSP only after:

1. complete reception or generation;
2. framing and length checks;
3. integrity validation;
4. successful transfer into NUCLEO-owned memory; and
5. successful host-side validation.

## 6\. Initial record format

The first protocol record should be intentionally stable and inspectable. A 64-byte fixed header is recommended for first implementation because it is easy to align and permits later additions without immediately changing every transfer assumption.

### 6.1 Proposed header fields

|Field|Initial type|Meaning|Required at first proof?|
|-|-:|-|-:|
|`magic`|`uint32\_t`|Thin-Pod frame identifier|Yes|
|`protocol\_version`|`uint16\_t`|Interface/protocol version; initially `0x0001`|Yes|
|`message\_type`|`uint16\_t`|Synthetic window, test payload, vibration window, status or error|Yes|
|`node\_id`|`uint32\_t`|Source Pod identity|Yes for node/wireless proof|
|`window\_sequence`|`uint32\_t`|Ordered source-window identifier|Yes|
|`sample\_rate\_hz`|`uint32\_t`|Sampling-rate metadata|Yes for measurement window|
|`sample\_count`|`uint16\_t`|Number of raw samples|Yes|
|`sample\_format`|`uint16\_t`|Initially signed 16-bit little-endian|Yes|
|`sensor\_id`|`uint32\_t`|Sensor/channel identity|Yes for actual measurement|
|`calibration\_version`|`uint32\_t`|Calibration or configuration record identifier|Placeholder permitted initially|
|`acquisition\_flags`|`uint32\_t`|Triggered, scheduled, clipped, test, incomplete, etc.|Recommended|
|`payload\_bytes`|`uint32\_t`|Declared payload length|Yes|
|`timestamp\_or\_tick`|`uint64\_t`|Node-side acquisition timing record|Recommended once available|
|`firmware\_build\_id`|`uint32\_t`|Source firmware traceability marker|Recommended|
|`reserved`|Remaining bytes|Future compatible growth / alignment|Yes, zero-filled initially|

### 6.2 Initial message types

|Message type|Purpose|
|-|-|
|`SYNTHETIC\_WINDOW`|Deterministic byte-pattern record for wired host-interface verification|
|`UWB\_TEST\_WINDOW`|Known framed record transported through UWB before live sensor acquisition|
|`VIBRATION\_WINDOW`|Actual raw vibration measurement record|
|`STATUS`|Capacity, protocol or diagnostic report|
|`ERROR`|Explicit failure/event report|

## 7\. Initial commissioning window size

The recommended first record size is:

```text
Sample count:          2048 samples
Sample format:         signed 16-bit little-endian
Raw payload size:      4096 bytes
Header/alignment:      64 bytes
Initial record size:   4160 bytes
```

For a single-axis measurement, this corresponds to:

|Sample rate|Time represented by one 2048-sample window|Raw sample-data rate if continuously produced|
|-:|-:|-:|
|25.6 ksample/s|80 ms|409.6 kbit/s|
|51.2 ksample/s|40 ms|819.2 kbit/s|

The ADXL1005 is a wide-bandwidth analogue accelerometer; high-rate sampling remains a legitimate later measurement goal. The first communications proof should nevertheless use deterministic synthetic records before attempting actual high-bandwidth sensor acquisition.

The design intent is **selected bounded windows**, not indefinite raw streaming. This leaves room for framing, UWB protocol overhead, integrity checking, reassembly, retransmission policy, multiple-node scheduling and power management.

## 8\. Gateway-side DWM buffer model

The Gateway-side DWM subsystem should initially use two complete-window slots. This is sufficient to prove the data path while remaining conservative with local communications-subsystem memory.

### 8.1 Initial DWM allocation

|Allocation|Initial value|
|-|-:|
|Complete-window slots|2|
|Bytes per record|4160 bytes|
|Raw record-buffer allocation|8320 bytes|
|Slot A initial role|Receive/reassembly working slot|
|Slot B initial role|Completed record waiting for host transfer|

A four-slot DWM queue is a later optimisation, to be considered only after measured firmware-memory use, UWB receive behaviour and NUCLEO service latency are known.

### 8.2 DWM slot state machine

```text
FREE
  ↓ accepted credit and incoming window begins
RX\_IN\_PROGRESS
  ↓ all fragments received and integrity accepted
READY\_FOR\_HOST
  ↓ NUCLEO selects record and begins read
HOST\_TRANSFER\_IN\_PROGRESS
  ↓ NUCLEO acknowledges verified ownership
FREE
```

Fault path:

```text
RX\_IN\_PROGRESS or HOST\_TRANSFER\_IN\_PROGRESS
  ↓ timeout, CRC failure, framing error or host rejection
ERROR\_QUARANTINED
  ↓ counter/event recorded and explicit release action
FREE
```

### 8.3 DWM ownership rule

A `READY\_FOR\_HOST` or `HOST\_TRANSFER\_IN\_PROGRESS` record must not be overwritten by a newly received window. A completed record remains owned by the DWM queue until the NUCLEO has acknowledged successful transfer or expressly rejected it.

## 9\. NUCLEO raw-window memory model

The NUCLEO should hold a deeper queue than the DWM communications subsystem, because it is responsible for preserving raw measurement evidence while analysis proceeds asynchronously.

### 9.1 Initial NUCLEO allocation

|Allocation|Initial value|
|-|-:|
|Raw-window slots|32|
|Bytes per record|4160 bytes|
|Raw-window buffer allocation|133,120 bytes|
|Intended purpose|Ingress elasticity and preserved raw evidence for later DSP processing|

This is a first implementation value. It should later be adjusted against measured task latency, desired event retention and any storage/logging strategy.

### 9.2 NUCLEO slot state machine

```text
FREE
  available for a newly admitted record

FILLING
  SPI/DMA or host-interface transfer in progress

VALID\_RAW
  complete record received; framing and integrity accepted

PROCESSING
  DSP or later inference task consumes the preserved record

PROCESSED\_RETAIN
  raw measurement retained because it supports a logged event/result

RELEASED
  slot returns to FREE pool
```

Fault state:

```text
INVALID\_QUARANTINED
  framing, CRC, sequence or metadata failure retained long enough for diagnosis
```

### 9.3 NUCLEO ownership rule

A `VALID\_RAW` record must not be silently overwritten to make space for a later record. When raw-window capacity approaches exhaustion, the NUCLEO must withdraw further admission credits before accepted measurement records are lost.

## 10\. Flow-control model

### 10.1 Credit meaning

A **window credit** means:

> Capacity exists for one additional complete validated vibration-window record to be received by the Gateway DWM subsystem and subsequently transferred into NUCLEO-owned raw-window storage.

Credits are issued only where the NUCLEO buffer policy and the DWM local queue both permit another record.

### 10.2 Admission chain

```text
NUCLEO has permissible FREE capacity
        ↓
NUCLEO grants credit to Gateway-side DWM firmware
        ↓
Gateway DWM admits or schedules a single node-side window transmission
        ↓
Node-side DWM transmits only under that admission policy
        ↓
Gateway DWM reassembles and validates complete record
        ↓
Gateway DWM asserts DWM\_IRQ / DATA\_READY
        ↓
NUCLEO reads record into a FREE raw-window slot
        ↓
NUCLEO validates and ACKs ownership
        ↓
Credit may be replenished only if capacity remains
```

### 10.3 Fundamental overload rule

```text
No free preservation capacity → no new measurement-window admission.
```

A busy or credit-withheld event is an acceptable and recordable operational outcome. Silent overwrite of a completed accepted measurement record is not.

## 11\. Commissioning mode: stop-and-wait

The first complete implementation should permit only one admitted window at a time.

```text
NUCLEO verifies at least one FREE raw-window slot
        ↓
NUCLEO → Gateway DWM: SET\_CREDITS(1)
        ↓
Gateway DWM permits one synthetic/test/window transaction
        ↓
Gateway DWM validates complete received record
        ↓
Gateway DWM asserts DWM\_IRQ
        ↓
NUCLEO reads descriptor and payload
        ↓
NUCLEO validates record and sends ACK\_WINDOW
        ↓
Only then may the next credit be issued
```

### 11.1 Why stop-and-wait is preferred first

Stop-and-wait is not intended as the final throughput policy. It is the best initial proof mode because it isolates and verifies:

* record framing;
* DWM local ownership;
* interrupt behaviour;
* SPI transaction correctness;
* NUCLEO memory ownership;
* end-record integrity checking;
* acknowledgement/release behaviour;
* busy/withheld-credit behaviour; and
* evidence logging.

Pipelining should be introduced only after repeated success in this mode.

## 12\. Later pipelined admission policy

After stop-and-wait is proven, more than one outstanding credit may be considered.

A suitable policy form is:

```text
permitted\_credits =
    minimum(
        DWM\_free\_complete\_window\_slots,
        NUCLEO\_free\_raw\_slots - NUCLEO\_reserved\_free\_slots,
        scheduler\_limit
    )
```

Initial pipelined candidate settings:

|Control value|Proposed commissioning value|
|-|-:|
|DWM complete-window slots|2|
|NUCLEO raw-window slots|32|
|NUCLEO protected free-slot reserve|4|
|Initial outstanding credits|1|
|Later outstanding credits|No greater than available DWM slots and NUCLEO policy limit|

The protected NUCLEO reserve prevents queue exhaustion during in-flight transfer, error handling or controlled shutdown.

## 13\. NUCLEO-to-DWM host protocol

The first SPI host protocol should be simple and register-like. It should expose complete records and diagnostic state, without coupling the NUCLEO to radio-internal implementation detail.

### 13.1 Minimal command set

|Command|Direction|Purpose|
|-|-|-|
|`GET\_CAPABILITIES`|NUCLEO → DWM|Read protocol version, maximum record size, DWM slot count and firmware identity|
|`GET\_STATUS`|NUCLEO → DWM|Read ready-record count, active state, credits and error flags|
|`SET\_CREDITS`|NUCLEO → DWM|Set the number of further complete windows permitted|
|`READ\_DESCRIPTOR`|NUCLEO → DWM|Read the header/metadata of the oldest ready record|
|`READ\_PAYLOAD`|NUCLEO → DWM|Read payload bytes or chunks into NUCLEO-owned memory|
|`ACK\_WINDOW`|NUCLEO → DWM|Confirm validated ownership; permit DWM to free the local slot|
|`REJECT\_WINDOW`|NUCLEO → DWM|Reject record after failed host-side validation while preserving counters|
|`GET\_COUNTERS`|NUCLEO → DWM|Read loss, CRC, timeout, busy and host-interface counters|
|`CLEAR\_STATUS\_EVENTS`|NUCLEO → DWM|Acknowledge reported status/error events|
|`RESET\_INTERFACE`|NUCLEO → DWM|Controlled host-protocol recovery without implying direct radio ownership|

### 13.2 DWM\_IRQ behaviour

`DWM\_IRQ` should be configured as a level-signalled notification, with polarity fixed and documented during bring-up.

```text
DWM\_IRQ asserted while:
    ready\_record\_count > 0
    OR status\_event\_pending = true
    OR error\_event\_pending = true
```

The NUCLEO clears the underlying cause through explicit record acknowledgement/rejection and status-event handling. A level signal is preferred to an unlatched short pulse because a busy host task cannot miss that work remains pending.

## 14\. NUCLEO receive-task implementation

Bulk ingress must be isolated from processing work.

```text
DWM\_IRQ interrupt
        ↓
minimal interrupt handler records pending DWM service event
        ↓
high-priority receive task obtains FREE raw-window slot
        ↓
receive task reads STATUS and DESCRIPTOR
        ↓
receive task initiates payload transfer into FILLING slot
        ↓
receive task checks length, CRC and sequence
        ↓
ACK\_WINDOW or REJECT\_WINDOW issued
        ↓
accepted slot enters VALID\_RAW queue
        ↓
DSP task processes later
```

The interrupt handler must not:

* execute a bulk record transfer;
* calculate FFT or DSP features;
* run TinyML;
* wait for optional network reporting; or
* perform lengthy logging operations.

### 14.1 SPI transfer-rate commissioning

For the initial 4160-byte record, ideal raw transfer time is:

|SPI clock|Ideal transfer time for 4160 bytes|
|-:|-:|
|1 MHz|33.28 ms|
|2 MHz|16.64 ms|
|4 MHz|8.32 ms|
|8 MHz|4.16 ms|

These values are timing arithmetic only. They are not a claim that a given SPI rate has been proven on Gateway rev 0.1. The maximum reliable transaction rate must be established through repeated integrity-checked transfers on assembled hardware and the actual firmware interface.

## 15\. NUCLEO queue high-water policy

Using the proposed 32-slot raw-window queue:

|Slots used|Operational state|Admission behaviour|
|-:|-|-|
|0–15|Normal|Credits may be issued under ordinary scheduling rules|
|16–23|Elevated occupancy|Continue cautiously; record analysis/retention latency|
|24–27|High-water warning|Withhold or severely restrict new measurement credits until the queue drains|
|28–31|Protected reserve entered|Do not issue new measurement credits; retain capacity for in-flight/status/error handling|
|32|Full|Reject unsolicited arrival as a protocol fault; never overwrite a valid retained record|

The threshold values are commissioning parameters. They may be revised after measured DSP, logging and transport timing exists, but the behaviour principle must remain: buffer pressure causes backpressure, not silent loss.

## 16\. Node-side acquisition modes

### 16.1 First proof mode: capture or transmit only on grant

For the first transport proof, the node should transmit only in response to a Gateway-authorised window opportunity:

```text
Gateway grants one window
        ↓
Node emits one deterministic synthetic record
        ↓
later: Node captures and emits one measurement record
```

This mode avoids node-local queue ambiguity and allows deterministic verification.

### 16.2 Later monitoring mode: event capture before radio admission

A practical monitoring system may need to preserve a transient event before the Gateway is ready to receive it. That later mode requires node-local storage and explicit loss accounting:

|Node counter|Meaning|
|-|-|
|`windows\_captured`|Number of raw windows created locally|
|`windows\_transmitted`|Number of windows accepted and sent through transport|
|`windows\_deferred\_busy`|Captured windows awaiting transport because Gateway capacity is unavailable|
|`windows\_dropped\_local\_full`|Captured records lost because node-local storage is full|
|`trigger\_events\_without\_window`|Events detected without preserved sample evidence|

Event-triggered autonomous capture is deferred until the fundamental admitted transport path has been proven.

## 17\. UWB record fragmentation and reassembly

The initial 4096-byte raw payload should be expected to require subdivision into transport fragments. Fragment handling is the responsibility of the Qorvo-subsystem firmware, not the NUCLEO DSP task.

### 17.1 Fragment record requirements

|Fragment field|Purpose|
|-|-|
|`protocol\_version`|Ensures compatible reassembly rules|
|`window\_sequence`|Identifies the complete parent window|
|`fragment\_index`|Orders fragments|
|`fragment\_count`|Detects incomplete sets|
|`fragment\_payload\_bytes`|Permits final short fragment|
|`window\_total\_bytes`|Confirms reconstructed size|
|`window\_crc32`|Verifies the complete reconstructed record|
|`fragment\_flags`|Start, end, retransmitted, status or error marker|

The exact fragment payload size is intentionally not fixed here. It must be selected against the Qorvo firmware environment, UWB PHY/settings, any security/integrity overhead and measured transfer reliability.

### 17.2 Reassembly error behaviour

|Condition|Required Gateway-side DWM behaviour|
|-|-|
|Fragment missing at timeout|Mark window incomplete; do not expose it to NUCLEO as a valid record|
|Duplicate fragment|Handle deterministically and increment diagnostic counter|
|Incorrect reconstructed length|Reject record and report framing error|
|Whole-window CRC failure|Reject record; never issue `READY\_FOR\_HOST` as a valid measurement|
|No free receive slot|Withhold new admission / return busy state|
|NUCLEO host unavailable or unresponsive|Stop issuing new credits and record host-unavailable status|

## 18\. Measurement-integrity and fault counters

Missing or rejected data must be explicable. The following counters should be present in the DWM and NUCLEO firmware once the corresponding path is implemented.

### 18.1 Gateway-side DWM counters

|Counter|Meaning|
|-|-|
|`uwb\_windows\_granted`|Window opportunities authorised for transmission|
|`uwb\_windows\_started`|Window reassembly processes begun|
|`uwb\_windows\_completed\_valid`|Fully reconstructed, integrity-valid windows|
|`uwb\_windows\_crc\_failed`|Reconstructed records rejected by integrity checking|
|`uwb\_windows\_timed\_out`|Incomplete windows abandoned after timeout|
|`uwb\_busy\_responses`|Admissions refused/withheld for lack of capacity|
|`host\_records\_ready`|Complete records presented to host interface|
|`host\_records\_acked`|Records accepted into NUCLEO ownership|
|`host\_records\_rejected`|Records rejected by NUCLEO validation|
|`host\_service\_timeouts`|Records not serviced within defined host deadline|

### 18.2 NUCLEO counters

|Counter|Meaning|
|-|-|
|`records\_read`|Records transferred from DWM host interface|
|`records\_crc\_valid`|Records accepted after host-side integrity check|
|`records\_crc\_failed`|Records rejected after host-side integrity check|
|`sequence\_gaps`|Missing ordered records detected|
|`raw\_queue\_high\_water\_events`|Times ingress approached configured buffer threshold|
|`credits\_withheld`|Backpressure/admission decisions taken|
|`records\_processed\_dsp`|Raw records submitted to completed DSP calculation|
|`records\_retained`|Raw records preserved for later evidence/analysis|
|`records\_released`|Slot releases after policy permits disposal|
|`xiao\_reporting\_deferred`|Backhaul/reporting work deferred to protect ingress|

## 19\. Analysis and onward-networking scheduling rule

DSP and optional networking must remain downstream consumers of accepted raw data. They must not delay measurement ingress.

Proposed Gateway task priority:

|Priority|Task class|Reason|
|-:|-|-|
|1|Hardware fault, rail fault, watchdog and controlled interface recovery|Protect hardware and preserve failure evidence|
|2|DWM IRQ servicing and raw-window ingress|Preserve accepted measurement records|
|3|Host-side integrity checking and raw-record queueing|Establish trusted raw ownership|
|4|DSP feature extraction|Derive interpretable analysis from preserved raw data|
|5|Local logging and user-facing status|Record results without blocking ingress|
|6|XIAO onward-network reporting|Downstream optional consumer only|
|7|TinyML experimentation|Permitted only after repeatable data and DSP baseline exist|

For initial DWM-to-NUCLEO host-transfer experiments, the XIAO should remain unfitted or inactive. In later Gateway operation, any XIAO transaction sharing carrier-board SPI signal routes must yield to DWM ingress whenever a validated measurement record is pending.

## 20\. Staged implementation and acceptance plan

|Stage|DWM-subsystem activity|NUCLEO activity|Pass evidence|
|-|-|-|-|
|H0|Boot and expose capability/status structure|Read status repeatedly over host interface|Stable protocol identity and no unexplained interface errors|
|H1|Place one synthetic 4160-byte record in `READY\_FOR\_HOST`|Read into one raw-window slot and verify bytes/CRC|Exact byte match|
|H2|Generate increasing synthetic sequence records|ACK and log each accepted record|100 consecutive transfers without corruption or sequence error|
|H3|Retain one DWM record while NUCLEO deliberately delays ACK|Observe no overwrite and appropriate pending/busy status|DWM ownership/backpressure demonstrated|
|H4|Fill or simulate high occupancy in NUCLEO raw queue|Withdraw credits according to high-water policy|No valid raw record silently overwritten|
|H5|Receive a known framed payload through UWB|Read validated record into NUCLEO memory|First RF-to-host record with integrity evidence|
|H6|Receive an actual node-generated vibration window|Store accepted `VALID\_RAW` measurement record|First measurement transport proof|
|H7|Calculate RMS and peak from the retained raw record|Log input sequence/CRC and derived result|First analytic-supervisor proof|
|H8|Add crest factor, kurtosis, band energy and envelope work in sequence|Preserve processing version and input record|DSP baseline documented|
|H9|Enable optional XIAO reporting|Demonstrate that reporting yields to ingress|Backhaul does not compromise raw receipt|
|H10|Explore TinyML only after repeatable dataset exists|Run versioned inference against retained records|Later research result, not rev 0.1 hardware claim|

## 21\. Firmware implementation decision for first proof

The first practical implementation should be constrained to:

```text
One node
One admitted window at a time
2048 signed 16-bit samples per synthetic or measurement record
64-byte fixed/aligned record header
4160 bytes per initial complete record
Two complete-window slots in Gateway-side DWM firmware
Thirty-two complete raw-window slots in NUCLEO memory
NUCLEO-controlled SPI host reads
Level-signalled DWM\_IRQ for record/status pending
CRC and sequence verification before DSP
XIAO inactive during first ingress proof
No TinyML until transport and DSP baseline are repeatable
```

This provides a clear, testable definition of ‘overrun prevention’ on rev 0.1 hardware.

## 22\. Relationship to Gateway OSHWA preparation

This document forms part of the preparatory documentation for a later, separate OSHWA application for **Thin-Pod Gateway rev 0.1**.

It does not assert that:

* the Gateway PCB has already arrived or passed physical bring-up;
* the DWM-to-NUCLEO firmware-defined host protocol has already been implemented;
* UWB vibration-window transfer has already been demonstrated;
* DSP or TinyML has already been performed on received data; or
* the Gateway is included in the submitted OSHWA certification scope of Thin-Pod rev 0.1.

A future Gateway OSHWA application should reference physical bring-up evidence, reconciled source and fabrication outputs, BOM, licence/provenance documentation and this architecture record only after the relevant implementation and tests are complete.

## 23\. Documentation integration

This file should be placed at:

```text
Thin-Pod-Gateway-rev0.1/
└── docs/
    └── data-path-flow-control-and-buffering.md
```

The Gateway `README.md` should remain concise and link to this document for the detailed data-ingress architecture. The System Interface Control Document should refer to this file as the controlling record for window-admission, queueing, backpressure and overrun-prevention behaviour.

## 24\. References

### Project documentation

* `README.md` — Thin-Pod Gateway rev 0.1 overview and release boundary.
* `docs/certification-scope.md` — intended separate Gateway OSHWA boundary.
* `docs/gateway-bring-up-and-verification-protocol.md` — staged PCB and host-interface verification protocol.
* `docs/system-interface-control-document.md` — node-to-Gateway architectural interface record.
* Thin-Pod rev 0.1 repository documentation — submitted sensor-node certification boundary and prototype analogue evidence.

### Manufacturer and certification references

* Analog Devices, `ADXL1005` high-frequency, single-axis analogue accelerometer: [https://www.analog.com/en/products/adxl1005.html](https://www.analog.com/en/products/adxl1005.html)
* Qorvo, `DWM3001C` UWB module: [https://www.qorvo.com/products/p/DWM3001C](https://www.qorvo.com/products/p/DWM3001C)
* Qorvo, `DWM3001CDK` development kit: [https://www.qorvo.com/products/p/DWM3001CDK](https://www.qorvo.com/products/p/DWM3001CDK)
* Nordic Semiconductor, `nRF52833` product information: [https://www.nordicsemi.com/Products/nRF52833](https://www.nordicsemi.com/Products/nRF52833)
* STMicroelectronics, `NUCLEO-N657X0-Q`: [https://www.st.com/en/evaluation-tools/nucleo-n657x0-q.html](https://www.st.com/en/evaluation-tools/nucleo-n657x0-q.html)
* STMicroelectronics, `STM32N657X0`: [https://www.st.com/en/microcontrollers-microprocessors/stm32n657x0.html](https://www.st.com/en/microcontrollers-microprocessors/stm32n657x0.html)
* OSHWA Certification Requirements: [https://certification.oshwa.org/requirements.html](https://certification.oshwa.org/requirements.html)
* OSHWA Documentation Guidance: [https://certification.oshwa.org/process/documentation.html](https://certification.oshwa.org/process/documentation.html)

## 25\. Engineering decision

Thin-Pod Gateway rev 0.1 is to avoid overrun by making capacity explicit:

```text
bounded raw vibration windows
+ short Gateway-DWM complete-record queue
+ NUCLEO-controlled host-interface reads
+ deeper NUCLEO raw-record queue
+ credit-based backpressure to the transmitting Pod
+ explicit integrity checking and failure counters
+ optional networking kept downstream of measurement ingress
```

This architecture preserves measurement evidence, fits the modular Qorvo-subsystem-to-NUCLEO-supervisor division, and provides a disciplined route from first carrier-board bring-up to later DSP and TinyML work.

