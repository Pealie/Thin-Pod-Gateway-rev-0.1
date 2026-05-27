#!/usr/bin/env python3
"""Generate or verify the Thin-Pod TPHIP-1 golden synthetic window vector.

This is a deterministic protocol test vector, not physical sensor data.
It creates a 4160-byte SYNTHETIC_WINDOW record defined by:
  64-byte TPHIP-1 record header + 4096-byte signed-int16 payload.

Usage:
  python crc-reference.py generate
  python crc-reference.py verify golden-window-v1.bin

Software-licence status:
  Select and add the repository software licence before public software release.
"""

from __future__ import annotations

import argparse
import hashlib
from pathlib import Path
import struct
import sys
import zlib

HEADER_BYTES = 64
SAMPLE_COUNT = 2048
PAYLOAD_BYTES = SAMPLE_COUNT * 2
RECORD_BYTES = HEADER_BYTES + PAYLOAD_BYTES
EXPECTED_CRC32 = 0x0B1D48B2
EXPECTED_SHA256 = "9145a9612896dfe063b028355bd0282223b478713c7e6877cad6cb3b44df0c06"
DEFAULT_BINARY = Path(__file__).with_name("golden-window-v1.bin")


def build_record() -> bytes:
    """Construct the canonical TPHIP-1 golden record."""
    header_without_crc = struct.pack(
        "<4sHHHHIIIIIIIIQII",
        b"TPWN",       # magic
        1,             # record_version
        1,             # message_type = SYNTHETIC_WINDOW
        HEADER_BYTES,  # header_bytes
        1,             # sample_format = INT16_LE
        1,             # node_id
        1,             # window_sequence
        25600,         # sample_rate_hz: protocol-test metadata only
        SAMPLE_COUNT,  # sample_count
        PAYLOAD_BYTES, # payload_bytes
        0,             # sensor_id
        0,             # calibration_version
        1,             # acquisition_flags = SYNTHETIC
        0,             # node_tick
        1,             # node_firmware_build_id
        RECORD_BYTES,  # record_total_bytes
    )
    samples = [(i % 1024) - 512 for i in range(SAMPLE_COUNT)]
    payload = b"".join(struct.pack("<h", sample) for sample in samples)
    crc32 = zlib.crc32(header_without_crc + payload) & 0xFFFFFFFF

    if crc32 != EXPECTED_CRC32:
        raise RuntimeError(
            f"Generated CRC-32 does not match protocol vector: "
            f"0x{crc32:08X} != 0x{EXPECTED_CRC32:08X}"
        )

    record = header_without_crc + struct.pack("<I", crc32) + payload
    if len(record) != RECORD_BYTES:
        raise RuntimeError(f"Unexpected record length: {len(record)}")
    return record


def calculate_record_crc(record: bytes) -> int:
    """Calculate CRC over header bytes 0..59 and payload bytes 64..end."""
    if len(record) != RECORD_BYTES:
        raise ValueError(f"Record must be {RECORD_BYTES} bytes, got {len(record)}")
    return zlib.crc32(record[:60] + record[64:]) & 0xFFFFFFFF


def verify_record(path: Path) -> bool:
    """Verify file identity, embedded CRC, calculated CRC and canonical bytes."""
    try:
        data = path.read_bytes()
    except OSError as exc:
        print(f"ERROR: Cannot read {path}: {exc}", file=sys.stderr)
        return False

    if len(data) != RECORD_BYTES:
        print(f"FAIL: size {len(data)} bytes; expected {RECORD_BYTES}")
        return False

    embedded_crc = struct.unpack("<I", data[60:64])[0]
    calculated_crc = calculate_record_crc(data)
    digest = hashlib.sha256(data).hexdigest()
    canonical = build_record()

    checks = [
        ("length", len(data) == RECORD_BYTES, f"{len(data)} bytes"),
        ("embedded CRC-32", embedded_crc == EXPECTED_CRC32, f"0x{embedded_crc:08X}"),
        ("calculated CRC-32", calculated_crc == EXPECTED_CRC32, f"0x{calculated_crc:08X}"),
        ("SHA-256", digest == EXPECTED_SHA256, digest),
        ("canonical byte equality", data == canonical, "exact match" if data == canonical else "differs"),
    ]

    passed = True
    for name, ok, value in checks:
        print(f"{'PASS' if ok else 'FAIL'}: {name}: {value}")
        passed &= ok
    return passed


def generate(path: Path) -> bool:
    """Write canonical binary record and verify it."""
    record = build_record()
    path.write_bytes(record)
    print(f"Wrote {path}: {len(record)} bytes")
    return verify_record(path)


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    sub = parser.add_subparsers(dest="command")

    gen = sub.add_parser("generate", help="Generate the canonical binary record.")
    gen.add_argument("path", nargs="?", type=Path, default=DEFAULT_BINARY)

    ver = sub.add_parser("verify", help="Verify an existing canonical binary record.")
    ver.add_argument("path", nargs="?", type=Path, default=DEFAULT_BINARY)

    args = parser.parse_args()
    if args.command in (None, "verify"):
        path = getattr(args, "path", DEFAULT_BINARY)
        ok = verify_record(path)
    else:
        ok = generate(args.path)
    return 0 if ok else 1


if __name__ == "__main__":
    raise SystemExit(main())
