# Thin-Pod Gateway rev 0.1: DWM-to-NUCLEO Host-Interface Protocol

**Document identifier:** `TP-GW-IFP-0001`  
**Protocol designation:** `TPHIP-1` — Thin-Pod Host Interface Protocol, version 1 draft  
**Document revision:** Draft A  
**Gateway hardware:** Thin-Pod Gateway rev 0.1  
**Communications subsystem:** Gateway-side Qorvo DWM3001-CDK / DWM3001C nRF52833 firmware  
**Host subsystem:** STM32 NUCLEO-N657X0-Q  
**Document status:** Implementation contract for commissioning; not yet physically or firmware verified  
**Document date:** 27 May 2026  
**Certification boundary:** This protocol supports future Gateway development. It does not extend the submitted OSHWA scope of Thin-Pod rev 0.1.

## 1. Purpose

This document defines the first firmware-level host interface between the Gateway-side Qorvo DWM3001 subsystem and the STM32 NUCLEO-N657X0-Q.

The interface exists to move **complete, integrity-checked vibration-window records** from the communications subsystem into NUCLEO-owned memory without allowing uncontrolled data flow, silent record overwrite or premature dependence on DSP/TinyML behaviour.

The protocol is deliberately conservative. Its first duty is not high throughput; it is to establish that the Gateway rev 0.1 copper route, two firmware endpoints and window-ownership rules work repeatably.

## 2. System responsibility boundary

```text
Node-side Qorvo subsystem
    frames and transmits admitted vibration windows over UWB
        ↓
Gateway-side Qorvo subsystem
    receives / reassembles / validates UWB windows
    queues complete records temporarily
    exposes records through TPHIP-1
        ↓
NUCLEO-N657X0-Q
    grants receive capacity
    reads records into owned memory
    validates, stores and later processes raw windows
```

| Subsystem | Responsibility under this protocol | Explicit exclusion |
|---|---|---|
| Gateway DWM firmware | Communications termination, short record queue, status/counters, record presentation to host | Does not perform the Gateway DSP/TinyML role |
| NUCLEO firmware | SPI host transactions, admission control, persistent raw-window queue, DSP scheduling | Does not treat the CDK as a transparent direct-DW3110 radio breakout |
| XIAO ESP32-C6 | Optional later reporting/backhaul | Not involved in first TPHIP-1 proof and must not block ingress |

## 3. Hardware interface basis

Gateway rev 0.1 routes the following physical interface candidate between the NUCLEO and the DWM3001-CDK:

| Gateway net | NUCLEO endpoint | DWM3001-CDK endpoint | Protocol use |
|---|---|---|---|
| `SPI5_SCK` | `CN15.11 / PE15` | `J10.23 / SPI1_CLK` | SPI clock driven by NUCLEO |
| `SPI5_MISO` | `CN15.13 / PG1` | `J10.21 / SPI1_MISO` | Response/record data returned by DWM firmware |
| `SPI5_MOSI` | `CN15.15 / PG2` | `J10.19 / SPI1_MOSI` | Request data sent by NUCLEO |
| `DWM_CS` | `CN15.17 / PA3` | `J10.24 / CS_RPI` | Active-low transaction selection |
| `DWM_IRQ` | `CN15.16 / PB9` | `J10.15 / GPIO_RPI` | Record/status notification |
| `DWM_RESET` | `CN15.33 / PD0` | `J10.12 / RESET` | Controlled subsystem reset |

Qorvo identifies the DWM3001C as integrating its own nRF52833 BLE SoC and DW3110-based UWB hardware. Accordingly, the routed interface is treated as a host interface to firmware running on the DWM subsystem. TPHIP-1 does not assert direct NUCLEO control of the DW3110 radio.

## 4. Commissioning assumptions and verification gates

The following points are **proposed protocol settings**, not validated hardware facts until bring-up and firmware testing are complete.

