# DWM-to-NUCLEO Host Interface Protocol

## Document control

**Document identifier:** TP-GW-PROT-0001
**Document revision:** Draft A.2
**Product context:** Thin-Pod Gateway rev 0.1
**Document status:** Firmware-enabling protocol contract
**Related architecture:** `docs/architecture/Gateway_UWB_Module_Access_Model.md`
**Certification boundary:** Supports Gateway rev 0.1 open-hardware packaging by documenting the modular DWM3001-CDK-to-NUCLEO interface. It does not claim direct NUCLEO ownership of the DW3110 silicon.

---

## 1. Purpose

This document specifies the first DWM-to-NUCLEO host-interface contract for Thin-Pod Gateway rev 0.1.

The purpose is to convert the Gateway rev 0.1 modular access model into an implementable byte-level interface between:

```text
DWM3001-CDK UWB coprocessor/module
    and
NUCLEO-N657X0-Q Gateway supervisor
```

The first proof is deliberately small:

```text
GET_CAPABILITIES
GET_STATUS
GET_COUNTERS
```

This proof establishes that the NUCLEO can communicate with Gateway-side DWM firmware through a defined host-interface path. It does not attempt UWB window transfer, DSP, TinyML or direct DW3110 register ownership.

---

## 2. Architectural position

Gateway rev 0.1 treats the DWM3001-CDK as an intelligent UWB coprocessor/module.

```text
DWM3001-CDK:
    owns DW3110 radio control
    runs UWB role firmware
    exposes a host-interface contract to the NUCLEO

NUCLEO-N657X0-Q:
    acts as SPI host/controller
    requests status, counters and later descriptors/payloads
    owns Gateway-side memory, logging and analysis responsibilities

Gateway carrier PCB:
    provides the modular open-hardware interconnect layer
```

Direct NUCLEO-to-DW3110 register probing remains diagnostic bring-up evidence. This protocol is the primary Gateway rev 0.1 host-interface route.

---

## 3. Electrical role model

Initial electrical role:

```text
NUCLEO-N657X0-Q:
    SPI host/controller

DWM3001-CDK firmware:
    SPI peripheral/target

DWM_IRQ:
    level-signalled notification from DWM side to NUCLEO

DWM_RESET:
    optional host-controlled reset line, used only in defined recovery flows
```

Signal naming:

```text
SCK:
    NUCLEO-generated SPI clock

COPI / MOSI:
    Controller Out, Peripheral In
    NUCLEO to DWM

CIPO / MISO:
    Controller In, Peripheral Out
    DWM to NUCLEO

CS:
    NUCLEO-controlled active-low chip select

IRQ:
    DWM-controlled notification line
```

Initial SPI settings for proof:

```text
Mode:        SPI mode 0
Bit order:   MSB first
Word size:   8 bit
Frequency:   1 MHz initial
CS:          asserted low for the complete frame
Endian:      little-endian for multi-byte integer fields
```

Frequency may be increased only after repeated integrity-tested transfer.

---

## 4. Transfer model

The interface uses a simple two-phase SPI transaction model.

SPI is host-clocked. The DWM peripheral cannot send an unsolicited response unless the NUCLEO clocks it out. Therefore each command uses:

```text
Phase 1: request transaction
    NUCLEO sends request frame.
    DWM receives command and prepares response.
    CIPO bytes during this phase are ignored or treated as previous/idle response.

Phase 2: response transaction
    DWM asserts IRQ when response is ready.
    NUCLEO clocks a response frame by sending zero-fill bytes.
    DWM returns the prepared response frame on CIPO.
```

For the minimal proof, each command may be executed synchronously with a fixed short delay before the response read if IRQ is not yet implemented. IRQ should be added immediately after the first command/response loop is proven.

---

## 5. Common frame format

All request and response frames begin with a 16-byte header.

```text
Offset  Size  Field
0       4     magic
4       1     version
5       1     header_len
6       1     opcode
7       1     flags
8       2     sequence
10      2     status_or_reserved
12      2     payload_len
14      2     crc16
```

Field definitions:

