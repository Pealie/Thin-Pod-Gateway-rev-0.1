# Thin-Pod Shared Firmware Test Vectors

**Directory:** `firmware/test-vectors/`  
**Purpose:** Store deterministic binary records, metadata and verification scripts shared by node, Gateway DWM and NUCLEO firmware tests  
**Status:** Test-vector contract defined; vector files to be generated and committed during firmware implementation  
**Document date:** 27 May 2026  
**Protocol basis:** `TPHIP-1`

## Purpose

This directory is the single shared source of truth for deterministic data-path verification.

The first vector proves an essential fact before sensor acquisition or UWB transport is involved:

> A complete known raw-window record created or exposed by Gateway-side DWM firmware can be read into NUCLEO memory and verified byte-for-byte under the `TPHIP-1` record contract.

Every implementation should use the same binary input and expected integrity result. No firmware endpoint should be considered interoperable merely because it transfers a similar-looking buffer.

## Planned contents

```text
test-vectors/
├── README.md
├── golden-window-v1.bin
├── golden-window-v1-metadata.md
└── crc-reference.py
```

| File | Role |
|---|---|
| `golden-window-v1.bin` | Canonical 4160-byte binary `SYNTHETIC_WINDOW` record |
| `golden-window-v1-metadata.md` | Human-readable identity, field layout, CRC and hash record for the vector |
| `crc-reference.py` | Deterministic generator and validator for the binary record and CRC |

The README file defines the vector now. The binary vector, metadata document and script should be generated and committed as the next firmware-preparation task.

## Golden vector definition

### Complete record

| Quantity | Value |
|---|---:|
| Record version | `1` |
| Message type | `SYNTHETIC_WINDOW` (`1`) |
| Header size | `64` bytes |
| Sample representation | Signed 16-bit little-endian |
| Sample count | `2048` |
| Payload size | `4096` bytes |
| Complete record size | `4160` bytes |
| Expected CRC-32 | `0x0B1D48B2` |

### Header fields

| Field | Golden value |
|---|---:|
| `magic` | ASCII `TPWN` |
| `record_version` | `1` |
| `message_type` | `1` |
| `header_bytes` | `64` |
| `sample_format` | `1` |
| `node_id` | `1` |
| `window_sequence` | `1` |
| `sample_rate_hz` | `25600` |
| `sample_count` | `2048` |
| `payload_bytes` | `4096` |
| `sensor_id` | `0` |
| `calibration_version` | `0` |
| `acquisition_flags` | `1` (`SYNTHETIC`) |
| `node_tick` | `0` |
| `node_firmware_build_id` | `1` |
| `record_total_bytes` | `4160` |
| `record_crc32` | `0x0B1D48B2` |

The `sample_rate_hz` value is protocol-test metadata. It must not be read as evidence that 25.6 ksample/s is a qualified ADXL1005 measurement mode.

## Payload generation rule

The payload is a deterministic repeated signed-ramp sequence:

```python
samples = [(i % 1024) - 512 for i in range(2048)]
payload = b"".join(struct.pack("<h", sample) for sample in samples)
```

This produces 2048 signed 16-bit samples and exactly 4096 payload bytes.

First sixteen payload bytes:

```text
00 FE 01 FE 02 FE 03 FE 04 FE 05 FE 06 FE 07 FE
```

Last sixteen payload bytes:

```text
F8 01 F9 01 FA 01 FB 01 FC 01 FD 01 FE 01 FF 01
```

## CRC definition

The vector uses CRC-32/ISO-HDLC as implemented by standard ZIP/Ethernet-compatible CRC-32 routines such as Python `zlib.crc32`.

CRC coverage is:

```text
header bytes 0–59
followed immediately by
payload bytes 64–4159
```

The four CRC-storage bytes at header offsets 60–63 are excluded from the CRC calculation.

Expected result:

```text
record_crc32 = 0x0B1D48B2
```

## Reference-script requirements

`crc-reference.py` should:

1. construct the exact 60-byte header-before-CRC;
2. generate the 4096-byte deterministic payload;
3. calculate the record CRC;
4. assert that the CRC is `0x0B1D48B2`;
5. assemble and write the 4160-byte binary record;
6. assert the final binary size;
7. optionally calculate a SHA-256 artifact hash;
8. support verifying an existing `golden-window-v1.bin`; and
9. print clear pass/fail output for engineering use.

The reference script is an evidence-generation tool. It should remain short, deterministic and dependency-light.

## Use by firmware areas

| Firmware area | Required use of vector |
|---|---|
| `../gateway-dwm/tphip-golden-record/` | Generate or expose the canonical record from Gateway-side DWM firmware |
| `../nucleo-host/tphip-reader/` | Read, reconstruct, validate and compare received bytes against canonical vector |
| `../node-dwm/` | Not required for first vector; may later generate separate sensor/acquisition test records |

## Initial acceptance tests

| Test | Pass condition |
|---|---|
| Reference vector generation | Binary output is 4160 bytes; CRC is `0x0B1D48B2` |
| Gateway DWM local generation | Firmware reports same CRC for locally generated canonical record |
| NUCLEO reconstruction | Received record byte-for-byte equals `golden-window-v1.bin` |
| Repeatability | 100 accepted transfers contain no unexplained CRC/length/sequence error |
| Withheld ACK | DWM retains unresolved record and does not overwrite it |
| Corruption injection | Modified byte causes validation failure and prevents valid DSP publication |

## Metadata file content requirement

When `golden-window-v1.bin` is generated, `golden-window-v1-metadata.md` should record:

| Metadata | Required value |
|---|---|
| Generator script revision | Commit/build identity |
| Vector file name | `golden-window-v1.bin` |
| Binary length | `4160` bytes |
| Record CRC-32 | `0x0B1D48B2` |
| SHA-256 | Calculate from committed binary |
| Header field table | As defined in this README |
| Payload generation rule | As defined in this README |
| Intended tests | DWM generation, NUCLEO receipt and fault injection |
| Limitations | Synthetic vector; no physical sensor or UWB claim |

## Non-claims

The golden vector proves protocol-level data preservation only. It is not:

- an ADXL1005 measurement;
- evidence of ADC fidelity;
- evidence of UWB record reception;
- a calibrated acceleration dataset;
- a DSP result; or
- a TinyML training or inference dataset.

## Immediate next action

Create `crc-reference.py`, generate `golden-window-v1.bin`, calculate its SHA-256 hash and record the results in `golden-window-v1-metadata.md`. Those three files then become the deterministic common input for Gateway-DWM and NUCLEO-host implementation work.
