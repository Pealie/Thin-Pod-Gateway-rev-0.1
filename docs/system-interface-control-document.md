# Thin-Pod rev 0.1 to Thin-Pod Gateway rev 0.1: System Interface Control Document

**Document identifier:** `TP-SYS-ICD-0001`  
**Revision:** Draft A  
**Document date:** 27 May 2026  
**Node hardware:** Thin-Pod rev 0.1  
**Gateway hardware:** Thin-Pod Gateway rev 0.1  
**System state:** Node OSHWA application submitted; Gateway PCB ordered and awaiting bring-up  
**Scope note:** This system document does not expand the submitted OSHWA certification scope of Thin-Pod rev 0.1. The Gateway remains a separate hardware product and future certification candidate.

## 1\. Purpose

This Interface Control Document (ICD) defines the intended boundaries between:

* the **Thin-Pod rev 0.1** vibration-sensor node carrier PCB;
* the **Thin-Pod Gateway rev 0.1** receiver/host carrier PCB;
* the Qorvo UWB development subsystems used on each side;
* the Gateway NUCLEO host/supervisor; and
* optional onward-networking and later analytic functionality.

Its purpose is to prevent system integration from drifting into informal assumptions. Each subsystem may evolve independently, but any end-to-end result must cross defined electrical, data and responsibility boundaries.

## 2\. Certification and revision boundary

### 2.1 Separate hardware products

|Hardware product|Revision|Certification position|
|-|-:|-|
|Thin-Pod sensor-node carrier PCB|rev 0.1|Submitted to OSHWA as a bounded open-hardware sensor-node release|
|Thin-Pod Gateway carrier PCB|rev 0.1|Separate PCB in manufacture/bring-up phase; no OSHWA application yet submitted|

The Gateway is explicitly outside the node certification scope. The submitted node documentation excludes Gateway hardware, implemented wireless transport, DSP and TinyML from the rev 0.1 node claim.

### 2.2 Deferred rev 0.3 direction

No rev 0.3 hardware is being implemented in the present work. A later rev 0.3 direction may introduce SMT and chip-down integration, including DWM3001C module integration and further manufacturing refinement. The present ICD is written for the two separate rev 0.1 carrier-board products.

## 3\. System architecture

```text
Mechanical vibration source
        ↓
Thin-Pod rev 0.1 sensor-node carrier PCB
  ADXL1005-based analogue vibration interface
  protected / regulated / switched sensor power
        ↓ analogue signal into node-side commercial CDK interface
Node-side Qorvo DWM3001-CDK subsystem
        ↓ intended UWB framed vibration-window transport
Gateway-side Qorvo DWM3001-CDK subsystem
  mounted on Thin-Pod Gateway rev 0.1
        ↓ wired host-interface route to be verified
STM32 NUCLEO-N657X0-Q
  buffer reception, logging and later DSP supervisor
        ↓ optional onward interface, separately proven
Seeed Studio XIAO ESP32-C6
```

## 4\. Responsibility allocation

|Subsystem|Primary responsibility in rev 0.1 system development|Explicit non-responsibility at present|
|-|-|-|
|Thin-Pod rev 0.1 carrier PCB|Provide powered analogue vibration measurement path and node-side CDK physical/electrical interface|Does not certify or guarantee completed wireless transport, DSP or TinyML|
|Node-side DWM3001-CDK|Prospective node-side UWB acquisition/transport subsystem|Not claimed as Thin-Pod-created open hardware|
|Gateway-side DWM3001-CDK|Prospective receiver/communications subsystem for framed windows|Not assumed to expose a proven host protocol until tested|
|Thin-Pod Gateway rev 0.1 PCB|Provide physical interconnect, rails, test access and host-interface wiring|Does not by itself prove firmware exchange or UWB reception|
|NUCLEO-N657X0-Q|Host/supervisor for receipt into memory, logging and later DSP|Should not be presumed to control the DW3110 radio directly|
|XIAO ESP32-C6|Optional onward-networking subsystem after local receipt/processing is proven|Not required for first node-to-Gateway transport proof|
|DSP/TinyML software|Future analysis of received windows|Outside current hardware-certification claims|