| Item | Initial protocol decision | Verification action |
|---|---|---|
| SPI ownership | NUCLEO is SPI host/master; Gateway DWM firmware is SPI peripheral/slave | Implement synthetic-buffer proof |
| SPI bit order | MSB first | Confirm with captured first command/response |
| SPI mode | Mode 0 (`CPOL=0`, `CPHA=0`) initially | Confirm byte-accurate repeated transfers; change by versioned protocol decision only |
| Initial SPI clock | 1 MHz, rising to 2 MHz after first success | Increase only after integrity-tested repeated transfers |
| `DWM_CS` polarity | Active low | Scope/logic-analyser verification |
| `DWM_IRQ` policy | Active-high level signal initially | Firmware and scope verification; document final polarity |
| Reset policy | NUCLEO-controlled recovery line; not part of normal record flow | Validate idle state and controlled reset test |
| Maximum first record size | 4160 bytes | Golden-record transfer test |
| First payload chunk size | 256 bytes | Optimise later only after reliable commissioning |

If the DWM3001/nRF52833 firmware cannot provide the assumed SPI-peripheral interface on the exposed pins as intended, the interface architecture must be revised and recorded; the PCB route alone is not proof of a working firmware endpoint.

## 5. Protocol principles

TPHIP-1 obeys the following rules:

1. The NUCLEO initiates every SPI transaction and controls bulk-transfer timing.
2. The DWM subsystem retains ownership of any complete ready record until NUCLEO sends `ACK_WINDOW` or `REJECT_WINDOW`.
3. A record is not submitted to DSP until NUCLEO has completed host-side validation.
4. New window capacity is granted explicitly through credits.
5. Queue pressure produces credit withholding or explicit busy status, not silent overwrite.
6. CRC provides accidental-corruption detection; it is not an authenticity or security signature.
7. Protocol versioning is independent of hardware revision.

## 6. Data representation conventions

| Convention | TPHIP-1 definition |
|---|---|
| Integer byte order | Little-endian |
| Character tags | Literal ASCII byte order as shown |
| Boolean false / true | `0x00` / `0x01` |
| Unused/reserved fields | Transmit as zero; receiver ignores unless a later version defines them |
| CRC algorithm | CRC-32/ISO-HDLC, polynomial `0x04C11DB7` reflected form, initial value `0xFFFFFFFF`, final XOR `0xFFFFFFFF`; compatible with standard Ethernet/ZIP CRC-32 implementations |
| Record alignment target | Complete initial window record is 4160 bytes, divisible by 64 |
| Protocol maximum record length at first proof | 4160 bytes only; larger lengths rejected until versioned support is added |

## 7. Host request / response transaction model

A variable response cannot be assumed to exist at the instant the DWM firmware first receives a request opcode. TPHIP-1 therefore uses a two-phase request/response model.

```text
Phase 1: Host request
NUCLEO asserts DWM_CS
NUCLEO clocks a fixed 16-byte REQUEST_FRAME to DWM
NUCLEO deasserts DWM_CS

Phase 2: DWM preparation
DWM firmware validates request and prepares response
DWM asserts DWM_IRQ because RESPONSE_READY or another event is pending

Phase 3: Host response read
NUCLEO observes DWM_IRQ / known pending response
NUCLEO asserts DWM_CS
NUCLEO clocks the announced number of response bytes using dummy MOSI bytes
DWM returns RESPONSE_HEADER and optional response payload
NUCLEO deasserts DWM_CS
```

For commissioning, only one host request may be outstanding at a time. A later pipelined protocol would require a new protocol version or expressly compatible extension.

## 8. Request frame

Every host request is a fixed 16-byte structure.

### 8.1 Request frame layout

| Offset | Bytes | Field | Type | Definition |
|---:|---:|---|---|---|
| 0 | 2 | `magic` | ASCII bytes | `TP` (`0x54 0x50`) |
| 2 | 1 | `protocol_version` | `uint8_t` | `0x01` for TPHIP-1 |
| 3 | 1 | `opcode` | `uint8_t` | Command identifier |
| 4 | 2 | `transaction_id` | `uint16_t` | Host-generated sequence for request/response pairing |
| 6 | 2 | `flags` | `uint16_t` | Command-specific flags; zero initially unless defined |
| 8 | 4 | `arg0` | `uint32_t` | Command argument |
| 12 | 4 | `request_crc32` | `uint32_t` | CRC over request bytes 0–11 |

