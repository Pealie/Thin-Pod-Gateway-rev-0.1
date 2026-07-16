# Gateway DWM Firmware: UWB Communications Coprocessor and Host Record Interface

**Firmware area:** `firmware/gateway-dwm/`<br>
**Hardware target:** Gateway-side Qorvo DWM3001-CDK / DWM3001C nRF52833 on Thin-Pod Gateway rev 0.1<br>
**Status:** Host-interface architecture specified; implementation and hardware verification pending<br>
**Document date:** 27 May 2026<br>
**Primary role:** Present complete validated records to the NUCLEO through a firmware-defined host interface

## Purpose

This directory is for firmware running on the **Gateway-side Qorvo DWM3001 subsystem**.

The Gateway DWM firmware is not intended to be a passive radio breakout controlled directly by the NUCLEO. It is intended to function as a communications coprocessor:

```text
later: receive framed UWB records from Thin-Pod node
        ↓
reassemble and validate complete bounded window
        ↓
queue a short number of complete records
        ↓
signal the NUCLEO that a record/status event is ready
        ↓
serve the record over the TPHIP-1 host interface
        ↓
release it only after NUCLEO ACK or explicit rejection
```

The first implementation does not begin with radio reception. It begins with a deterministic synthetic record presented to the NUCLEO over the same host interface that a later UWB-received record will use.

## Hardware-interface boundary

Gateway rev 0.1 routes the candidate host-interface signals between the NUCLEO and the Gateway DWM3001-CDK:

| Gateway net | NUCLEO endpoint | DWM3001-CDK endpoint | Firmware intent |
|---|---|---|---|
| `SPI5_SCK` | `CN15.11 / PE15` | `J10.23 / SPI1_CLK` | NUCLEO-driven host-interface clock |
| `SPI5_MISO` | `CN15.13 / PG1` | `J10.21 / SPI1_MISO` | DWM-to-host response/record data |
| `SPI5_MOSI` | `CN15.15 / PG2` | `J10.19 / SPI1_MOSI` | Host requests/commands |
| `DWM_CS` | `CN15.17 / PA3` | `J10.24 / CS_RPI` | Host transaction selection |
| `DWM_IRQ` | `CN15.16 / PB9` | `J10.15 / GPIO_RPI` | Record/status-ready notification |
| `DWM_RESET` | `CN15.33 / PD0` | `J10.12 / RESET` | Controlled recovery |

The intended commissioning role is:

```text
NUCLEO:               SPI host/master and authoritative admission controller
Gateway DWM firmware: SPI peripheral/slave and communications coprocessor
DWM_IRQ:              level-signalled record/status notification
```

This requires firmware implementation and verification. The routed copper does not alone prove the host protocol.

## Certification and licence boundary

Thin-Pod Gateway rev 0.1 is OSHWA-certified as `UK000092`, separately from the Thin-Pod rev 0.1 node certified as `UK000091`. Project-authored firmware in this repository is licensed under MIT and forms part of the declared Gateway software record; the certified hardware object remains the creator-designed carrier PCB.

`LICENSE-SOFTWARE.md` applies MIT to Thin-Pod-authored firmware and software. External Qorvo SDK material, vendor firmware and other third-party components remain under their original terms.

## Controlling protocol and architecture documents

| Document | Firmware dependency |
|---|---|
| `../../docs/dwm-to-nucleo-host-interface-protocol.md` | `TPHIP-1` byte-level request/response and golden-vector contract |
| `../../docs/data-path-flow-control-and-buffering.md` | Window admission, queue states, credit and overrun policy |
| `../../docs/data-path-fault-injection-and-recovery-tests.md` | Required failure evidence |
| `../../docs/window-throughput-and-airtime-budget.md` | Calculated/measurement targets |
| `../../docs/measurement-record-and-provenance-model.md` | Required build/transport/ingestion records |
| `../../docs/system-interface-control-document.md` | Subsystem responsibility split |

## Directory structure

```text
gateway-dwm/
├── README.md
└── tphip-golden-record/
    └── README.md
```

A later implementation layout may become:

```text
gateway-dwm/
├── README.md
├── tphip-golden-record/
│   ├── README.md
│   └── evidence/
├── app/
│   ├── tphip/
│   ├── record-queue/
│   ├── status-counters/
│   └── uwb-receiver/
├── tests/
└── build-notes/
```

## Immediate firmware role: host-interface proof

The first Gateway DWM firmware must not depend on UWB reception. It should:

1. boot and expose a `TPHIP-1` capability/status record;
2. implement the DWM-side SPI-peripheral endpoint where the firmware/toolchain permits;
3. implement active, level-signalled `DWM_IRQ` semantics;
4. construct or load the fixed golden `SYNTHETIC_WINDOW` record;
5. retain that record in a complete-record slot until host acknowledgement; and
6. prove that credit/withheld-ack behaviour does not cause overwrite.

### First record target

| Item | Value |
|---|---:|
| Record type | `SYNTHETIC_WINDOW` |
| Complete record size | 4160 bytes |
| Payload | 2048 signed 16-bit ramp samples |
| Expected record CRC-32 | `0x0B1D48B2` |
| DWM record slots | 2 initially |
| Commissioning outstanding credits | 0 or 1 |