## 5\. Node-side physical and measurement interface

### 5.1 Thin-Pod rev 0.1 measurement chain

The node release provides:

```text
Mechanical excitation
        ↓
ADXL1005-based analogue sensor interface
        ↓
raw analogue signal
        ↓
filtered / ADC node
        ↓
node-side DWM3001-CDK ADC/control interface
```

The rev 0.1 node has documented prototype evidence of a switched sensor rail near 3.35 V, a resting filtered analogue output near 1.76 V and visible response to mechanical excitation after prototype ground-return correction. Its released copper design incorporates the corrected CDK ground return.

### 5.2 Node-to-radio responsibility boundary

|Boundary item|Node carrier responsibility|Node CDK/firmware responsibility|
|-|-|-|
|Sensor power|Provide protected/regulator/switched rail arrangement|Not applicable except loading/operation|
|Analogue output|Provide conditioned analogue signal at CDK-facing node|Sample signal through appropriate CDK/MCU capability|
|Sensor metadata|Define hardware identity/calibration information when implemented|Frame metadata for transport when firmware exists|
|UWB transport|Provide physical CDK interface only|Implement radio communications and framing|

## 6\. Wireless transport boundary

### 6.1 Transport concept

The intended transport unit is a bounded **vibration window**, not an indefinite continuous stream. This matches a modular diagnostic system in which measurements are sampled, framed, transferred and then analysed at the Gateway.

```text
bounded sample window + metadata + integrity field → UWB transfer → Gateway receipt
```

\## Flow control and overrun prevention



Gateway rev 0.1 is intended to use a credit-controlled, complete-window transfer

model. The Gateway-side DWM3001 subsystem will receive, reassemble and validate

framed vibration windows, but will expose records to the NUCLEO only through a

firmware-defined host interface. The NUCLEO will control bulk SPI transfers,

accept windows into preallocated raw-data buffers and grant further receive

capacity only while buffer space remains available.



Where receive or analysis capacity is exhausted, the intended behaviour is to

withhold new window credits or report a busy/overflow-prevention state rather

than silently overwrite accepted measurement data. DSP, TinyML and optional

network reporting remain downstream consumers and must not block the

measurement-ingress path.



### 6.2 Current status

|Transport aspect|Status|
|-|-|
|Physical node-side CDK interface|Present in Thin-Pod rev 0.1 release|
|Physical Gateway-side CDK interface|Present in ordered Gateway rev 0.1 carrier PCB; pending bring-up|
|Radio firmware|Not defined as part of submitted node certification|
|UWB packet/window transfer|Not yet claimed as demonstrated|
|Gateway memory receipt|To be proven after Gateway bring-up|

## 7\. Gateway-side physical interface

### 7.1 Power-domain intent

|Gateway connection|Intended use|
|-|-|
|`5V\_GATEWAY` from NUCLEO `CN3.6`|Feed DWM3001-CDK intended 5 V route and XIAO `5V/VBUS`|
|`3V3\_GATEWAY` from NUCLEO `CN3.16`|Auxiliary/pull-up supply for carrier circuitry|
|Common `GND`|Shared electrical reference for Gateway modules and carrier PCB|
|XIAO `3V3` pin|Not driven as a carrier-board power input in rev 0.1|

### 7.2 Gateway-side DWM route

|Net|NUCLEO endpoint|DWM3001-CDK endpoint|Intended interface role|
|-|-|-|-|
|`SPI5\_SCK`|`CN15.11` / `PE15`|`J10.23` / `SPI1\_CLK`|Host clock route|
|`SPI5\_MISO`|`CN15.13` / `PG1`|`J10.21` / `SPI1\_MISO`|DWM-to-host data route|
|`SPI5\_MOSI`|`CN15.15`|`J10.19` / `SPI1\_MOSI`|Host-to-DWM data route|
|`DWM\_CS`|`CN15.17`|`J10.24` / `CS\_RPI`|DWM selection/control route|
|`DWM\_IRQ`|`CN15.16`|`J10.15`|DWM event/GPIO route|
|`DWM\_RESET`|`CN15.33`|`J10.12`|DWM reset route|