### 8.2 Golden request example: `GET_CAPABILITIES`

| Field | Value |
|---|---:|
| `magic` | `TP` |
| `protocol_version` | `0x01` |
| `opcode` | `0x01` (`GET_CAPABILITIES`) |
| `transaction_id` | `0x0001` |
| `flags` | `0x0000` |
| `arg0` | `0x00000000` |
| `request_crc32` | `0x080505DB` |

Complete request bytes:

```text
54 50 01 01 01 00 00 00 00 00 00 00 DB 05 05 08
```

## 9. Response frame

A response contains a fixed 24-byte response header followed by a command-specific payload of `payload_length` bytes. Its CRC covers the first 20 header bytes followed by the payload; the CRC field itself is excluded.

### 9.1 Response header layout

| Offset | Bytes | Field | Type | Definition |
|---:|---:|---|---|---|
| 0 | 2 | `magic` | ASCII bytes | `TR` (`0x54 0x52`) |
| 2 | 1 | `protocol_version` | `uint8_t` | `0x01` |
| 3 | 1 | `result_code` | `uint8_t` | Success or failure status |
| 4 | 1 | `opcode_echo` | `uint8_t` | Opcode being answered |
| 5 | 1 | `response_flags` | `uint8_t` | Ready/event state flags |
| 6 | 2 | `transaction_id` | `uint16_t` | Must match request transaction ID |
| 8 | 4 | `payload_length` | `uint32_t` | Bytes following the 24-byte header |
| 12 | 4 | `arg0` | `uint32_t` | Response-specific summary value |
| 16 | 4 | `arg1` | `uint32_t` | Response-specific summary value |
| 20 | 4 | `response_crc32` | `uint32_t` | CRC over bytes 0–19 plus payload bytes |

### 9.2 Result codes

| Value | Name | Meaning |
|---:|---|---|
| `0x00` | `OK` | Command accepted and completed |
| `0x01` | `ERR_BAD_MAGIC` | Request magic incorrect |
| `0x02` | `ERR_BAD_VERSION` | Protocol version unsupported |
| `0x03` | `ERR_BAD_CRC` | Request CRC failed |
| `0x04` | `ERR_BAD_OPCODE` | Opcode unsupported |
| `0x05` | `ERR_BAD_ARGUMENT` | Invalid argument, offset or length |
| `0x06` | `ERR_BUSY` | DWM cannot service command in current state |
| `0x07` | `ERR_NO_READY_RECORD` | Read/ACK requested without a completed record |
| `0x08` | `ERR_RECORD_MISMATCH` | Record/sequence identifier does not match oldest queued record |
| `0x09` | `ERR_INTERFACE_FAULT` | Internal host-interface fault; counters/status should be read |
| `0x0A` | `ERR_RESET_OCCURRED` | Record/session invalidated by reset |

### 9.3 Response flags

| Bit | Mask | Name | Meaning while set |
|---:|---:|---|---|
| 0 | `0x01` | `WINDOW_READY` | At least one complete validated record awaits host ownership |
| 1 | `0x02` | `RESPONSE_READY` | A command response has been prepared |
| 2 | `0x04` | `STATUS_EVENT_PENDING` | Non-fatal status/event has not yet been acknowledged |
| 3 | `0x08` | `ERROR_EVENT_PENDING` | Error counter/event requires host inspection |
| 4 | `0x10` | `NO_CREDITS` | New radio-window admission is withheld |
| 5 | `0x20` | `RX_IN_PROGRESS` | A UWB record is currently being assembled |
| 6 | `0x40` | `HOST_TRANSFER_ACTIVE` | Record is currently exposed for host read |
| 7 | `0x80` | Reserved | Zero in TPHIP-1 |

## 10. DWM_IRQ semantics

`DWM_IRQ` is intended as an active-high, level-signalled notification in TPHIP-1:

```text
DWM_IRQ = HIGH while:
    WINDOW_READY is set
    OR RESPONSE_READY is set
    OR STATUS_EVENT_PENDING is set
    OR ERROR_EVENT_PENDING is set
```

The signal is not a payload channel and should not be treated as an instruction to do substantial processing inside an interrupt handler.

