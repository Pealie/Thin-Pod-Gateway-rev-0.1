# Thin-Pod Gateway Telemetry Packet v1

## Purpose

This document defines the first fixed telemetry packet used by the Thin-Pod Gateway rev 0.1 completion work.

This document directly supports Gateway completion checklist item 4: one defined telemetry packet.

No new Gateway architecture scope is introduced here.

## Packet role

Telemetry Packet v1 is the minimum binary packet required to prove that a Thin-Pod node can transfer a defined measurement/status payload to the Gateway and that the Gateway-side tooling can parse and log the result.

The packet is intentionally small, fixed-length, and suitable for early UWB transport tests.

It is not a full production telemetry protocol.

## Byte order

All multi-byte integer fields are little-endian.

## Packet length

Telemetry Packet v1 is fixed length:

```text
24 bytes
```

## Binary layout

| Offset | Size | Field | Type | Description |
|---:|---:|---|---|---|
| 0 | 2 | magic | uint16 | Packet marker. ASCII `TP`, encoded as bytes `54 50`. |
| 2 | 1 | version | uint8 | Packet version. For this packet, value is `1`. |
| 3 | 1 | packet_type | uint8 | Packet type. `1` means telemetry. |
| 4 | 2 | node_id | uint16 | Sending node identifier. |
| 6 | 4 | sequence | uint32 | Monotonic packet sequence number from the node. |
| 10 | 4 | uptime_ms | uint32 | Node uptime in milliseconds. |
| 14 | 2 | battery_mv | uint16 | Node battery or supply estimate in millivolts. |
| 16 | 2 | sample_rms_mg | uint16 | Example vibration RMS value in mg. |
| 18 | 2 | sample_peak_mg | uint16 | Example vibration peak value in mg. |
| 20 | 2 | status_flags | uint16 | Bitfield for node or packet status. |
| 22 | 2 | crc16 | uint16 | CRC16 over bytes 0 to 21. |

## Packet type values

| Value | Meaning |
|---:|---|
| 1 | Telemetry |

## Status flags

For rev 0.1, status flags are reserved and may be set to zero.

| Bit | Meaning |
|---:|---|
| 0 | Reserved |
| 1 | Reserved |
| 2 | Reserved |
| 3 | Reserved |
| 4-15 | Reserved |

## Example decoded packet

| Field | Value |
|---|---:|
| magic | TP |
| version | 1 |
| packet_type | 1 |
| node_id | 1 |
| sequence | 42 |
| uptime_ms | 123456 |
| battery_mv | 3300 |
| sample_rms_mg | 125 |
| sample_peak_mg | 410 |
| status_flags | 0 |

## Example packet bytes before CRC

```text
54 50 01 01 01 00 2A 00 00 00 40 E2 01 00 E4 0C 7D 00 9A 01 00 00
```

## Example packet bytes with placeholder CRC

```text
54 50 01 01 01 00 2A 00 00 00 40 E2 01 00 E4 0C 7D 00 9A 01 00 00 XX XX
```

The final two bytes are the CRC16 value and should be calculated by the packet generator or parser.

## Parser expectations

A Gateway-side parser should:

- accept packet bytes in the fixed 24-byte layout;
- confirm the first two bytes match ASCII `TP`;
- confirm `version` is `1`;
- confirm `packet_type` is `1` for telemetry;
- decode all multi-byte fields as little-endian integers;
- calculate CRC16 over bytes 0 to 21 when CRC support is enabled;
- report whether the CRC is valid;
- emit either CSV or JSONL output for validation logging.

## JSON representation

A parsed packet may be represented as:

```json
{
  "magic": "TP",
  "version": 1,
  "packet_type": "telemetry",
  "node_id": 1,
  "sequence": 42,
  "uptime_ms": 123456,
  "battery_mv": 3300,
  "sample_rms_mg": 125,
  "sample_peak_mg": 410,
  "status_flags": 0,
  "crc_valid": true
}
```

## Rev 0.1 limitations

Telemetry Packet v1 is deliberately narrow. It does not yet define:

- full waveform transfer;
- FFT bin transfer;
- TinyML inference output;
- timestamp synchronisation between node and Gateway;
- multi-node scheduling;
- retransmission behaviour;
- production radio framing;
- security or authentication.

Those features may be added in later packet versions.


## Security classification and freeze condition

Telemetry Packet v1 is a laboratory parser and transport test vector.

The current fields have the following security meaning:

- `node_id` is an asserted identifier and is not cryptographically verified;
- `sequence` supports ordering observation but does not provide replay resistance without an authenticated sender, boot epoch and receiver replay state;
- `crc16` detects accidental corruption and does not provide sender authenticity or resistance to deliberate alteration.

Telemetry Packet v1 must not be described as secure, authenticated, replay-resistant or suitable for external deployment.

A secure successor must use a new packet version and satisfy `docs/security/Thin-Pod_Protocol_Security_Requirements.md`. At minimum, it must bind node identity, boot epoch or session identifier, sequence number, key identifier, acquisition context, payload length and payload to a cryptographic authentication tag. Security fields must not be retrofitted ambiguously into the reserved v1 status bits.

The v1 parser should retain separate future output fields for:

```text
crc_valid
authentication_status
replay_status
key_id
boot_epoch
```

For v1, only `crc_valid` can be populated from the packet. The other fields must remain `not_present` or equivalent.

## Completion checklist link

This document satisfies:

- Item 4: one defined telemetry packet

It also supports:

- Item 5: one node-to-Gateway transfer
- Item 6: one parser or logging script
- Item 7: one validation log
- Item 10: OSHWA certification preparation

## Revision history

| Date | Change |
|---|---|
| 2026-06-27 | Initial Telemetry Packet v1 definition |
| 2026-07-11 | Added explicit laboratory-only security classification and secure-successor gate |
