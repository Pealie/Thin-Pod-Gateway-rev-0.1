#!/usr/bin/env python3
from __future__ import annotations

import argparse
import csv
import re
from pathlib import Path

PACKET_RE = re.compile(r"\bTPGW_PKT\b\s+(?P<body>.*)$")

def parse_value(value: str):
    value = value.strip()
    if value.lower().startswith("0x"):
        return int(value, 16)
    try:
        return int(value)
    except ValueError:
        return value

def parse_packet_line(line: str) -> dict | None:
    match = PACKET_RE.search(line)
    if not match:
        return None
    result = {}
    for item in match.group("body").split():
        if "=" not in item:
            continue
        key, value = item.split("=", 1)
        result[key] = parse_value(value)
    return result

def parse_file(path: Path) -> list[dict]:
    packets = []
    for line in path.read_text(encoding="utf-8", errors="replace").splitlines():
        packet = parse_packet_line(line)
        if packet is not None:
            packets.append(packet)
    return packets

def write_csv(path: Path, packets: list[dict]) -> None:
    keys = sorted({key for packet in packets for key in packet.keys()})
    with path.open("w", newline="", encoding="utf-8") as f:
        writer = csv.DictWriter(f, fieldnames=keys)
        writer.writeheader()
        writer.writerows(packets)

def main() -> int:
    parser = argparse.ArgumentParser(description="Parse Thin-Pod Gateway RTT packet logs.")
    parser.add_argument("logfile", type=Path)
    parser.add_argument("--csv", dest="csv_path", type=Path)
    args = parser.parse_args()

    packets = parse_file(args.logfile)
    print(f"packets={len(packets)}")
    for packet in packets:
        print(packet)

    if args.csv_path:
        write_csv(args.csv_path, packets)
        print(f"wrote_csv={args.csv_path}")

    return 0

if __name__ == "__main__":
    raise SystemExit(main())