### NUCLEO IRQ service behaviour

```text
GPIO interrupt occurs
        ↓
interrupt handler sets a DWM service event flag
        ↓
high-priority host-interface task issues GET_STATUS / reads prepared response
        ↓
task services record or status condition
        ↓
DWM deasserts IRQ only when no asserted conditions remain
```

A level-signalled policy prevents a busy host from missing a short pulse and losing knowledge that a validated record is waiting.

## 11. Command set

### 11.1 Opcode summary

| Opcode | Command | Purpose | Request `arg0` | Response payload |
|---:|---|---|---|---|
| `0x01` | `GET_CAPABILITIES` | Discover firmware/protocol limits | Zero | Fixed `CAPABILITIES` structure |
| `0x02` | `GET_STATUS` | Read current queue/interface/event state | Zero | Fixed `STATUS` structure |
| `0x03` | `SET_CREDITS` | Set permitted outstanding incoming windows | Credit count | Fixed `STATUS` structure |
| `0x10` | `READ_DESCRIPTOR` | Read 64-byte header of oldest ready window | Expected `window_sequence` or zero for oldest | 64-byte window header |
| `0x11` | `READ_PAYLOAD` | Read data chunk from oldest ready window | Packed offset/length selector, defined below | Payload chunk |
| `0x12` | `ACK_WINDOW` | Accept ownership and permit DWM slot release | Accepted `window_sequence` | Empty |
| `0x13` | `REJECT_WINDOW` | Reject record with reason and permit defined release | Rejected `window_sequence` | Empty; reason in flags/next event |
| `0x20` | `GET_COUNTERS` | Read diagnostic counters | Counter page number | Counter-page payload |
| `0x21` | `CLEAR_STATUS_EVENTS` | Acknowledge event indications | Event mask | Updated status |
| `0x7F` | `RESET_INTERFACE` | Reset host protocol state only where safe | Reset key / mode | Updated capability/status report |

### 11.2 `READ_PAYLOAD` argument packing

For first implementation, payload reads use fixed maximum 256-byte chunks. `arg0` is packed as:

| Bits | Field | Meaning |
|---:|---|---|
| 0–15 | `offset_bytes` | Payload offset from the start of the raw payload at record offset 64 |
| 16–31 | `length_bytes` | Requested chunk length; 1–256 bytes initially |

The request must not cross the current record payload boundary. Invalid ranges return `ERR_BAD_ARGUMENT`.

### 11.3 `SET_CREDITS` policy

`SET_CREDITS` sets the maximum number of additional complete incoming windows that the Gateway DWM is authorised to admit. During commissioning:

```text
valid credit values: 0 or 1
```

`SET_CREDITS(0)` withholds new admission while preserving already queued/ready records. It does not discard existing data.

## 12. Capability payload

`GET_CAPABILITIES` returns a fixed payload sufficient to reject incompatible firmware before data movement begins.

| Field | Type | Initial value or meaning |
|---|---:|---|
| `host_protocol_version` | `uint16_t` | `0x0001` |
| `window_record_version` | `uint16_t` | `0x0001` |
| `max_record_bytes` | `uint32_t` | `4160` |
| `max_payload_chunk_bytes` | `uint16_t` | `256` initially |
| `dwm_complete_window_slots` | `uint8_t` | `2` initially |
| `max_outstanding_credits` | `uint8_t` | `1` for commissioning |
| `dwm_firmware_build_id` | `uint32_t` | Build identifier |
| `feature_flags` | `uint32_t` | Support indicators for UWB record receive, counters, reset behaviour, etc. |
| `reserved` | bytes | Zero until defined |

## 13. Status payload

`GET_STATUS` and `SET_CREDITS` responses should return a compact status object.

| Field | Type | Meaning |
|---|---:|---|
| `state_flags` | `uint32_t` | Expanded state flags corresponding to IRQ/status conditions |
| `ready_record_count` | `uint8_t` | Complete host-ready records |
| `rx_working_count` | `uint8_t` | Records currently in reception/reassembly |
| `credits_remaining` | `uint8_t` | Incoming windows currently admitted but not completed |
| `free_dwm_slots` | `uint8_t` | Free complete-record slots |
| `oldest_ready_sequence` | `uint32_t` | Sequence identifier for host read/ACK |
| `last_error_code` | `uint32_t` | Most recent error/event code |
| `dwm_uptime_ticks` | `uint64_t` | Diagnostic timing value |
| `reserved` | bytes | Zero until defined |

