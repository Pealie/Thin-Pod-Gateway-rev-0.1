# Thin-Pod TPHIP-1 Golden Window Vector Metadata

**Vector file:** `golden-window-v1.bin`  
**Reference generator/verifier:** `crc-reference.py`  
**Record designation:** `TPHIP-1` golden `SYNTHETIC_WINDOW` record  
**Record version:** `1`  
**Generation date:** 27 May 2026  
**Status:** Canonical deterministic commissioning vector  
**Measurement status:** Synthetic protocol test data; not a physical vibration measurement  

## Purpose

This vector is the first common implementation artefact for the Thin-Pod Gateway firmware path. It exists to test whether Gateway-side DWM firmware can present a complete deterministic record and whether NUCLEO host firmware can read, validate, acknowledge and preserve that record without corruption or silent overwrite.

It is suitable for:

- local reference verification;
- Gateway-DWM record-generation testing;
- NUCLEO `TPHIP-1` record-receipt testing;
- CRC-failure and withheld-acknowledgement tests; and
- later regression testing.

It is not evidence of:

- ADXL1005 digital acquisition;
- UWB record transmission;
- a calibrated vibration measurement;
- DSP performance; or
- TinyML operation.

## Canonical file identity

| Property | Value |
|---|---:|
| File name | `golden-window-v1.bin` |
| Binary length | `4160` bytes |
| Header length | `64` bytes |
| Raw payload length | `4096` bytes |
| Record CRC-32 | `0x0B1D48B2` |
| SHA-256 | `9145a9612896dfe063b028355bd0282223b478713c7e6877cad6cb3b44df0c06` |

## CRC definition

The vector uses **CRC-32/ISO-HDLC**, as produced by standard ZIP/Ethernet-compatible implementations such as Python `zlib.crc32`.

CRC coverage is:

```text
header bytes 0–59
followed immediately by
payload bytes 64–4159
```

The stored CRC field at record offsets 60–63 is excluded from its own calculation. It is written little-endian into the binary record as:

```text
B2 48 1D 0B
```

## Header field definition

| Offset | Bytes | Field | Value | Meaning |
|---:|---:|---|---:|---|
| 0 | 4 | `magic` | ASCII `TPWN` | Thin-Pod Window record identity |
| 4 | 2 | `record_version` | `1` | Record format version |
| 6 | 2 | `message_type` | `1` | `SYNTHETIC_WINDOW` |
| 8 | 2 | `header_bytes` | `64` | Fixed record-header length |
| 10 | 2 | `sample_format` | `1` | Signed 16-bit little-endian |
| 12 | 4 | `node_id` | `1` | Synthetic source identity |
| 16 | 4 | `window_sequence` | `1` | First deterministic record |
| 20 | 4 | `sample_rate_hz` | `25600` | Protocol-test metadata only |
| 24 | 4 | `sample_count` | `2048` | Number of payload samples |
| 28 | 4 | `payload_bytes` | `4096` | Size of raw payload |
| 32 | 4 | `sensor_id` | `0` | No physical sensor; synthetic |
| 36 | 4 | `calibration_version` | `0` | Not applicable |
| 40 | 4 | `acquisition_flags` | `1` | `SYNTHETIC` |
| 44 | 8 | `node_tick` | `0` | No acquisition timestamp |
| 52 | 4 | `node_firmware_build_id` | `1` | Deterministic test marker |
| 56 | 4 | `record_total_bytes` | `4160` | Complete binary size |
| 60 | 4 | `record_crc32` | `0x0B1D48B2` | Integrity field stored little-endian |

The `sample_rate_hz` value is deterministic metadata for testing the wire format. It must not be interpreted as a qualified ADXL1005 acquisition sample rate.

## Payload generation rule

The 4096-byte payload consists of 2048 signed 16-bit little-endian values:

```python
samples = [(i % 1024) - 512 for i in range(2048)]
payload = b"".join(struct.pack("<h", sample) for sample in samples)
```

This produces two successive ramps from `-512` through `511`.

| Payload location | Hex bytes |
|---|---|
| First 16 bytes | `00 FE 01 FE 02 FE 03 FE 04 FE 05 FE 06 FE 07 FE` |
| Last 16 bytes | `F8 01 F9 01 FA 01 FB 01 FC 01 FD 01 FE 01 FF 01` |

## Verification commands

Generate or regenerate the canonical binary record from this directory:

```powershell
python .\crc-reference.py generate
```

Verify the checked-in binary record:

```powershell
python .\crc-reference.py verify .\golden-window-v1.bin
```

Expected verification result:

```text
PASS: length: 4160 bytes
PASS: embedded CRC-32: 0x0B1D48B2
PASS: calculated CRC-32: 0x0B1D48B2
PASS: SHA-256: 9145a9612896dfe063b028355bd0282223b478713c7e6877cad6cb3b44df0c06
PASS: canonical byte equality: exact match
```

## Intended firmware tests

| Firmware location | Test role |
|---|---|
| `firmware/gateway-dwm/tphip-golden-record/` | DWM firmware exposes or constructs this record and retains it until acknowledged |
| `firmware/nucleo-host/tphip-reader/` | NUCLEO reconstructs and validates the received record against this vector |
| `docs/data-path-fault-injection-and-recovery-tests.md` | Uses modified/blocked variants to prove error visibility and ownership behaviour |

## Software-licence note

`crc-reference.py` is creator-authored software-support material. Before this script or operational firmware is publicly released as a maintained software component of the Gateway repository, a software licence should be chosen and recorded in `LICENSE-SOFTWARE.md`.
