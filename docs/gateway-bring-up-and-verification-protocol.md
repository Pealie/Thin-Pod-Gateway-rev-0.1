# Thin-Pod Gateway rev 0.1: Bring-Up and Verification Protocol

**Product:** Thin-Pod Gateway<br>
**Release path:** rev 0.1<br>
**Ordered fabrication-build identifier:** `rev 0.1f`, where present on the manufactured PCB/silkscreen<br>
**Board state at preparation:** PCB ordered; arrival and assembly were pending<br>
**Current status:** Protocol completed through the recorded rev 0.1 bring-up evidence; OSHWA UID `UK000092` issued 16 July 2026<br>

> **Historical protocol status (16 July 2026):** This document preserves the original commissioning plan and blank/pending worksheets. Completed bring-up evidence is recorded in [`docs/bringup/Gateway_rev0_1_Hardware_Bringup_Note.md`](bringup/Gateway_rev0_1_Hardware_Bringup_Note.md), and the released carrier is OSHWA-certified as [`UK000092`](https://certification.oshwa.org/uk000092.html).

**Document status:** Test protocol prepared before board arrival<br>
**Document date:** 27 May 2026<br>

## 1. Purpose

This document defines the staged bring-up protocol for the first manufactured **Thin-Pod Gateway rev 0.1** carrier PCB. Its role is to ensure that the board is verified as hardware before firmware or system-level work obscures basic electrical faults.

The Gateway is a separate hardware artefact from the already submitted **Thin-Pod rev 0.1** sensor-node carrier PCB. Nothing in this protocol extends or modifies the submitted sensor-node OSHWA certification boundary.

The immediate verification objectives are:

1. confirm that the fabricated carrier PCB corresponds physically to the intended NUCLEO, DWM3001-CDK and XIAO footprints;
2. confirm power, ground, pull-up and host-interface routing before fitting modules;
3. power modules in controlled stages;
4. establish an electrically trusted path on which host-interface firmware tests can later be performed; and
5. collect the evidence needed for a possible later OSHWA certification submission for **Thin-Pod Gateway rev 0.1**.

## 2. Hardware under test

The ordered Gateway carrier-board design is intended to hold:

| Reference | Hardware | Functional role |
|---|---|---|
| `U1` | STM32 `NUCLEO-N657X0-Q` | Main host and prospective DSP/TinyML supervisor |
| `U2` | Qorvo `DWM3001-CDK` | Gateway-side UWB development board |
| `U3` | Seeed Studio `XIAO ESP32-C6` | Optional onward-networking subsystem |
| `R1`, `R2` | 10 kΩ pull-ups | Chip-select idle-state control |
| `C1`, `C3` | 100 nF capacitors | Local high-frequency decoupling |
| `C2`, `C4` | 10 µF capacitors | Local bulk decoupling |
| `C5` | 22 µF capacitor | Additional local supply reserve |
| `TP1`–`TP13` | Test points | Power, reset, interrupt and SPI observability |
| `H1`–`H4` | M3 mounting holes | Mechanical support |

The ordered PCB has previously been recorded as an approximately 160 mm × 145 mm, two-layer carrier-board prototype with a bottom-copper `GND` zone and a fabrication/silkscreen identity including `rev 0.1f`. Before OSHWA release, this physical build identity must be reconciled to the public Gateway rev 0.1 release record.

## 3. Design intent to be verified

### 3.1 Power distribution

| Net / connection | Design intent | Verification status |
|---|---|---|
| `5V_GATEWAY` from NUCLEO `CN3.6` | Powers DWM3001-CDK 5 V input route and XIAO `5V/VBUS` | Pending physical test |
| `3V3_GATEWAY` from NUCLEO `CN3.16` | Provides pull-up/auxiliary 3.3 V rail | Pending physical test |
| Common `GND` | Ties NUCLEO, DWM3001-CDK, XIAO, passive returns and test points together | Pending physical test |
| XIAO `3V3` pin | Left unconnected as a power input in the carrier-board design | Pending continuity check |

### 3.2 NUCLEO-to-DWM3001-CDK route

| Signal | NUCLEO connection | DWM3001-CDK connection | Purpose |
|---|---|---|---|
| `SPI5_SCK` | `CN15.11` / `PE15` | `J10.23` / `SPI1_CLK` | Host clock route |
| `SPI5_MISO` | `CN15.13` / `PG1` | `J10.21` / `SPI1_MISO` | Module-to-host data route |
| `SPI5_MOSI` | `CN15.15` | `J10.19` / `SPI1_MOSI` | Host-to-module data route |
| `DWM_CS` | `CN15.17` | `J10.24` / `CS_RPI` | DWM chip-select/control route |
| `DWM_IRQ` | `CN15.16` | `J10.15` | DWM interrupt/GPIO route |
| `DWM_RESET` | `CN15.33` | `J10.12` | DWM reset route |

This is a physical route to the DWM3001-CDK interface. Whether it supports the desired framed-buffer exchange is a firmware/protocol verification question and must not be assumed solely from continuity.

### 3.3 NUCLEO-to-XIAO ESP32-C6 route

| Signal | NUCLEO connection | XIAO connection | Purpose |
|---|---|---|---|
| `SPI5_SCK` | `CN15.11` | `D8` / `GPIO19` / `SCK` | Shared SPI clock |
| `SPI5_MISO` | `CN15.13` | `D9` / `GPIO20` / `MISO` | Shared SPI data route |
| `SPI5_MOSI` | `CN15.15` | `D10` / `GPIO18` / `MOSI` | Shared SPI data route |
| `C6_CS` | `CN15.19` | `D3` / `GPIO21` | Separate chip-select route |
| `C6_INT` | `CN15.5` | `D2` / `GPIO2` | Interrupt/control route |
| `5V_GATEWAY` | `CN3.6` | `5V/VBUS` | XIAO power-input route |
| `GND` | Common reference | `GND` | Ground return |

## 4. Required equipment

| Equipment | Purpose |
|---|---|
| Digital multimeter with continuity and resistance modes | Unpowered continuity, resistance and voltage checks |
| Current-limited power source / correct NUCLEO power arrangement | Controlled powered bring-up |
| Oscilloscope | Rail behaviour and later SPI/control-line observation |
| Logic analyser, where available | SPI/control-line capture during firmware stages |
| ESD-safe work area | Protection of development modules |
| Camera / phone camera | Public evidence record |
| NUCLEO-N657X0-Q | Staged host-module installation |
| DWM3001-CDK | Staged UWB-module installation |
| XIAO ESP32-C6 | Staged optional-networking module installation |
| Appropriate headers/socketing hardware | Non-destructive, reversible module fitting |

## 5. Stop conditions

Bring-up must stop immediately where any of the following occurs:

- visible solder bridge, damaged pad, displaced copper or suspect connector orientation;
- continuity between a supply rail and `GND` inconsistent with expected passive loading;
- unexpected continuity between `5V_GATEWAY` and `3V3_GATEWAY`;
- unexpected continuity to the XIAO `3V3` pin;
- unexpected power rail voltage;
- abnormal current draw, heat, smell, smoke or LED behaviour;
- module orientation uncertainty; or
- a measured pin mapping inconsistent with the intended interface table.

No firmware activity should be used to rationalise an unresolved hardware failure.

## 6. Gate G0: Documentation and arrival record

### Objective

Establish the received hardware identity before any assembly or test.

### Record

| Item | Result / reference | Status |
|---|---|---|
| Supplier / order reference |  | Pending |
| PCB arrival date |  | Pending |
| Quantity received |  | Pending |
| Board identifier visible on silkscreen |  | Pending |
| Intended release relationship | Gateway rev 0.1; ordered fabrication build may be marked `rev 0.1f` | Prepared |
| Photographs taken: front and back |  | Pending |
| Packaging / visible damage checked |  | Pending |

### Acceptance criterion

Bare PCB is received without obvious physical damage, and its visible identity is documented.

## 7. Gate G1: Bare-board visual inspection

Inspect the PCB before soldering headers, fitting passives or mounting any commercial modules.

| Inspection point | Expected condition | Observation | Pass / fail |
|---|---|---|---|
| Board outline and rounded corners | Clean, complete routed outline |  |  |
| Mounting holes `H1`–`H4` | Correct position and clean drilling |  |  |
| NUCLEO footprint orientation | `CN3` left and `CN15` right in intended top/component view |  |  |
| DWM3001-CDK footprint orientation | Pad labels and module orientation correspond to source |  |  |
| XIAO footprint orientation | Pin 14/VBUS, pin 13/GND, pin 12/3V3 position understood |  |  |
| Test points `TP1`–`TP13` | Present, legible and accessible |  |  |
| Passive footprints | Values/reference text and orientation intelligible |  |  |
| Solder mask openings | No obvious missing or merged openings |  |  |
| Silkscreen | Legible and does not obscure critical pads |  |  |
| Copper/fabrication defects | No visible bridging, scratches or damaged pads |  |  |

### Evidence

Place photographs under:

```text
images/bring-up/G0-G1-arrival-and-visual-inspection/
```

## 8. Gate G2: Bare-board continuity and resistance checks

All tests in this gate are carried out with **no NUCLEO, no DWM3001-CDK and no XIAO fitted**.

### 8.1 Ground continuity

Confirm continuity among all intended ground points.

| From | To | Expected result | Measured result | Pass / fail |
|---|---|---|---|---|
| NUCLEO `CN3.8` | Gateway `GND` test point | Continuity |  |  |
| NUCLEO `CN3.19` | Gateway `GND` test point | Continuity |  |  |
| NUCLEO `CN3.20` | Gateway `GND` test point | Continuity |  |  |
| NUCLEO `CN3.22` | Gateway `GND` test point | Continuity |  |  |
| NUCLEO `CN15.9` | Gateway `GND` test point | Continuity |  |  |
| NUCLEO `CN15.20` | Gateway `GND` test point | Continuity |  |  |
| DWM3001-CDK GND pads | Gateway `GND` test point | Continuity |  |  |
| XIAO pin 13 / `GND` | Gateway `GND` test point | Continuity |  |  |
| Ground terminals of `C1`–`C5` | Gateway `GND` test point | Continuity |  |  |

### 8.2 No-short checks

| From | To | Expected result | Measured result | Pass / fail |
|---|---|---|---|---|
| `5V_GATEWAY` | `GND` | No low-resistance short |  |  |
| `3V3_GATEWAY` | `GND` | No low-resistance short |  |  |
| `5V_GATEWAY` | `3V3_GATEWAY` | No continuity |  |  |
| XIAO pin 12 / `3V3` | `5V_GATEWAY` | No continuity |  |  |
| XIAO pin 12 / `3V3` | `3V3_GATEWAY` | No continuity |  |  |
| SPI/control route | Adjacent unrelated pins | No continuity |  |  |

### 8.3 Resistance record

| Measurement | Expected interpretation | Measured resistance | Status |
|---|---|---|---|
| `5V_GATEWAY` to `GND` | Not a hard short; value recorded as baseline |  | Pending |
| `3V3_GATEWAY` to `GND` | Not a hard short; value recorded as baseline |  | Pending |
| `5V_GATEWAY` to `3V3_GATEWAY` | Not a hard short |  | Pending |
| `DWM_CS` to `3V3_GATEWAY` | Approximately fitted pull-up path, expected around 10 kΩ |  | Pending |
| `C6_CS` to `3V3_GATEWAY` | Approximately fitted pull-up path, expected around 10 kΩ |  | Pending |

### Acceptance criterion

All intended ground continuity checks pass; no supply short or unexpected XIAO power connection is found; pull-up paths show plausible resistance.

## 9. Gate G3: Critical signal-route continuity

With the PCB still unpowered and modules absent, confirm the host-interface nets.

### 9.1 DWM route

| Gateway signal | From NUCLEO pad | To DWM pad | Measured result | Pass / fail |
|---|---|---|---|---|
| `SPI5_SCK` | `CN15.11` | `J10.23` |  |  |
| `SPI5_MISO` | `CN15.13` | `J10.21` |  |  |
| `SPI5_MOSI` | `CN15.15` | `J10.19` |  |  |
| `DWM_CS` | `CN15.17` | `J10.24` |  |  |
| `DWM_IRQ` | `CN15.16` | `J10.15` |  |  |
| `DWM_RESET` | `CN15.33` | `J10.12` |  |  |

### 9.2 XIAO route

| Gateway signal | From NUCLEO pad | To XIAO pad | Measured result | Pass / fail |
|---|---|---|---|---|
| `SPI5_SCK` | `CN15.11` | pin 9 / `D8` |  |  |
| `SPI5_MISO` | `CN15.13` | pin 10 / `D9` |  |  |
| `SPI5_MOSI` | `CN15.15` | pin 11 / `D10` |  |  |
| `C6_CS` | `CN15.19` | pin 4 / `D3` |  |  |
| `C6_INT` | `CN15.5` | pin 3 / `D2` |  |  |
| NC check | N/A | XIAO pin 6 | No connection |  |

### Acceptance criterion

Each critical route corresponds to its intended pad mapping, with no observed cross-connection to adjacent pins.

## 10. Gate G4: Passive population and post-solder inspection

Populate only the carrier-board passives and headers required for staged testing, retaining the option not to fit downstream modules until power has been checked.

| Component group | Action | Evidence / result | Status |
|---|---|---|---|
| `R1`, `R2` pull-ups | Fit and inspect values/orientation |  | Pending |
| `C1`, `C3` 100 nF | Fit and inspect |  | Pending |
| `C2`, `C4` 10 µF | Fit with polarity check if relevant |  | Pending |
| `C5` 22 µF | Fit with polarity check if relevant |  | Pending |
| Headers/socketing | Fit for NUCLEO/DWM/XIAO only where orientation is confirmed |  | Pending |
| Solder joints | Magnified visual inspection |  | Pending |
| Repeat no-short checks | Re-run supply-to-ground tests after soldering |  | Pending |

### Acceptance criterion

Assembly is visually sound, and passive/header population has not created unintended rail shorts.

## 11. Gate G5: NUCLEO-only power validation

Fit the Gateway PCB to the NUCLEO only. **Do not fit the DWM3001-CDK or XIAO during the first powered test.**

Power the NUCLEO using its intended supported power method. Record the exact source and any jumper/configuration setting relevant to that power method.

| Measurement | Test point / location | Expected result | Measured result | Pass / fail |
|---|---|---|---|---|
| `5V_GATEWAY` | Gateway test point / distribution pad | Near 5 V |  |  |
| `3V3_GATEWAY` | Gateway test point / pull-up rail | Near 3.3 V |  |  |
| `GND` reference | Gateway ground point | Stable common reference |  |  |
| `DWM_CS` idle | DWM chip-select test point | High through pull-up |  |  |
| `C6_CS` idle | C6 chip-select test point | High through pull-up |  |  |
| DWM 5 V pad position | CDK `J10.2`/designated supply pad | Near 5 V |  |  |
| XIAO `5V/VBUS` pad | XIAO power pad | Near 5 V |  |  |
| XIAO `3V3` pin | XIAO pin 12 | No carrier-driven voltage |  |  |
| Thermal/visual observation | Whole assembly | No abnormal heating or behaviour |  |  |

### Acceptance criterion

Both Gateway rails and idle chip-select paths are correct, the XIAO power boundary is preserved, and no abnormal behaviour is observed.

## 12. Gate G6: XIAO-only module validation

Only after Gate G5 passes, fit the XIAO ESP32-C6 while leaving the DWM3001-CDK absent.

| Check | Expected result | Measured result | Pass / fail |
|---|---|---|---|
| XIAO `5V/VBUS` power | Correct 5 V input route |  |  |
| XIAO `GND` | Common with Gateway ground |  |  |
| XIAO `3V3` boundary | Not externally driven by carrier |  |  |
| `C6_CS` idle | High / sane default before active communication |  |  |
| `C6_INT` state | Sane level according to firmware/configuration |  |  |
| Thermal observation | No abnormal heating |  |  |
| Basic firmware presence, if exercised | XIAO responds through its supported development route |  |  |

### Acceptance criterion

The optional onward-networking module can be powered safely without disturbing the carrier-board rail assumptions.

## 13. Gate G7: DWM3001-CDK-only module validation

After Gate G5 passes, the DWM3001-CDK may be fitted and tested either with the XIAO absent or with the previously validated XIAO installation retained and clearly recorded.

| Check | Expected result | Measured result | Pass / fail |
|---|---|---|---|
| CDK supply pad | Intended 5 V power path present |  |  |
| CDK ground pads | Common with Gateway `GND` |  |  |
| `DWM_CS` idle | High / sane inactive level |  |  |
| `DWM_RESET` state | Not unintentionally held in reset |  |  |
| `DWM_IRQ` electrical condition | Not shorted to rail or ground |  |  |
| Module LED / power behaviour | Consistent with documented CDK operation |  |  |
| Thermal/current observation | No abnormal current or heating |  |  |

### Acceptance criterion

The Gateway powers the DWM3001-CDK safely and exposes its control-route signals without obvious electrical failure.

## 14. Gate G8: GPIO and low-speed SPI observability

Only after all preceding physical and power checks pass should the NUCLEO exercise control lines.

### Test order

1. Configure `DWM_CS` and `C6_CS` as host-controlled outputs.
2. Confirm both lines idle inactive/high.
3. Toggle each chip-select independently at a slow rate and observe at test points.
4. Exercise `DWM_RESET` under controlled conditions only after its polarity and normal idle state are confirmed.
5. Initialise SPI5 at a conservative low frequency.
6. Observe `SPI5_SCK` and `SPI5_MOSI` with only one subordinate device selected at a time.
7. Record any response on `SPI5_MISO`.
8. Confirm that deselected devices remain electrically inactive on the shared bus.

| Test | Instrument evidence | Result | Status |
|---|---|---|---|
| `DWM_CS` toggle | Scope / logic-analyser capture |  | Pending |
| `C6_CS` toggle | Scope / logic-analyser capture |  | Pending |
| SPI5 clock activity | Scope / logic-analyser capture |  | Pending |
| DWM selected transaction attempt | Capture and firmware log |  | Pending |
| XIAO selected transaction attempt | Capture and firmware log |  | Pending |
| Non-selected bus behaviour | Capture / observation |  | Pending |

### Acceptance criterion

Carrier-board control and SPI signals are observable at their intended pads and do not reveal a physical bus-routing failure.

## 15. Gate G9: Synthetic buffer host-interface milestone

### Objective

Demonstrate the first meaningful Gateway data-path milestone without yet depending on a live Thin-Pod UWB transmission:

> A known synthetic payload associated with the DWM-side communications route is delivered into NUCLEO-accessible memory, checked for integrity and logged reproducibly.

### Important architectural constraint

The DWM3001-CDK/DWM3001C subsystem is not to be treated as raw DW3110 radio silicon under direct NUCLEO control. The intended modular path is that the Qorvo subsystem manages its local UWB function and exposes a higher-level host route or firmware-defined interface. The physical SPI route on rev 0.1 must be evaluated in that context.

### Initial synthetic record

Use a deterministic test message such as:

```text
message_type:       SYNTHETIC_WINDOW
protocol_version:   0x0001
source_id:          DWM_GATEWAY_TEST
sequence_number:    1
sample_rate_hz:     25600
sample_count:       256
sample_format:      int16_le
payload_pattern:    incrementing ramp or known sinusoid table
payload_crc32:      calculated integrity value
```

The chosen sample rate and count are provisional test values for interface development, not a final Thin-Pod transport specification.

### Evidence table

| Evidence item | Required record | Result / file reference |
|---|---|---|
| Synthetic payload source | Exact bytes or generating script |  |
| NUCLEO received buffer | Memory dump or serial log |  |
| Integrity comparison | CRC/checksum and byte count |  |
| Repeatability | At least three repeated successful transfers |  |
| Bus capture | Optional logic-analyser screenshot |  |
| Firmware versions | Commit/hash or build identifier |  |

### Acceptance criterion

A known buffer appears in NUCLEO-accessible memory without corruption, with a repeatable integrity check and documented firmware/toolchain state.

## 16. Gate G10: First UWB-received payload milestone

### Objective

Replace the synthetic test record with a payload actually received over the intended UWB subsystem path.

The first payload need not be a full calibrated vibration measurement. It must be a known, framed transmitted payload that passes through the UWB reception route and is made available to NUCLEO memory.

| Evidence item | Required record | Result / file reference |
|---|---|---|
| Transmitting endpoint | Hardware and firmware identity |  |
| Received payload format | Protocol version and framing |  |
| Packet/window count | Sent versus received counts |  |
| Integrity checks | CRC/error counters |  |
| NUCLEO buffer evidence | Memory/log capture |  |
| Failure cases | Dropped/corrupt packet observations |  |

### Acceptance criterion

A framed UWB-received record is delivered into Gateway-host memory and its integrity/result is recorded.

## 17. Gate G11: Initial analytic-supervisor proof

Only after a received window exists in NUCLEO-accessible memory should the first DSP calculation be performed.

| Processing result | Purpose | Status |
|---|---|---|
| RMS | Basic signal-energy measure | Pending |
| Peak / peak-to-peak | Basic amplitude sanity check | Pending |
| Crest factor | Initial impulsive-content measure | Pending |
| Kurtosis | Initial transient/fault-sensitive statistic | Pending |
| Band-energy placeholder | Confirms FFT/filter feature route when sample-rate/window is fixed | Pending |
| Metadata/error record | Preserves traceability of processing state | Pending |

TinyML remains outside the rev 0.1 Gateway bring-up milestone. The first requirement is a trusted data path and conventional DSP baseline.

## 18. Bring-up summary record

Complete after each work session.

| Date | Gates attempted | Gates passed | Faults discovered | Corrections made | Evidence committed |
|---|---|---|---|---|---|
|  |  |  |  |  |  |

## 19. OSHWA release decision after bring-up

After hardware bring-up:

| Outcome | Appropriate action |
|---|---|
| Ordered fabrication build passes without design changes | Freeze source/BOM/Gerbers as Gateway rev 0.1 candidate release; prepare separate OSHWA application |
| Minor documentation-only corrections required | Update public record transparently; design may remain rev 0.1 |
| Electrical or layout correction required before first certification submission | Incorporate the correction into the first clean Gateway rev 0.1 release, document prototype history and regenerate outputs before submission |
| Post-certification design change required | Assign a later Gateway revision; do not silently replace a certified release |

## References

- Project record: `76 Engineering Log.md`, Gateway fabrication order and initial bring-up checklist.
- STMicroelectronics, `NUCLEO-N657X0-Q` product page and documentation: <https://www.st.com/en/evaluation-tools/nucleo-n657x0-q.html>
- Qorvo, `DWM3001CDK` product page and product brief: <https://www.qorvo.com/products/p/DWM3001CDK>
- Seeed Studio, `XIAO ESP32C6` getting-started guide and pin map: <https://wiki.seeedstudio.com/xiao_esp32c6_getting_started/>
- OSHWA Certification Documentation Guidance: <https://certification.oshwa.org/process/documentation.html>
