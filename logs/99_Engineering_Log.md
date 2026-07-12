# 99 Engineering Log

**Date:** 11–12 July 2026
**Repository:** `Pealie/Thin-Pod-Gateway-rev-0.1`
**Branch:** `gateway-dwm-spis-get-capabilities`
**Scope:** Completion of the PoE-powered NUCLEO-to-DWM TPHIP `GET_CAPABILITIES` physical host-interface proof

## Session summary

The Thin-Pod Gateway rev 0.1 NUCLEO-to-DWM host interface has completed its first repeatable bidirectional physical protocol proof.

The investigation began with a deterministic two-transaction phase slip. The DWM3001-CDK received valid 16-byte requests and constructed valid 32-byte responses, although the NUCLEO did not recognise the READY assertion reliably. Each following 16-byte request then clocked only half of the pending response, producing:

```text
response_callback_result=16
response_complete ret=-122
```

The failure was traced through the RESET and READY hardware paths. A marginal solder connection at DWM J10.12 held the DWM RESET net at approximately 2 V under PoE-only power. Reflowing J10.12 restored the released RESET level to approximately 3.3 V.

A temporary static READY-toggle diagnostic then demonstrated valid rail-level signalling at every accessible point on the physical READY route:

```text
DWM J10.15    approximately 0 V / 3.3 V
Gateway TP6   approximately 0 V / 3.3 V
NUCLEO PB9    approximately 0 V / 3.3 V
```

The TPHIP SPIS endpoint was restored and rebuilt with the supported READY readback diagnostic retained. The existing NUCLEO host-interface probe was retained with its complete hexadecimal response dump. The resulting tested source therefore matches the diagnostic behaviour present in the captured validation evidence.

## Hardware configuration

| Item | Test configuration |
|---|---|
| Gateway carrier | Thin-Pod Gateway rev 0.1 physical fabrication build |
| Main host | STM32 NUCLEO-N657X0-Q |
| UWB subsystem | Qorvo DWM3001-CDK |
| Power | PoE splitter into NUCLEO USB-C sink/user connector |
| NUCLEO selector | Pins 3–4, `5V_USB_SNK` |
| DWM power | Gateway carrier 5 V path to DWM J10.2 |
| NUCLEO console | ST-LINK virtual COM port, 115200 8N1 |
| Host bus | SPI5, mode 0, 1 MHz, MSB first |
| Request length | 16 bytes |
| Response length | 32 bytes |

## Corrective action

The DWM J10.12 RESET solder connection was reflowed with flux and minimal fresh solder.

Before corrective action:

```text
DWM J10.12 RESET to TP1: approximately 2.0 V
READY path: approximately 0.5 V to 0.8 V during attempted assertion
```

After corrective action:

```text
DWM J10.12 RESET to TP1: approximately 3.3 V
DWM J10.15 READY:        approximately 0 V / 3.3 V
Gateway TP6 READY:       approximately 0 V / 3.3 V
NUCLEO PB9 READY:        approximately 0 V / 3.3 V
```

The observed change establishes the J10.12 joint as the principal physical cause of the marginal DWM start-up and invalid READY levels.

## Supported diagnostic source retained

Two diagnostic changes are retained as supported behaviour in the rev 0.1 bring-up applications.

### DWM READY output readback

The DWM READY pin remains configured with input sensing enabled while operating as an output. Input sensing is used solely to read back the physical logic level driven by the endpoint for bring-up observability.

The endpoint reports:

```text
tphip_dwm ready_set_ret=<return value> ready_physical=<0 or 1>
```

Protocol flow does not depend on this readback value.

### NUCLEO complete response dump

The NUCLEO host-interface probe retains the complete 32-byte hexadecimal response dump:

```text
response_after_clock=54 50 47 57 ...
```

This output permits direct comparison between captured physical responses and the frozen TPHIP test vectors. The application is a validation probe, so this detailed output forms part of its intended observability.

## TPHIP validation result

Each complete suite contains:

- one local valid-CRC sequence-mismatch parser self-test;
- ten valid `GET_CAPABILITIES` exchanges with sequences 1 through 10;
- one unknown-version test;
- one reserved-flags test;
- one oversized-payload test;
- one bad-CRC test;
- one unknown-opcode test.

The ten valid exchanges returned:

```text
status=OK
crc=OK
protocol=1.0
flags=0x000c
max_payload=64
max_record=0
build_id=0x00010001
guard=OK
result=PASS
```