## 14. Window record format

The record moved through the host interface is the same logical complete record validated after UWB reassembly or generated as a synthetic commissioning vector.

### 14.1 Initial record structure

| Offset | Bytes | Field | Type | Initial definition |
|---:|---:|---|---|---|
| 0 | 4 | `magic` | ASCII | `TPWN` |
| 4 | 2 | `record_version` | `uint16_t` | `0x0001` |
| 6 | 2 | `message_type` | `uint16_t` | See table below |
| 8 | 2 | `header_bytes` | `uint16_t` | `64` |
| 10 | 2 | `sample_format` | `uint16_t` | `0x0001` = signed 16-bit little-endian |
| 12 | 4 | `node_id` | `uint32_t` | Logical source node ID |
| 16 | 4 | `window_sequence` | `uint32_t` | Ordered record number |
| 20 | 4 | `sample_rate_hz` | `uint32_t` | Sampling metadata |
| 24 | 4 | `sample_count` | `uint32_t` | Initially `2048` |
| 28 | 4 | `payload_bytes` | `uint32_t` | Initially `4096` |
| 32 | 4 | `sensor_id` | `uint32_t` | Sensor/channel identity; zero for synthetic vector |
| 36 | 4 | `calibration_version` | `uint32_t` | Zero means uncharacterised/not assigned |
| 40 | 4 | `acquisition_flags` | `uint32_t` | Bit flags |
| 44 | 8 | `node_tick` | `uint64_t` | Source acquisition tick/time marker |
| 52 | 4 | `node_firmware_build_id` | `uint32_t` | Node generation/capture build ID |
| 56 | 4 | `record_total_bytes` | `uint32_t` | Initially `4160` |
| 60 | 4 | `record_crc32` | `uint32_t` | CRC over header bytes 0–59 followed by payload bytes 64–4159; bytes 60–63 excluded |
| 64 | 4096 | `raw_samples` | `int16_t[2048]` | Sample payload |

The complete initial record is exactly:

```text
64-byte aligned header + 4096-byte payload = 4160 bytes
```

### 14.2 Message types

| Value | Name | Use |
|---:|---|---|
| `0x0001` | `SYNTHETIC_WINDOW` | Locally generated byte-accurate interface test |
| `0x0002` | `UWB_TEST_WINDOW` | Known record transported over UWB |
| `0x0003` | `VIBRATION_WINDOW` | Actual externally sensed vibration window |
| `0x0010` | `STATUS_RECORD` | Structured non-measurement status |
| `0x0011` | `ERROR_RECORD` | Structured fault record |

### 14.3 Acquisition flags

| Bit | Mask | Flag | Meaning |
|---:|---:|---|---|
| 0 | `0x00000001` | `SYNTHETIC` | Payload is a generated test pattern |
| 1 | `0x00000002` | `SCHEDULED_CAPTURE` | Window acquired in a scheduled slot |
| 2 | `0x00000004` | `EVENT_TRIGGERED` | Window associated with a local trigger |
| 3 | `0x00000008` | `CLIPPED` | Acquisition detected clipping/saturation |
| 4 | `0x00000010` | `CALIBRATION_UNKNOWN` | Calibration state not established |
| 5 | `0x00000020` | `TEST_SOURCE` | Measurement input is a test/shaker/injected source |
| 6 | `0x00000040` | `INCOMPLETE_SOURCE` | Source acquisition reported missing samples; record must not be treated as valid measurement |
| 7–31 | — | Reserved | Zero in TPHIP-1 unless later defined |

## 15. Golden synthetic record

The first interface proof uses a deterministic complete record generated inside Gateway DWM firmware, before UWB is involved.

### 15.1 Golden record fields