```text
magic:
    ASCII "TPGW"

version:
    0x01 for this protocol version

header_len:
    0x10

opcode:
    command or response opcode

flags:
    bit 0: response frame
    bit 1: error frame
    bits 2-7: reserved, must be zero for Draft A

sequence:
    little-endian command sequence number.
    Response uses the same sequence as the request.

status_or_reserved:
    request: 0x0000
    response: status code

payload_len:
    little-endian payload length in bytes

crc16:
    CRC-16/CCITT-FALSE over header with crc16 field set to 0x0000,
    followed by payload bytes.
```

CRC parameters:

```text
Name:    CRC-16/CCITT-FALSE
Poly:    0x1021
Init:    0xffff
RefIn:   false
RefOut:  false
XorOut:  0x0000
```

---

## 6. Status codes

```text
0x0000 OK
0x0001 BUSY
0x0002 BAD_MAGIC
0x0003 BAD_VERSION
0x0004 BAD_LENGTH
0x0005 BAD_CRC
0x0006 UNKNOWN_OPCODE
0x0007 NOT_READY
0x0008 INTERNAL_ERROR
0x0009 BAD_FLAGS
```


### 6.1 Frozen GET_CAPABILITIES transaction lengths

The first hardware proof uses fixed physical transaction lengths:

```text
request transaction:   16 bytes
response transaction:  32 bytes
```

A successful `GET_CAPABILITIES` response has a 16-byte header and a 16-byte
payload. An error response has a 16-byte logical frame, sets `payload_len` to
zero, and zero-fills physical bytes 16 through 31. In both cases the NUCLEO
clocks exactly 32 response bytes. CRC covers the logical frame length only.

The DWM endpoint must reject:

```text
unknown protocol versions
reserved request flags
non-zero request reserved field
payload lengths greater than 64
non-zero GET_CAPABILITIES payload lengths
invalid CRC values
unknown opcodes
```

---

## 7. Minimal command set

### 7.1 GET_CAPABILITIES

Request:

```text
opcode:      0x01
payload_len: 0
```

Response:

```text
opcode:      0x81
status:      0x0000 on success
payload_len: 16
```

Payload:

```text
Offset  Size  Field
0       1     protocol_major
1       1     protocol_minor
2       2     capability_flags
4       4     max_payload_bytes
8       4     max_record_bytes
12      4     interface_build_id
```

Initial values for this proof:

```text
protocol_major:     1
protocol_minor:     0
capability_flags:   0x000c
                    bit2 = IRQ supported
                    bit3 = CRC supported
max_payload_bytes:  64
max_record_bytes:   0
interface_build_id: 0x00010001
```

Status and counters capability bits remain clear until those command handlers
exist. `max_record_bytes` remains zero until record transfer is implemented.

### 7.2 GET_STATUS

Request:

```text
opcode:      0x02
payload_len: 0
```

Response:

```text
opcode:      0x82
status:      0x0000 on success
payload_len: 16
```

Payload:

```text
Offset  Size  Field
0       4     uptime_ms
4       1     role
5       1     backend
6       1     interface_state
7       1     last_error
8       4     last_sequence_seen
12      4     reserved
```

Initial enum values:

```text
role:
    0 = unknown
    1 = gateway_initiator
    2 = node_responder
    3 = test_stub

backend:
    0 = unknown
    1 = stub
    2 = uwb_live
    3 = loopback

interface_state:
    0 = booting
    1 = ready
    2 = busy
    3 = error
```

For the current Stage-1 Gateway initiator/stub state, expected proof values are:

```text
role:    gateway_initiator
backend: stub
state:   ready
```

### 7.3 GET_COUNTERS

Request:

```text
opcode:      0x03
payload_len: 0
```

Response:

```text
opcode:      0x83
status:      0x0000 on success
payload_len: 32
```

Payload:

```text
Offset  Size  Field
0       4     poll_count
4       4     packet_count
8       4     crc_error_count
12      4     bad_command_count
16      4     busy_count
20      4     reset_count
24      4     last_rx_sequence
28      4     last_tx_sequence
```

For the first proof, `poll_count` and `packet_count` may be generated from the existing Stage-1 stub activity.

---

## 8. GET_CAPABILITIES proof acceptance criteria