The adverse requests returned the required statuses:

| Test | Required status | Observed status |
|---|---|---|
| Unknown version | `BAD_VERSION` | `BAD_VERSION` |
| Reserved flags | `BAD_FLAGS` | `BAD_FLAGS` |
| Oversized payload | `BAD_LENGTH` | `BAD_LENGTH` |
| Bad request CRC | `BAD_CRC` | `BAD_CRC` |
| Unknown opcode | `UNKNOWN_OPCODE` | `UNKNOWN_OPCODE` |

Every fully captured suite concluded:

```text
suite=GET_CAPABILITIES passes=16 failures=0 guard=OK result=PASS
```

## Repeatability evidence

Three independent PoE power-cycle procedures were completed.

PuTTY could attach only after the boards were powered. Each power-cycle log therefore begins with repeated:

```text
host_if_probe idle ready=0 suite_result=PASS
```

This records that the power-up suite had completed successfully before console capture began. A manual NUCLEO reset was then applied, and each log captured a second complete suite from boot banner through final PASS.

| Evidence record | Power-up state | Fully captured rerun |
|---|---|---|
| `nucleo_putty_sun2.log` | `suite_result=PASS` visible on attachment | 16 passes, 0 failures |
| `nucleo_putty_sun3.log` | `suite_result=PASS` visible on attachment | 16 passes, 0 failures |
| `nucleo_putty_sun4.log` | `suite_result=PASS` visible on attachment | 16 passes, 0 failures |

An earlier complete passing record is also present in `nucleo_putty_sun1.log`.

The evidence supports three successful PoE power-up executions and three fully captured NUCLEO-reset reruns with identical protocol results.

## Physical path established

The completed proof covers:

```text
NUCLEO SPI5 SCK / COPI / CS
    →
Gateway carrier routing and test access
    →
DWM3001-CDK SPIS request reception
    →
TPHIP request validation and response construction
    →
DWM READY assertion and CIPO response transfer
    →
Gateway carrier READY / CIPO routing
    →
NUCLEO response reception, CRC validation and decoding
```

The following carrier routes are now physically exercised:

| Function | NUCLEO | DWM3001-CDK | Gateway test point |
|---|---|---|---|
| SCK | PE15 | J10.23 / P0.31 | TP10 |
| COPI / MOSI | PG2 | J10.19 / P0.27 | TP11 |
| CIPO / MISO | PG1 | J10.21 / P0.07 | TP12 |
| CS | PA3 | J10.24 / P0.30 | TP7 |
| READY | PB9 | J10.15 / P0.28 | TP6 |
| RESET | PD0 | J10.12 / P0.18 | TP5 |

## Milestone conclusion

The PoE-powered Thin-Pod Gateway has completed three independent power-cycle validations of the NUCLEO-to-DWM TPHIP `GET_CAPABILITIES` interface.

Each validation demonstrated:

- stable DWM RESET release at approximately 3.3 V;
- valid READY low and high rail levels across J10.15, TP6 and PB9;
- ten consecutive valid bidirectional SPI exchanges;
- complete 32-byte CIPO responses;
- request and response CRC validation;
- sequence correlation;
- expected adverse-request handling;
- intact request and response buffer guards;
- zero suite failures.

The NUCLEO host-interface and physical pin-path confirmation milestone is closed.

## Evidence boundary

Established:

- PoE power reaches the NUCLEO and DWM through the intended Gateway arrangement.
- The DWM RESET path releases correctly after J10.12 reflow.
- READY is physically valid from DWM P0.28 through J10.15 and TP6 to NUCLEO PB9.
- The NUCLEO sends valid 16-byte requests over the physical SPI path.
- The DWM validates requests and constructs the expected responses.
- The DWM returns complete 32-byte responses over CIPO.
- The NUCLEO validates magic, version, flags, lengths, opcode, sequence, status and CRC.
- All normal and adverse host-interface tests pass.
- Three PoE power-up executions and three fully captured reruns pass.

Pending:

- real DWM-to-DWM DW3110 UWB RF exchange;
- node-to-Gateway vibration-window transport;
- Gateway buffering, DSP and later TinyML validation;
- any later production security, secure-boot or regulatory claims.

## Next Gateway milestone

The next gated Gateway activity is:

> Real DWM3001-CDK to DWM3001-CDK UWB RF proof using the staged node responder and Gateway initiator roles.

No RF, vibration-transport, DSP, TinyML, production-security or regulatory claim is made from the completed host-interface proof.