| Field | Value |
|---|---:|
| `magic` | `TPWN` |
| `record_version` | `1` |
| `message_type` | `1` (`SYNTHETIC_WINDOW`) |
| `header_bytes` | `64` |
| `sample_format` | `1` (`INT16_LE`) |
| `node_id` | `1` |
| `window_sequence` | `1` |
| `sample_rate_hz` | `25600` metadata test value only |
| `sample_count` | `2048` |
| `payload_bytes` | `4096` |
| `sensor_id` | `0` |
| `calibration_version` | `0` |
| `acquisition_flags` | `0x00000001` (`SYNTHETIC`) |
| `node_tick` | `0` |
| `node_firmware_build_id` | `1` |
| `record_total_bytes` | `4160` |
| `record_crc32` | `0x0B1D48B2` |

### 15.2 Golden payload generator

The raw payload is a deterministic signed 16-bit little-endian ramp:

```python
samples = [(i % 1024) - 512 for i in range(2048)]
payload = b"".join(struct.pack("<h", sample) for sample in samples)
```

First sixteen payload bytes:

```text
00 FE 01 FE 02 FE 03 FE 04 FE 05 FE 06 FE 07 FE
```

Last sixteen payload bytes:

```text
F8 01 F9 01 FA 01 FB 01 FC 01 FD 01 FE 01 FF 01
```

### 15.3 Golden CRC calculation

The CRC field is calculated over header bytes 0–59 concatenated directly with the 4096 payload bytes at offsets 64–4159. The four CRC-storage bytes at offsets 60–63 are excluded.

Reference generator:

```python
import struct
import zlib

header_without_crc = struct.pack(
    "<4sHHHHIIIIIIIIQII",
    b"TPWN",       # magic
    1,             # record_version
    1,             # SYNTHETIC_WINDOW
    64,            # header_bytes
    1,             # INT16_LE
    1,             # node_id
    1,             # window_sequence
    25600,         # sample_rate_hz: metadata test value only
    2048,          # sample_count
    4096,          # payload_bytes
    0,             # sensor_id
    0,             # calibration_version
    1,             # SYNTHETIC flag
    0,             # node_tick
    1,             # node_firmware_build_id
    4160,          # record_total_bytes
)

samples = [(i % 1024) - 512 for i in range(2048)]
payload = b"".join(struct.pack("<h", sample) for sample in samples)

crc = zlib.crc32(header_without_crc + payload) & 0xFFFFFFFF
record = header_without_crc + struct.pack("<I", crc) + payload

assert len(record) == 4160
assert crc == 0x0B1D48B2
```

Any firmware implementation failing to generate or receive this exact record and CRC has not passed the first TPHIP-1 interchange test.

## 16. Record ownership and acknowledgement

### 16.1 Ownership transitions

```text
DWM owns record:
    RX_IN_PROGRESS → READY_FOR_HOST → HOST_TRANSFER_IN_PROGRESS

NUCLEO obtains ownership only after:
    complete descriptor/payload read
    + host-side CRC accepted
    + sequence/length checks accepted
    + ACK_WINDOW transmitted successfully

After ACK_WINDOW:
    DWM frees local slot
    NUCLEO record state becomes VALID_RAW
```

### 16.2 Reject behaviour

`REJECT_WINDOW` must include a diagnostic reason in a later status/event report. A rejected record is not a valid DSP input. Depending on commissioning mode, the DWM may release the slot after the rejection has been recorded.

## 17. Credit and overrun-control behaviour

For commissioning, the permitted credit state is binary:

| Credits | Meaning |
|---:|---|
| `0` | Do not admit a new incoming measurement/test window |
| `1` | One additional complete incoming window may be admitted |

The NUCLEO grants credit only if its raw-record buffer policy permits future ownership. The Gateway DWM must not admit a second incoming window solely because its radio is able to receive it.

When capacity is exhausted:

```text
DWM status sets NO_CREDITS
Node admission is withheld or answered busy
Already accepted complete records are retained until ACK/REJECT
No VALID_RAW NUCLEO record is silently overwritten
```

## 18. Reset and recovery behaviour