## DWM-side state model

### Record-slot states

```text
FREE
  ↓ synthetic record generation or later admitted UWB record begins
RX_IN_PROGRESS
  ↓ complete record passes integrity checks
READY_FOR_HOST
  ↓ NUCLEO begins descriptor/payload read
HOST_TRANSFER_IN_PROGRESS
  ↓ NUCLEO ACKs valid ownership
FREE
```

Fault path:

```text
RX_IN_PROGRESS or HOST_TRANSFER_IN_PROGRESS
  ↓ CRC error, timeout, reset or host rejection
ERROR_QUARANTINED
  ↓ event/counter recorded and explicit release
FREE
```

### Ownership rule

A completed `READY_FOR_HOST` record must not be silently replaced or freed while NUCLEO ownership remains unresolved. This is the central behaviour to prove in the first firmware tests.

## TPHIP-1 minimum implementation

The first usable firmware subset should implement:

| Command | Required at first wired proof? | Purpose |
|---|---:|---|
| `GET_CAPABILITIES` | Yes | Confirm protocol version, record size and queue capability |
| `GET_STATUS` | Yes | Explain ready/credit/error state and IRQ reason |
| `SET_CREDITS` | Yes | Permit or withhold one additional admitted record |
| `READ_DESCRIPTOR` | Yes | Return record header |
| `READ_PAYLOAD` | Yes | Return payload in 256-byte commissioning chunks |
| `ACK_WINDOW` | Yes | Release a successfully owned record |
| `REJECT_WINDOW` | Yes | Record host rejection without processing it as valid data |
| `GET_COUNTERS` | Recommended before fault tests | Make errors and busy events visible |
| `CLEAR_STATUS_EVENTS` | Recommended before fault tests | Clear acknowledged event signals |
| `RESET_INTERFACE` | Later after baseline works | Controlled host-protocol recovery |

Initial electrical/protocol assumptions are provisional until verified: NUCLEO-mastered SPI, Mode 0, MSB-first, active-low chip select, active-high level `DWM_IRQ`, initial clock at 1 MHz and 256-byte payload chunks.

## Counters to implement

| Counter | Why required |
|---|---|
| `host_records_ready` | Confirms records exposed for host ownership |
| `host_records_acked` | Confirms accepted transfer |
| `host_records_rejected` | Confirms invalid/rejected transfer is visible |
| `host_service_timeouts` | Confirms unavailable/slow host does not cause silent loss |
| `uwb_windows_granted` | Required when UWB admission begins |
| `uwb_windows_started` | Required when UWB receive/reassembly begins |
| `uwb_windows_completed_valid` | Confirms valid completed RF record |
| `uwb_windows_crc_failed` | Confirms corrupt RF record visibility |
| `uwb_windows_timed_out` | Confirms incomplete fragment visibility |
| `uwb_busy_responses` | Confirms capacity protection |
| `dwm_resets_detected` | Supports recovery/provenance |

## Staged implementation plan

| Stage | Work | Pass criterion |
|---:|---|---|
| GW-DWM-0 | Establish firmware build/flash/debug route and identify build | Firmware runs and version can be recorded |
| GW-DWM-1 | Expose stable capability/status object | NUCLEO or temporary host can read stable response |
| GW-DWM-2 | Add synthetic record generation and correct local CRC | Golden record CRC equals `0x0B1D48B2` |
| GW-DWM-3 | Serve descriptor and payload over `TPHIP-1` | NUCLEO reconstructs exact record |
| GW-DWM-4 | Retain until `ACK_WINDOW`; test withheld ACK | No overwrite/free before policy permits |
| GW-DWM-5 | Implement credit/busy behaviour | `SET_CREDITS(0)` prevents new admission |
| GW-DWM-6 | Implement counters and fault events | Injected faults become readable status |
| GW-DWM-7 | Add known UWB test-record reception | Known RF record enters same host path |
| GW-DWM-8 | Add actual node record receipt | Actual measurement record reaches NUCLEO ownership |

## What not to implement first

Do not allow the Gateway DWM effort to become dominated initially by:

- complete UWB application design before the host hand-off works;
- DSP or TinyML on the DWM firmware;
- optional XIAO networking;
- uncontrolled streaming;
- multiple Pods;
- high-throughput optimisation; or
- final security architecture beyond preserving clear integrity/provenance boundaries.

The communications subsystem should first demonstrate one correct, retained, acknowledged record.

## Evidence required

Every completed host-interface firmware stage should record:

| Evidence | Content |
|---|---|
| DWM hardware identity | CDK/module instance and Gateway wiring/PCB build used |
| Firmware build identity | Commit/tag/build ID and development environment |
| Host settings | SPI mode/frequency/chunk size/IRQ polarity |
| Record identity | Message type, sequence, expected CRC |
| Status/counters | Before and after test |
| Binary comparison | Received bytes or hash/CRC match |
| Trace evidence | Logic-analyser or scope capture where available |
| Result | Explicit pass/fail plus anomalies |

## Immediate next action

Create the first Gateway DWM firmware project only far enough to expose build identity and a fixed capability/status response. Once that is stable, implement the golden synthetic record path and test it against the NUCLEO host reader. UWB reception belongs after the record hand-off boundary is proven.
