#!/usr/bin/env python3
"""
Thin-Pod Gateway Telemetry Packet Logger

Purpose
-------
Parse Thin-Pod Gateway Telemetry Packet v1 records from a text file containing
hex bytes, then emit decoded CSV or JSONL output.

This script directly supports Gateway completion checklist item 6:
one parser or logging script.

It also supports:
- Item 4: one defined telemetry packet
- Item 5: one node-to-Gateway transfer
- Item 7: one validation log

Packet format
-------------
Telemetry Packet v1 is a fixed 24-byte little-endian packet:

Offset  Size  Field
0       2     magic: ASCII "TP", bytes 54 50
2       1     version
3       1     packet_type
4       2     node_id
6       4     sequence
10      4     uptime_ms
14      2     battery_mv
16      2     sample_rms_mg
18      2     sample_peak_mg
20      2     status_flags
22      2     crc16

CRC handling
------------
The CRC field is treated as a little-endian uint16.

For rev 0.1 tooling, this script uses CRC-16/CCITT-FALSE:
- polynomial: 0x1021
- initial value: 0xFFFF
- xorout: 0x0000
- no reflection

The sample file may contain XX XX as a placeholder CRC. In that case, the
packet is decoded and the expected CRC bytes are reported, but crc_valid is
reported as "not_checked".
"""

from __future__ import annotations

import argparse
import csv
import json
import re
import sys
from pathlib import Path
from typing import Iterable, List, Optional, TextIO


PACKET_LEN_BYTES = 24
PAYLOAD_LEN_WITHOUT_CRC = 22

MAGIC_BYTES = (0x54, 0x50)  # ASCII "TP"

PACKET_TYPES = {
    1: "telemetry",
}

HEX_TOKEN_RE = re.compile(r"(?<![0-9A-Fa-f])(?:[0-9A-Fa-f]{2}|XX|xx)(?![0-9A-Fa-f])")


class PacketParseError(ValueError):
    """Raised when a candidate packet cannot be parsed."""


def crc16_ccitt_false(data: bytes) -> int:
    """Return CRC-16/CCITT-FALSE for data."""
    crc = 0xFFFF

    for byte in data:
        crc ^= byte << 8

        for _ in range(8):
            if crc & 0x8000:
                crc = ((crc << 1) ^ 0x1021) & 0xFFFF
            else:
                crc = (crc << 1) & 0xFFFF

    return crc & 0xFFFF


def crc_to_little_endian_hex(crc: int) -> str:
    """Return CRC as two little-endian hex bytes, for example '60 03'."""
    return f"{crc & 0xFF:02X} {(crc >> 8) & 0xFF:02X}"


def token_to_int(token: str) -> int:
    token = token.upper()

    if token == "XX":
        raise PacketParseError("placeholder byte encountered where numeric byte was required")

    return int(token, 16)


def le_u16(data: bytes, offset: int) -> int:
    return data[offset] | (data[offset + 1] << 8)


def le_u32(data: bytes, offset: int) -> int:
    return (
        data[offset]
        | (data[offset + 1] << 8)
        | (data[offset + 2] << 16)
        | (data[offset + 3] << 24)
    )


def extract_packet_tokens(line: str) -> Optional[List[str]]:
    """
    Extract the first 24-byte packet from a line of text.

    The function scans for the magic bytes 54 50 and then takes 24 tokens
    from that point. This allows log lines such as:

        [Gateway RX] 54 50 01 01 ...

    It also avoids accidentally parsing timestamp fragments before the packet.
    """
    tokens = [token.upper() for token in HEX_TOKEN_RE.findall(line)]

    if len(tokens) < PACKET_LEN_BYTES:
        return None

    for index in range(0, len(tokens) - 1):
        if tokens[index] == "54" and tokens[index + 1] == "50":
            candidate = tokens[index : index + PACKET_LEN_BYTES]

            if len(candidate) == PACKET_LEN_BYTES:
                return candidate

    return None