| Event | Required TPHIP-1 behaviour |
|---|---|
| NUCLEO resets while DWM retains an unacknowledged ready record | On reconnect, NUCLEO reads status; either re-reads/ACKs record or explicitly rejects after logging recovery state |
| DWM resets before ACK | NUCLEO treats in-flight record as not safely committed; records reset/failure event |
| DWM resets after NUCLEO accepted record | NUCLEO-owned `VALID_RAW` record remains usable with reset event attached to session log |
| Corrupt request CRC | DWM returns `ERR_BAD_CRC`; no state-changing command performed |
| Corrupt response CRC | NUCLEO rejects response, does not ACK record and retries/status-checks according to policy |
| Unsupported version | No data movement; log compatibility fault |
| Host-service timeout | DWM stops new admission and records timeout/error status |

`RESET_INTERFACE` is not a radio reset command. It is a controlled host-protocol recovery function whose exact scope must be recorded in firmware documentation.

## 19. First implementation sequence

| Phase | Implementation step | Pass criterion |
|---:|---|---|
| 1 | Implement request/response frame parsing and `GET_CAPABILITIES` | Golden request accepted and valid capabilities response read repeatedly |
| 2 | Implement `GET_STATUS`, IRQ state and event clearing | Host can explain IRQ state at all times |
| 3 | Generate golden synthetic record in DWM firmware | Local DWM CRC matches `0x0B1D48B2` |
| 4 | Implement descriptor and 256-byte chunk reads | NUCLEO reconstructs exact 4160-byte record |
| 5 | Implement `ACK_WINDOW` / record retention | Withheld ACK prevents DWM overwrite/freeing |
| 6 | Implement credit state `0/1` | No second synthetic/admitted record appears without credit |
| 7 | Run 100 synthetic transfers | Zero CRC/sequence errors |
| 8 | Extend record source to UWB test record | Known RF-received record reaches NUCLEO memory intact |
| 9 | Extend source to actual vibration window | Valid raw measurement record reaches NUCLEO memory |
| 10 | Permit DSP consumer | Derived result linked to retained raw record |

## 20. Firmware evidence required

For each completed implementation stage, record:

| Evidence | Required content |
|---|---|
| Firmware source revision | Repository commit/tag/build identifier for NUCLEO and DWM firmware |
| Protocol version | TPHIP version and any compatible extension flags |
| Hardware identity | Gateway PCB revision/build and fitted commercial module identities |
| Interface settings | SPI mode, frequency, IRQ polarity and payload chunk size |
| Golden-vector outcome | Byte equality and CRC result |
| Logic capture | At least one representative CS/SCK/MOSI/MISO/IRQ transaction capture where possible |
| Error counters | Counter snapshot before and after test |
| Pass/fail result | Explicit acceptance decision and anomalies |

## 21. Revision control

| Change | Treatment |
|---|---|
| Clarification that does not affect wire format or behaviour | Document revision update |
| New optional command that does not alter existing command interpretation | Compatible feature flag plus document revision |
| Any change to request/response frame layout, CRC coverage, record layout or ownership semantics | New protocol version |
| PCB route change | Gateway hardware revision decision, independent of protocol version |
| Node acquisition-format change while preserving record format | Update metadata/source definitions; protocol change only if wire representation changes |

## 22. References

- Qorvo, [DWM3001C product information](https://www.qorvo.com/products/p/DWM3001C).
- Qorvo, [DWM3001CDK development kit product information](https://www.qorvo.com/products/p/DWM3001CDK).
- Nordic Semiconductor, [nRF52833 product information](https://www.nordicsemi.com/Products/nRF52833).
- STMicroelectronics, [STM32N657X0 product information](https://www.st.com/en/microcontrollers-microprocessors/stm32n657x0.html).
- `docs/data-path-flow-control-and-buffering.md`.
- `docs/system-interface-control-document.md`.
- `docs/gateway-bring-up-and-verification-protocol.md`.

## 23. Engineering decision

The first Gateway DWM-to-NUCLEO data path is defined as a NUCLEO-mastered, SPI/IRQ, credit-controlled, complete-record interface. Its first mandatory proof is the exact transfer and validation of the 4160-byte golden synthetic record. Until that proof passes, UWB receipt, vibration sampling, DSP and TinyML remain downstream intentions rather than verified Gateway behaviour.