This wiring is to be treated as a host-interface candidate. The NUCLEO is not assumed to have direct access to the DWM3001C module’s internal DW3110 radio SPI; the appropriate software boundary is expected to involve firmware running in the Qorvo subsystem.

### 7.3 Optional XIAO route

|Net|NUCLEO endpoint|XIAO endpoint|Intended role|
|-|-|-|-|
|`SPI5\_SCK`|`CN15.11`|`D8` / `GPIO19`|Shared SPI clock|
|`SPI5\_MISO`|`CN15.13`|`D9` / `GPIO20`|Shared SPI return|
|`SPI5\_MOSI`|`CN15.15`|`D10` / `GPIO18`|Shared SPI outbound|
|`C6\_CS`|`CN15.19`|`D3` / `GPIO21`|Independent XIAO selection|
|`C6\_INT`|`CN15.5`|`D2` / `GPIO2`|Interrupt/control route|
|`5V\_GATEWAY`|`CN3.6`|`5V/VBUS`|Power input|
|`GND`|Common|`GND`|Reference|

The XIAO is not required to establish the initial UWB-to-NUCLEO memory path and should not be allowed to complicate early bring-up.

## 8\. Data interface concept

### 8.1 Development sequence

The data-path proof should advance in controlled increments:

|Phase|Data source|Gateway milestone|
|-|-|-|
|P0|Static hardware test only|Power, ground and route verified|
|P1|NUCLEO-local synthetic buffer|Memory/storage/log pipeline established independently of DWM|
|P2|DWM-side synthetic framed payload|Known buffer transfers across DWM-to-NUCLEO host route|
|P3|UWB-received known test payload|Wireless reception reaches NUCLEO memory with integrity evidence|
|P4|Node-generated vibration window|Actual measurement window received and recorded|
|P5|DSP baseline|RMS, peak, crest factor, kurtosis and later band/envelope features evaluated|
|P6|Optional onward reporting|XIAO or other route passes processed record outward|
|P7|TinyML evaluation|Considered only after trustworthy repeatable data path exists|

### 8.2 Initial framed window record

The first protocol version should prioritise traceability and integrity rather than compression or sophistication.

|Field|Initial type / meaning|Required in first host-transfer proof?|
|-|-|-:|
|`magic`|Fixed value identifying Thin-Pod frame|Yes|
|`protocol\_version`|Unsigned version identifier, initial `0x0001`|Yes|
|`message\_type`|`SYNTHETIC\_WINDOW`, `TEST\_PAYLOAD`, `VIBRATION\_WINDOW`, `STATUS`|Yes|
|`node\_id`|Logical transmitting-node identity|Yes for wireless/node test|
|`gateway\_id`|Optional receiving Gateway identity|Later / logging|
|`sequence\_number`|Incrementing frame counter|Yes|
|`sample\_rate\_hz`|Sampling-rate metadata|Yes for window types|
|`sample\_count`|Number of samples represented|Yes for window types|
|`sample\_format`|e.g. signed 16-bit little-endian|Yes for window types|
|`sensor\_id`|Sensor/channel identity|Yes for actual node window|
|`calibration\_state`|Placeholder/version marker|Recommended for actual node window|
|`payload\_length\_bytes`|Payload size|Yes|
|`payload`|Sample values or known test sequence|Yes|
|`crc32`|Initial integrity field|Yes|
|`processing\_state`|Raw / filtered / derived marker|Later, when applicable|
|`firmware\_build\_id`|Sender/receiver traceability|Recommended once firmware stabilises|

### 8.3 Initial synthetic-window test vector

A suitable first framed buffer is:

|Parameter|Initial test value|
|-|-|
|`protocol\_version`|`0x0001`|
|`message\_type`|`SYNTHETIC\_WINDOW`|
|`node\_id`|`NODE\_TEST\_0001`|
|`sequence\_number`|`1`|
|`sample\_rate\_hz`|`25600` provisional|
|`sample\_count`|`256` provisional|
|`sample\_format`|`int16\_le`|
|`payload`|Deterministic incrementing ramp or generated sinusoid table|
|`crc32`|Calculated over defined header/payload range|