def parse_packet_tokens(tokens: List[str], source_line: int) -> dict:
    if len(tokens) != PACKET_LEN_BYTES:
        raise PacketParseError(f"expected {PACKET_LEN_BYTES} bytes, got {len(tokens)}")

    payload_tokens = tokens[:PAYLOAD_LEN_WITHOUT_CRC]
    crc_tokens = tokens[PAYLOAD_LEN_WITHOUT_CRC:PACKET_LEN_BYTES]

    if any(token == "XX" for token in payload_tokens):
        raise PacketParseError("payload contains placeholder byte XX")

    payload = bytes(token_to_int(token) for token in payload_tokens)

    if payload[0] != MAGIC_BYTES[0] or payload[1] != MAGIC_BYTES[1]:
        raise PacketParseError("packet magic is not 54 50 / TP")

    expected_crc = crc16_ccitt_false(payload)
    expected_crc_hex = crc_to_little_endian_hex(expected_crc)

    crc_field_hex = f"{crc_tokens[0]} {crc_tokens[1]}"
    crc_present = all(token != "XX" for token in crc_tokens)

    crc_field_value = None
    crc_valid: str | bool = "not_checked"

    if crc_present:
        crc_bytes = bytes(token_to_int(token) for token in crc_tokens)
        crc_field_value = le_u16(crc_bytes, 0)
        crc_valid = crc_field_value == expected_crc

    packet_type_raw = payload[3]
    packet_type = PACKET_TYPES.get(packet_type_raw, f"unknown_{packet_type_raw}")

    return {
        "source_line": source_line,
        "magic": "TP",
        "version": payload[2],
        "packet_type_raw": packet_type_raw,
        "packet_type": packet_type,
        "node_id": le_u16(payload, 4),
        "sequence": le_u32(payload, 6),
        "uptime_ms": le_u32(payload, 10),
        "battery_mv": le_u16(payload, 14),
        "sample_rms_mg": le_u16(payload, 16),
        "sample_peak_mg": le_u16(payload, 18),
        "status_flags": le_u16(payload, 20),
        "crc_field_hex": crc_field_hex,
        "crc_expected_hex": expected_crc_hex,
        "crc_field_value": crc_field_value,
        "crc_expected_value": expected_crc,
        "crc_valid": crc_valid,
        "note": "" if crc_present else "CRC placeholder present; expected CRC calculated but not checked",
    }


def read_lines(input_path: str) -> Iterable[str]:
    if input_path == "-":
        yield from sys.stdin
        return

    path = Path(input_path)

    with path.open("r", encoding="utf-8") as handle:
        yield from handle


def parse_input(input_path: str, strict: bool = False) -> List[dict]:
    rows: List[dict] = []

    for line_number, line in enumerate(read_lines(input_path), start=1):
        tokens = extract_packet_tokens(line)

        if tokens is None:
            continue

        try:
            rows.append(parse_packet_tokens(tokens, line_number))
        except PacketParseError as exc:
            if strict:
                raise

            rows.append(
                {
                    "source_line": line_number,
                    "magic": "",
                    "version": "",
                    "packet_type_raw": "",
                    "packet_type": "",
                    "node_id": "",
                    "sequence": "",
                    "uptime_ms": "",
                    "battery_mv": "",
                    "sample_rms_mg": "",
                    "sample_peak_mg": "",
                    "status_flags": "",
                    "crc_field_hex": "",
                    "crc_expected_hex": "",
                    "crc_field_value": "",
                    "crc_expected_value": "",
                    "crc_valid": "parse_error",
                    "note": str(exc),
                }
            )

    return rows


def open_output(path: Optional[str]) -> TextIO:
    if path is None or path == "-":
        return sys.stdout

    return Path(path).open("w", encoding="utf-8", newline="")


def write_csv(rows: List[dict], output_path: Optional[str]) -> None:
    fieldnames = [
        "source_line",
        "magic",
        "version",
        "packet_type_raw",
        "packet_type",
        "node_id",
        "sequence",
        "uptime_ms",
        "battery_mv",
        "sample_rms_mg",
        "sample_peak_mg",
        "status_flags",
        "crc_field_hex",
        "crc_expected_hex",
        "crc_field_value",
        "crc_expected_value",
        "crc_valid",
        "note",
    ]

    handle = open_output(output_path)

    try:
        writer = csv.DictWriter(handle, fieldnames=fieldnames, extrasaction="ignore")
        writer.writeheader()

        for row in rows:
            writer.writerow(row)
    finally:
        if handle is not sys.stdout:
            handle.close()


def write_jsonl(rows: List[dict], output_path: Optional[str]) -> None:
    handle = open_output(output_path)

    try:
        for row in rows:
            handle.write(json.dumps(row, sort_keys=False) + "\n")
    finally:
        if handle is not sys.stdout:
            handle.close()


def build_arg_parser() -> argparse.ArgumentParser:
    parser = argparse.ArgumentParser(
        description="Parse Thin-Pod Gateway Telemetry Packet v1 hex logs."
    )

    parser.add_argument(
        "input",
        help="Input file containing hex packet lines. Use '-' to read from stdin.",
    )

    parser.add_argument(
        "--format",
        choices=["csv", "jsonl"],
        default="csv",
        help="Output format. Default: csv.",
    )

    parser.add_argument(
        "-o",
        "--output",
        help="Output file. Defaults to stdout.",
    )

    parser.add_argument(
        "--strict",
        action="store_true",
        help="Fail on the first malformed candidate packet.",
    )

    return parser


def main() -> int:
    parser = build_arg_parser()
    args = parser.parse_args()

    try:
        rows = parse_input(args.input, strict=args.strict)
    except FileNotFoundError:
        print(f"ERROR: input file not found: {args.input}", file=sys.stderr)
        return 2
    except PacketParseError as exc:
        print(f"ERROR: packet parse failed: {exc}", file=sys.stderr)
        return 3

    if not rows:
        print("ERROR: no Telemetry Packet v1 records found", file=sys.stderr)
        return 4

    if args.format == "csv":
        write_csv(rows, args.output)
    else:
        write_jsonl(rows, args.output)

    return 0


if __name__ == "__main__":
    raise SystemExit(main())
