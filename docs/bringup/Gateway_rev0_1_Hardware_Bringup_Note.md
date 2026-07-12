# Thin-Pod Gateway rev 0.1 Hardware Bring-Up Note

## Purpose

This note records stable hardware bring-up evidence for the Thin-Pod Gateway rev 0.1 PCB.

It supports Gateway completion checklist item 1, the stable hardware bring-up note, and provides supporting evidence for the validation log and OSHWA preparation.

No new Gateway architecture scope is introduced here.

## Board under test

| Item | Detail |
|---|---|
| Project | Thin-Pod Gateway |
| Hardware revision | rev 0.1 |
| Physical fabrication identifier | rev 0.1f where present on the PCB |
| Main controller | STM32 NUCLEO-N657X0-Q |
| UWB module | Qorvo DWM3001-CDK |
| Wi-Fi / auxiliary module | Seeed XIAO ESP32-C6 |
| Power source | PoE splitter into NUCLEO USB-C sink/user input |
| NUCLEO power selector | Pins 3–4, `5V_USB_SNK` |
| Test state | Populated bench bring-up |
| Validation date | 12 July 2026 |

## Bring-up objective

The bring-up objective is to confirm that the manufactured Gateway PCB:

- accepts the intended PoE-derived 5 V supply arrangement;
- distributes power to the attached development modules;
- provides a stable common ground;
- releases the DWM reset input correctly;
- carries the DWM READY signal at valid rail levels;
- supports a complete bidirectional NUCLEO-to-DWM host-interface exchange;
- operates without temporary jumper wiring after solder-joint correction.

## Assembly issues discovered and corrected

### DWM ground return

Initial bring-up found unreliable DWM power unless a temporary jumper was fitted between the CDK battery-ground connection and Gateway ground.

The issue was associated with the fitted DWM header ground path, including a marginal connection around J10.25. Reflow restored stable common ground, and the temporary jumper was removed.

### DWM RESET connection

During TPHIP host-interface validation, DWM J10.12 RESET measured approximately 2 V when released under PoE-only operation. This level was insufficiently close to the 3.3 V logic rail and coincided with invalid READY levels of approximately 0.5 V to 0.8 V.

The J10.12 solder connection was reflowed. The released RESET level then measured approximately 3.3 V.

A subsequent static READY diagnostic produced clean alternating levels at:

```text
DWM J10.15    approximately 0 V / 3.3 V
Gateway TP6   approximately 0 V / 3.3 V
NUCLEO PB9    approximately 0 V / 3.3 V
```

## Power checks

| Test point / location | Expected | Observed | Result |
|---|---:|---:|---|
| NUCLEO CN3.6 to TP1 | approximately 5 V | approximately 5 V | PASS |
| DWM J10.2 to TP1 | approximately 5 V | approximately 5 V | PASS |
| NUCLEO CN3.16 to TP1 | approximately 3.3 V | approximately 3.3 V | PASS |
| DWM J10.12 RESET to TP1 after reflow | approximately 3.3 V | approximately 3.3 V | PASS |
| Gateway ground to DWM ground pins | continuity | continuity confirmed | PASS |
| Gateway ground to XIAO ground | continuity | continuity confirmed | PASS |

The XIAO is outside the measurement-critical path. Its full onward-networking function remains deferred.

## Host-interface checks

| Check | Expected | Observed | Result |
|---|---|---|---|
| READY low level at J10.15, TP6 and PB9 | approximately 0 V | approximately 0 V | PASS |
| READY high level at J10.15, TP6 and PB9 | approximately 3.3 V | approximately 3.3 V | PASS |
| Ten valid `GET_CAPABILITIES` exchanges | all pass | all pass | PASS |
| Unknown version | `BAD_VERSION` | `BAD_VERSION` | PASS |
| Reserved flags | `BAD_FLAGS` | `BAD_FLAGS` | PASS |
| Oversized payload | `BAD_LENGTH` | `BAD_LENGTH` | PASS |
| Bad CRC | `BAD_CRC` | `BAD_CRC` | PASS |
| Unknown opcode | `UNKNOWN_OPCODE` | `UNKNOWN_OPCODE` | PASS |
| Buffer guards | intact | `guard=OK` | PASS |
| Suite summary | 16 passes, 0 failures | 16 passes, 0 failures | PASS |
| Three PoE power-cycle procedures | suite passes after each cycle | confirmed | PASS |

## Functional observations

| Observation | Expected | Observed | Result |
|---|---|---|---|
| NUCLEO powers from PoE splitter | normal boot | normal boot | PASS |
| DWM powers from Gateway PCB | powers without jumper | confirmed | PASS |
| DWM firmware executes after PoE power-up | stable execution | confirmed | PASS |
| DWM red LED diagnostic behaviour | firmware-dependent activity | flashing observed during static diagnostic | PASS |
| READY returns low after response | low idle state | confirmed | PASS |
| NUCLEO host remains in idle loop after suite | stable PASS state | confirmed | PASS |
| Unexpected heating | absent | none observed | PASS |
| Brownout or repeated reset loop | absent | none observed after J10.12 repair | PASS |

## Stable bring-up conclusion

The Thin-Pod Gateway rev 0.1 carrier board has completed stable PoE-powered bring-up for the NUCLEO and Gateway DWM3001-CDK.

The board distributes the required power rails, provides stable ground continuity and supports valid RESET, READY and bidirectional SPI paths. Following reflow of the marginal DWM J10.12 RESET joint, the released RESET level reached approximately 3.3 V and READY reached valid 0 V and 3.3 V levels across DWM J10.15, Gateway TP6 and NUCLEO PB9.

The TPHIP `GET_CAPABILITIES` suite completed with:

```text
suite=GET_CAPABILITIES passes=16 failures=0 guard=OK result=PASS
```

Three PoE power-cycle procedures produced successful power-up suites and fully captured passing reruns after NUCLEO reset.

The NUCLEO-to-DWM host-interface and physical carrier pin-path milestone is closed.

## Remaining verification boundary

Pending work includes:

- real DWM-to-DWM DW3110 UWB RF exchange;
- node-to-Gateway vibration-window transport;
- Gateway buffering, DSP and TinyML evaluation;
- optional XIAO onward-networking validation;
- later production-security and regulatory work.

## Completion checklist link

This document satisfies:

- Item 1: stable hardware bring-up note.

It provides supporting evidence for:

- Item 3: minimal SPI/GPIO and host-interface probe;
- Item 7: validation log;
- Item 10: OSHWA certification preparation.

## Revision history

| Date | Change |
|---|---|
| 2026-06-27 | Initial bring-up note created |
| 2026-07-12 | Replaced provisional entries with measured PoE, RESET, READY and complete TPHIP host-interface evidence |
