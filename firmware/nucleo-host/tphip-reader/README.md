# TPHIP Reader Implementation Workspace

**Directory:** `firmware/nucleo-host/tphip-reader/`  
**Protocol:** `TPHIP-1`  
**Purpose:** Implement and verify the NUCLEO-side reader for complete records exposed by Gateway-side DWM firmware  
**Status:** Implementation plan prepared; code and test evidence pending  
**Document date:** 27 May 2026

## Purpose

This directory contains the NUCLEO-side implementation work for the first firmware-defined link between the Gateway communications subsystem and the analytic supervisor.

The reader is responsible for:

```text
observe DWM_IRQ / poll status safely
        ↓
issue TPHIP-1 commands as SPI host/master
        ↓
allocate NUCLEO-owned raw-record slot
        ↓
read complete record
        ↓
validate length, record version, sequence and CRC
        ↓
ACK or REJECT explicitly
        ↓
publish accepted record to downstream raw-data queue
```

It must never accept corrupt or incomplete data as a valid DSP input.

## Controlling references

| Document / asset | Use |
|---|---|
| `../../../docs/dwm-to-nucleo-host-interface-protocol.md` | Wire-format and command contract |
| `../../../docs/data-path-flow-control-and-buffering.md` | Queue and credit policy |
| `../../../docs/data-path-fault-injection-and-recovery-tests.md` | Required reader failure tests |
| `../../../docs/measurement-record-and-provenance-model.md` | Accepted-record identity and evidence requirements |
| `../../test-vectors/README.md` | Golden record expectation |
| `../../gateway-dwm/tphip-golden-record/README.md` | DWM-side complementary proof |

## First required implementation subset

| Functionality | Required behaviour |
|---|---|
| SPI host initialisation | Conservative commissioning configuration; exact mode/rate recorded |
| `DWM_CS` control | Clean active transaction boundaries |
| `DWM_IRQ` handling | Short ISR/event flag only; work deferred to host service task |
| `DWM_RESET` control | Safe idle state and deliberate recovery action only |
| Request frame generation | Fixed 16-byte `TPHIP-1` request with request CRC |
| Response reading | Validate response magic/version/transaction ID/CRC |
| Capabilities/status commands | Read interface identity and reason for pending event |
| Descriptor/payload read | Reconstruct initial 4160-byte record in allocated slot |
| Record CRC validation | Compare with expected/embedded result |
| ACK/reject | Transfer ownership explicitly |
| Counters/logging | Preserve failures and queue-pressure events |

## Proposed source-module separation

The actual project layout should follow the STM32 toolchain selected for the NUCLEO, but logical separation should be maintained:

```text
tphip-reader/
├── README.md
├── src/
│   ├── tphip_transport.c        # SPI transaction framing and CRC
│   ├── tphip_client.c           # command-level API
│   ├── raw_window_pool.c        # 32-slot ownership/state control
│   ├── dwm_irq_service.c        # IRQ/event hand-off
│   └── tphip_diagnostics.c      # counters/status logging
├── include/
│   ├── tphip_protocol.h
│   ├── raw_window_record.h
│   └── raw_window_pool.h
├── tests/
└── evidence/
```

Names are indicative rather than fixed until the initial STM32 project is created.

## Protocol constants to implement first

| Constant | Initial value |
|---|---:|
| Host protocol version | `1` (`TPHIP-1`) |
| Maximum record bytes | `4160` |
| Header bytes | `64` |
| Raw payload bytes | `4096` |
| Samples per golden record | `2048` |
| Payload chunk bytes | `256` initially |
| Golden record CRC | `0x0B1D48B2` |
| Raw-record slots | `32` |
| Protected free-slot reserve | `4` |
| First credits permitted | `0` or `1` |

## Host-service state machine

```text
IDLE
  ↓ DWM_IRQ asserted or scheduled status poll
STATUS_PENDING
  ↓ GET_STATUS confirms record available and FREE slot exists
ALLOCATE_SLOT
  ↓ slot transitions FREE → FILLING
READ_DESCRIPTOR
  ↓ descriptor valid and within initial protocol limits
READ_PAYLOAD
  ↓ all chunks received
VALIDATE_RECORD
  ↓ length/version/CRC/sequence accepted
SEND_ACK
  ↓ successful host ownership
PUBLISH_VALID_RAW
  ↓ downstream consumer notified
IDLE
```

Failure branches:

```text
no FREE slot / high-water policy:
    SET_CREDITS(0), retain status, log backpressure

invalid response / CRC / descriptor / record:
    REJECT_WINDOW where safe, mark INVALID_QUARANTINED, log event

reset or transaction timeout:
    invalidate FILLING slot, enter interface recovery, never publish as VALID_RAW
```

## API-level intent

A compact host API may expose operations similar to:

```c
bool tphip_init(const tphip_config_t *config);
bool tphip_get_capabilities(tphip_capabilities_t *out);
bool tphip_get_status(tphip_status_t *out);
bool tphip_set_credits(uint8_t credits);
bool tphip_read_ready_record(raw_window_slot_t *slot);
bool tphip_ack_window(uint32_t sequence);
bool tphip_reject_window(uint32_t sequence, tphip_reject_reason_t reason);
bool tphip_get_counters(tphip_counters_t *out);
void tphip_service_pending_event(void);
```

These are design-intent names only until code exists. The important aspect is that transfer, record ownership and downstream processing remain separable.

## Raw-window-pool policy

| Slot state | Reader responsibility |
|---|---|
| `FREE` | May be allocated for an admitted record |
| `FILLING` | Owned exclusively by host transfer; unavailable to DSP |
| `VALID_RAW` | Fully validated; may be passed to DSP/logging |
| `PROCESSING` | Downstream consumer owns read access |
| `PROCESSED_RETAIN` | Record retained as evidence |
| `INVALID_QUARANTINED` | Failure record retained/diagnosed but never sent as valid DSP input |
| `RELEASED` | Deliberate transition back to free state |

A validated raw record must not be overwritten merely because a newer record is waiting. Queue pressure causes credit withdrawal.

## Golden-record test

The first reader proof is defined precisely:

| Step | Action | Expected result |
|---:|---|---|
| 1 | Read `GET_CAPABILITIES` | Reports compatible TPHIP-1 and max record size 4160 |
| 2 | Read `GET_STATUS` after DWM prepares record | Reports one ready golden record and asserted IRQ reason |
| 3 | Allocate one raw slot | Slot becomes `FILLING` |
| 4 | Read descriptor | Header identifies `SYNTHETIC_WINDOW`, sequence `1` |
| 5 | Read payload in 256-byte chunks | Record reconstructs to exactly 4160 bytes |
| 6 | Validate CRC | Result equals `0x0B1D48B2` |
| 7 | Send `ACK_WINDOW` | DWM-side slot releases; host slot becomes `VALID_RAW` |
| 8 | Compare with shared vector | Received bytes equal `golden-window-v1.bin` |

Required repeatability milestone:

```text
100 consecutive accepted golden-record transfers
with zero unexplained CRC, framing or sequence failure.
```

## Mandatory fault tests

The reader is incomplete until these behaviours are shown:

| Test | Reader outcome required |
|---|---|
| DWM record pending but NUCLEO withholds ACK | No misleading local acceptance; DWM record remains unresolved |
| Bad record CRC | Slot becomes invalid/quarantined; no DSP publication |
| Malformed record length | Reject safely without writing outside allocated record buffer |
| Unsupported protocol version | Reject interface/data rather than misparse |
| Sequence gap or duplicate | Record/log condition explicitly |
| Raw queue at high-water level | Issue zero/no further credits before overwrite |
| DWM reset during transfer | Invalidate `FILLING` slot and recover explicitly |
| Optional XIAO reporting delay | Reader service remains higher priority |

## Timing evidence to capture

| Timing measure | Why required |
|---|---|
| IRQ asserted to service-task start | Host responsiveness |
| Status/descriptor read time | Protocol overhead |
| Payload read time | SPI/data-path budget |
| Validation and ACK time | Ownership latency |
| Complete record ready to `VALID_RAW` time | Gateway ingress latency |
| Queue occupancy versus credit action | Overrun policy proof |

## Evidence folder

```text
tphip-reader/
├── README.md
└── evidence/
    └── <test-session-id>/
        ├── build-and-settings.md
        ├── status-counters-before.json
        ├── status-counters-after.json
        ├── received-golden-window.bin
        ├── crc-and-byte-comparison.md
        ├── timing-results.csv
        ├── fault-results.md
        └── traces/
```

## Completion criterion

This reader workspace reaches first completion when it can:

1. identify a compatible Gateway DWM TPHIP-1 endpoint;
2. reconstruct and validate the exact golden record;
3. ACK only after valid ownership;
4. withhold credits under pressure; and
5. refuse to publish invalid records to a downstream DSP consumer.

After that, the same reader should accept a known UWB test record and then a real ADXL1005-derived characterisation window without changing the fundamental ownership rules.