The provisional values are for communications-path development only; they do not set the final vibration bandwidth, ADC sampling plan or UWB efficiency target.

## 9\. Error and integrity behaviour

A trusted measurement path must not silently convert transport errors into apparently valid sensor data.

|Event|Required Gateway behaviour|
|-|-|
|Incorrect `magic` or unsupported `protocol\_version`|Reject frame; increment/log protocol error|
|Incorrect declared length|Reject frame; log framing error|
|CRC failure|Reject or quarantine payload; log integrity error|
|Sequence gap|Record dropped/missed-window event|
|Duplicate sequence|Record duplicate/replay condition during development|
|Unsupported sample format|Reject processing; retain diagnostic log|
|Missing sensor/calibration metadata for actual window|Record incomplete-measurement status; do not silently treat as calibrated|
|Host-interface timeout|Record timeout and subsystem identity|
|UWB receive failure|Preserve error counter; no fabricated sample record|

## 10\. First end-to-end acceptance criteria

The first meaningful system-level proof occurs when all of the following are true:

|Criterion|Evidence|
|-|-|
|Thin-Pod node-side physical analogue path is known to respond to mechanical excitation|Existing node prototype/release evidence, followed later by corrected-node test evidence|
|Gateway rev 0.1 powers and routes its commercial modules safely|Completed bring-up protocol record|
|A known payload reaches Gateway NUCLEO memory through the Gateway DWM host route|Buffer dump/log, CRC match and repeatability record|
|A known payload received over UWB reaches NUCLEO memory|Transmit/receive record and integrity evidence|
|An actual vibration window reaches NUCLEO memory|Framed measurement record with sample metadata|
|One conventional processing result is generated reproducibly|Logged RMS/peak or equivalent output with input buffer preserved|

This is a transport-and-observability milestone, not a predictive-maintenance claim.

## 11\. Documentation and repository placement

The system ICD should not be added to the submitted node certification repository in a way that suggests the Gateway has become part of the certified node scope. Suitable placement is:

```text
Thin-Pod-Gateway-rev0.1/
└── docs/
    └── system-interface-control-document.md
```

or, later, a separate system-level documentation repository clearly referencing both independent hardware artefacts.

The node repository may contain only a brief cross-reference after the Gateway exists as a separate public release, while retaining its existing statement that Gateway hardware and system functionality are outside the node’s submitted certification scope.

## 12\. Future revision control

|Future change|Revision treatment|
|-|-|
|Gateway rev 0.1 test results added without PCB change|Update Gateway rev 0.1 verification documentation|
|Gateway PCB correction discovered before first OSHWA submission|Incorporate into the clean first Gateway rev 0.1 release and retain prototype record|
|Gateway design changed after a frozen/certified rev 0.1 release|Assign later Gateway revision|
|SMT/chip-down integration, such as replacing development-module carrier approach|Reserve for later rev 0.3 programme; not part of current work|
|Changes to node PCB after submitted/frozen Thin-Pod rev 0.1|Separate node revision; do not silently expand certification record|
|Protocol change|Increment protocol version independently of board version when appropriate|

## References and source basis

* Thin-Pod rev 0.1 OSHWA certification-scope document, which explicitly excludes Gateway hardware and communications/DSP/TinyML implementation from the submitted node scope.
* Thin-Pod Gateway fabrication-order engineering record dated 21 May 2026, recording module mapping, rails and initial bring-up checklist.
* STMicroelectronics, `NUCLEO-N657X0-Q` product documentation: [https://www.st.com/en/evaluation-tools/nucleo-n657x0-q.html](https://www.st.com/en/evaluation-tools/nucleo-n657x0-q.html)
* Qorvo, `DWM3001CDK` product documentation: [https://www.qorvo.com/products/p/DWM3001CDK](https://www.qorvo.com/products/p/DWM3001CDK)
* Seeed Studio, `XIAO ESP32C6` documentation: [https://wiki.seeedstudio.com/xiao\_esp32c6\_getting\_started/](https://wiki.seeedstudio.com/xiao_esp32c6_getting_started/)

