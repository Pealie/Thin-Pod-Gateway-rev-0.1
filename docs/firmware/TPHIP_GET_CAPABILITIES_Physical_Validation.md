# TPHIP GET_CAPABILITIES Physical Validation

**Project:** Thin-Pod Gateway rev 0.1
**Validation date:** 12 July 2026
**Interface:** STM32 NUCLEO-N657X0-Q to Gateway DWM3001-CDK
**Protocol:** Thin-Pod Host Interface Protocol, `GET_CAPABILITIES` proof
**Result:** PASS

## Purpose

This record consolidates the physical and firmware evidence for the first complete bidirectional Thin-Pod Host Interface Protocol exchange across the manufactured Gateway carrier PCB.

The validation concerns the carrier-board power, RESET, READY, SPI and test-access paths between the NUCLEO host and the Gateway DWM3001-CDK. It does not establish a DW3110 UWB RF exchange.

## Hardware under test

| Item | Identity |
|---|---|
| Carrier board | Thin-Pod Gateway rev 0.1 |
| Host | STM32 NUCLEO-N657X0-Q |
| Peripheral subsystem | Qorvo DWM3001-CDK |
| Power source | PoE splitter through NUCLEO USB-C sink/user input |
| NUCLEO power selector | 3â€“4, `5V_USB_SNK` |
| Host serial output | ST-LINK virtual COM port, 115200 8N1 |

## Interface mapping

| Signal | NUCLEO endpoint | DWM endpoint | Test point |
|---|---|---|---|
| SCK | PE15 | P0.31 / J10.23 | TP10 |
| COPI / MOSI | PG2 | P0.27 / J10.19 | TP11 |
| CIPO / MISO | PG1 | P0.07 / J10.21 | TP12 |
| Active-low CS | PA3 | P0.30 / J10.24 | TP7 |
| READY | PB9 | P0.28 / J10.15 | TP6 |
| RESET | PD0 | P0.18 / J10.12 | TP5 |

## Corrective action preceding validation

During bring-up, DWM J10.12 RESET measured approximately 2 V when released under PoE-only operation. The J10.12 solder connection was reflowed.

Following reflow:

```text
DWM J10.12 RESET to TP1: approximately 3.3 V
```

A static READY diagnostic then confirmed:

```text
DWM J10.15 to TP1: approximately 0 V / 3.3 V
TP6 to TP1:        approximately 0 V / 3.3 V
NUCLEO PB9 to TP1: approximately 0 V / 3.3 V
```

The matching levels at all three locations establish the complete READY route through the carrier PCB.

## Firmware configuration

### DWM endpoint

- Zephyr / nRF Connect SDK application
- nRF52833 SPI2 configured as SPIS
- 1 MHz, mode 0, MSB first
- 16-byte request transaction
- 32-byte response transaction
- READY asserted only after the asynchronous response transfer is armed
- fixed static buffers protected by guard words
- READY output readback retained as supported bring-up observability

Input sensing remains enabled on READY solely so firmware can read back the level being driven. Protocol behaviour does not depend on the readback result.

### NUCLEO host probe

- Zephyr application for `nucleo_n657x0_q`
- SPI5 at 1 MHz, mode 0, MSB first
- manual active-low chip select
- high-impedance DWM RESET release after a 10 ms low pulse
- READY-high and READY-low timeout checks
- complete 32-byte response capture
- hexadecimal `response_after_clock` dump retained as supported probe output
- response parser and guard-word validation

## Suite composition

Each complete suite performs:

1. Local sequence-mismatch parser self-test
2. Ten valid `GET_CAPABILITIES` exchanges
3. Unknown-version request
4. Reserved-flags request
5. Oversized-payload request
6. Bad-CRC request
7. Unknown-opcode request

The suite result is therefore 16 checks.

## Expected valid capability response

```text
protocol_major     1
protocol_minor     0
capability_flags   0x000c
max_payload_bytes  64
max_record_bytes   0
interface_build_id 0x00010001
```

The frozen sequence-1 response is:

```text
54 50 47 57 01 10 81 01 01 00 00 00 10 00 2d fe
01 00 0c 00 40 00 00 00 00 00 00 00 01 00 01 00
```

## Observed normal results

Sequences 1 through 10 all returned:

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

READY returned low after each response transaction.

## Observed adverse-request results

| Test | Expected | Observed | CRC | Guards | Result |
|---|---|---|---|---|---|
| Unknown version | `BAD_VERSION` | `BAD_VERSION` | OK | OK | PASS |
| Reserved flags | `BAD_FLAGS` | `BAD_FLAGS` | OK | OK | PASS |
| Oversized payload | `BAD_LENGTH` | `BAD_LENGTH` | OK | OK | PASS |
| Bad request CRC | `BAD_CRC` | `BAD_CRC` | OK | OK | PASS |
| Unknown opcode | `UNKNOWN_OPCODE` | `UNKNOWN_OPCODE` | OK | OK | PASS |

Each fully captured run concluded:

```text
suite=GET_CAPABILITIES passes=16 failures=0 guard=OK result=PASS
```

## Repeatability

Three PoE power-cycle procedures were carried out.

Because the PuTTY COM session became available only after board power-up, the start of each log contains:

```text
host_if_probe idle ready=0 suite_result=PASS
```

This records completion of the suite initiated at power-up. A manual NUCLEO reset then generated a complete second suite within each captured log.

| Log | Power-up suite | Captured reset suite |
|---|---|---|
| `nucleo_putty_sun2.log` | PASS | 16 passes, 0 failures |
| `nucleo_putty_sun3.log` | PASS | 16 passes, 0 failures |
| `nucleo_putty_sun4.log` | PASS | 16 passes, 0 failures |

`nucleo_putty_sun1.log` provides an additional complete passing record.

## Acceptance decision

**PASS**

The following acceptance conditions are met:

- DWM RESET releases at approximately 3.3 V.
- READY reaches valid low and high rail levels at DWM J10.15, Gateway TP6 and NUCLEO PB9.
- SCK, COPI, CS, READY and RESET function across the physical carrier.
- Complete 32-byte CIPO responses reach the NUCLEO.
- Ten consecutive valid requests pass per suite.
- All five adverse-request cases return the intended status.
- CRC, sequence and guard checks pass.
- The result repeats across three PoE power cycles.
- The host remains idle with READY low after completion.

## Claim boundary

This validation closes the NUCLEO-to-DWM host-interface and carrier pin-path milestone.

It does not claim:

- a real DW3110 UWB RF exchange;
- node-to-Gateway vibration-window transport;
- Gateway DSP or TinyML operation;
- production authentication or secure-boot enforcement;
- radio, EMC, electrical-safety, CE or UKCA compliance.

## Next milestone

Real DWM3001-CDK to DWM3001-CDK UWB RF proof.