The first proof passes when all of the following are observed:

```text
request physical length is exactly 16 bytes
response physical length is exactly 32 bytes
response sequence matches the outstanding request sequence
response CRC validates
10 valid GET_CAPABILITIES exchanges pass in each cold boot
unknown version returns BAD_VERSION
reserved flags return BAD_FLAGS
oversized payload returns BAD_LENGTH
bad CRC returns BAD_CRC
unknown opcode returns UNKNOWN_OPCODE
local valid-CRC sequence-mismatch parser test is rejected
all static-buffer guard words remain intact
READY asserts only after the response transfer is armed
READY returns low after every response transfer
```

The hardware acceptance target is three cold power cycles with 30 of 30 valid
exchanges, zero CRC failures, zero sequence mismatches, zero guard failures and
zero READY timeouts.

---

## 9. Files proposed for implementation

Recommended firmware additions:

```text
firmware/dwm3001cdk_host_interface_stub/
firmware/nucleo_dwm_host_interface_probe/
```

Recommended documentation/evidence additions:

```text
docs/dwm-to-nucleo-host-interface-protocol.md
logs/99_Engineering_Log.md
logs/DWM_to_NUCLEO_Host_Interface_Proof_*.log
```

---

## 10. Implementation sequence

Recommended order:

```text
1. Commit this protocol document.
2. Implement DWM-side host-interface stub using the current Stage-1 Gateway role values.
3. Implement NUCLEO-side host-interface probe.
4. Prove GET_CAPABILITIES.
5. Prove GET_STATUS.
6. Prove GET_COUNTERS.
7. Add bad-opcode and bad-CRC diagnostics.
8. Record the result in Engineering Log 97.
```

---

## 11. Relationship to later window transfer

This minimal proof does not transfer a vibration window.

Later commands may extend the protocol with:

```text
READ_DESCRIPTOR
READ_PAYLOAD
ACK_WINDOW
REJECT_WINDOW
SET_CREDITS
RESET_INTERFACE
```

Those commands should remain out of scope until the three minimal proof commands are working repeatably.

---

## 12. Closeout position

This protocol is the first concrete firmware bridge for the adopted Gateway rev 0.1 architecture.

It proves the modular access model:

```text
DWM3001-CDK owns UWB/radio-side behaviour.
NUCLEO communicates with the DWM subsystem through a defined host interface.
Gateway PCB supplies the open-hardware modular interconnect layer.
```
---

## 13. Security freeze condition

The Draft A host-interface protocol remains a laboratory bring-up contract.

Its CRC-16/CCITT-FALSE field detects accidental corruption. It does not authenticate the DWM firmware, the originating node or the record payload, and it does not provide replay resistance.

Before this interface is designated suitable for external deployment:

1. the requirements in `docs/security/Thin-Pod_Protocol_Security_Requirements.md` must be allocated to a new protocol revision;
2. a transferred measurement descriptor must preserve node identity, boot epoch, UWB/application sequence, key identifier, authentication result, replay result, acquisition configuration identity and complete-record length;
3. control commands that modify state must require an authorised security context;
4. the parser must reject unknown versions, reserved flags, oversized lengths, invalid state transitions and unmatched request/response sequences;
5. future status semantics must distinguish `BAD_CRC`, `AUTH_FAILED`, `UNKNOWN_KEY`, `REPLAY_DUPLICATE`, `REPLAY_TOO_OLD` and `STALE_EPOCH`;
6. deterministic valid and adverse test vectors must be committed;
7. host-interface fuzzing and recovery tests must pass within fixed memory limits.

The existing `GET_CAPABILITIES`, `GET_STATUS` and `GET_COUNTERS` commands remain valid for the immediate firmware proof. Their success does not establish a secure deployment channel.

## 14. Revision history

| Date | Revision | Change |
|---|---|---|
| 2026-07-11 | Draft A.1 | Added protocol-security freeze conditions and explicit CRC-versus-authentication boundary |
| 2026-07-11 | Draft A.2 | Froze GET_CAPABILITIES transaction lengths, BAD_FLAGS status, initial capability values and proof acceptance criteria |