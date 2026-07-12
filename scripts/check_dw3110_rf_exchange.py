#!/usr/bin/env python3
"""Validate Thin-Pod DW3110 one-way RF proof RTT logs.

The checker accepts raw SEGGER RTT Logger or RTT Viewer text. Optional channel
prefixes such as ``00> `` are ignored. A passing record requires at least one
complete 20-frame transmitter suite and one complete 20-frame receiver suite,
with no failed, invalid, timed-out, errored, missing or duplicate frames.
"""

from __future__ import annotations

import argparse
import datetime as dt
import hashlib
import re
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Iterable


PREFIX_RE = re.compile(r"^\s*(?:\d{2}>\s*)?")
TOKEN_RE = re.compile(r"([A-Za-z_][A-Za-z0-9_]*)=([^\s]+)")


@dataclass(frozen=True)
class Summary:
    line_number: int
    fields: dict[str, str]
    raw: str


def sha256(path: Path) -> str:
    digest = hashlib.sha256()
    with path.open("rb") as stream:
        for chunk in iter(lambda: stream.read(1024 * 1024), b""):
            digest.update(chunk)
    return digest.hexdigest().upper()


def normalised_lines(path: Path) -> Iterable[tuple[int, str]]:
    # errors="replace" keeps validation deterministic even if a logger writes
    # a non-UTF-8 byte in its own banner or connection diagnostics.
    with path.open("r", encoding="utf-8", errors="replace", newline=None) as stream:
        for number, raw in enumerate(stream, start=1):
            yield number, PREFIX_RE.sub("", raw.rstrip("\r\n"), count=1)


def summaries(path: Path, marker: str) -> list[Summary]:
    found: list[Summary] = []
    for number, line in normalised_lines(path):
        if not line.startswith(marker + " "):
            continue
        fields = dict(TOKEN_RE.findall(line))
        found.append(Summary(number, fields, line))
    return found


def integer(summary: Summary, name: str) -> int:
    try:
        return int(summary.fields[name], 0)
    except KeyError as exc:
        raise ValueError(
            f"line {summary.line_number}: missing field {name!r}"
        ) from exc
    except ValueError as exc:
        raise ValueError(
            f"line {summary.line_number}: field {name!r} is not an integer: "
            f"{summary.fields.get(name)!r}"
        ) from exc


def passing_tx(summary: Summary) -> tuple[bool, str]:
    try:
        checks = {
            "role": summary.fields.get("role") == "gateway",
            "attempted": integer(summary, "attempted") == 20,
            "pass": integer(summary, "pass") == 20,
            "fail": integer(summary, "fail") == 0,
            "result": summary.fields.get("result") == "PASS",
        }
    except ValueError as exc:
        return False, str(exc)

    failed = [name for name, ok in checks.items() if not ok]
    return not failed, "failed fields: " + ", ".join(failed) if failed else "PASS"


def passing_rx(summary: Summary) -> tuple[bool, str]:
    try:
        checks = {
            "role": summary.fields.get("role") == "node",
            "valid": integer(summary, "valid") == 20,
            "invalid": integer(summary, "invalid") == 0,
            "timeouts": integer(summary, "timeouts") == 0,
            "errors": integer(summary, "errors") == 0,
            "gaps": integer(summary, "gaps") == 0,
            "duplicates": integer(summary, "duplicates") == 0,
            "result": summary.fields.get("result") == "PASS",
        }
    except ValueError as exc:
        return False, str(exc)

    failed = [name for name, ok in checks.items() if not ok]
    return not failed, "failed fields: " + ", ".join(failed) if failed else "PASS"


def count_result_lines(path: Path, marker: str, result: str) -> int:
    count = 0
    for _, line in normalised_lines(path):
        if line.startswith(marker + " ") and f"result={result}" in line:
            count += 1
    return count


def choose_passing(
    records: list[Summary],
    predicate,
) -> tuple[Summary | None, list[str]]:
    diagnostics: list[str] = []
    for record in records:
        passed, reason = predicate(record)
        diagnostics.append(f"line {record.line_number}: {reason}: {record.raw}")
        if passed:
            return record, diagnostics
    return None, diagnostics


def write_report(
    report_path: Path,
    source_commit: str,
    build_hash_file: Path | None,
    gateway_log: Path,
    node_log: Path,
    gateway_hash: str,
    node_hash: str,
    gateway_frame_passes: int,
    gateway_frame_failures: int,
    node_frame_passes: int,
    node_frame_failures: int,
    tx_summary: Summary,
    rx_summary: Summary,
) -> None:
    build_hashes = "Not supplied."
    if build_hash_file is not None:
        if not build_hash_file.is_file():
            raise FileNotFoundError(
                f"build hash file does not exist: {build_hash_file}"
            )
        build_hashes = build_hash_file.read_text(
            encoding="utf-8", errors="replace"
        ).strip()

    report = f"""# DW3110 One-Way RF Exchange Validation

**Date:** {dt.date.today().isoformat()}

**Direction:** Gateway DWM3001-CDK to Node DWM3001-CDK

**Source commit:** `{source_commit}`

**Method:** Polled DW3110 transmit and receive status, deterministic 24-byte payload, DW3110-generated two-byte FCS, RTT evidence

## Result

```text
PASS
```

The Gateway-side DW3110 transmitted a complete 20-frame suite. The Node-side
DW3110 received and validated a complete 20-frame suite with no invalid frames,
receive timeouts, receive errors, sequence gaps or duplicate frames in the
selected passing suite.

## Gateway summary

```text
{tx_summary.raw}
```

Frame records found in the Gateway log:

```text
TPRF_TX result=PASS: {gateway_frame_passes}
TPRF_TX result=FAIL: {gateway_frame_failures}
```

## Node summary

```text
{rx_summary.raw}
```

Frame records found in the Node log:

```text
TPRF_RX result=PASS: {node_frame_passes}
TPRF_RX result=FAIL: {node_frame_failures}
```

## Raw RTT evidence

```text
SHA256  {gateway_hash}  {gateway_log.name}
SHA256  {node_hash}  {node_log.name}
```

## Build artefact hashes

```text
{build_hashes}
```

## Validated claim

The Thin-Pod Gateway-side DWM3001-CDK transmitted deterministic UWB frames
through its internal DW3110, and the Node-side DWM3001-CDK received those
frames through its internal DW3110, verified the exact frame length and payload,
and completed a repeatable 20-frame one-way RF exchange.

## Evidence boundary

This validation does not establish bidirectional exchange, ranging,
vibration-window transport, telemetry transport, IRQ-driven receive handling,
RF range or link budget, production TX-power calibration, antenna performance,
coexistence performance, security properties or regulatory compliance. IRQ
levels remain observations only; event completion is established from DW3110
status bits.
"""

    report_path.parent.mkdir(parents=True, exist_ok=True)
    report_path.write_text(report, encoding="utf-8", newline="\n")


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("--gateway-log", required=True, type=Path)
    parser.add_argument("--node-log", required=True, type=Path)
    parser.add_argument("--report", type=Path)
    parser.add_argument("--source-commit", default="UNRECORDED")
    parser.add_argument("--build-hash-file", type=Path)
    args = parser.parse_args()

    missing = [str(path) for path in (args.gateway_log, args.node_log) if not path.is_file()]
    if missing:
        for path in missing:
            print(f"ERROR missing log: {path}", file=sys.stderr)
        return 2

    tx_records = summaries(args.gateway_log, "TPRF_TX_SUMMARY")
    rx_records = summaries(args.node_log, "TPRF_RX_SUMMARY")
    tx_pass, tx_diagnostics = choose_passing(tx_records, passing_tx)
    rx_pass, rx_diagnostics = choose_passing(rx_records, passing_rx)

    gateway_frame_passes = count_result_lines(args.gateway_log, "TPRF_TX", "PASS")
    gateway_frame_failures = count_result_lines(args.gateway_log, "TPRF_TX", "FAIL")
    node_frame_passes = count_result_lines(args.node_log, "TPRF_RX", "PASS")
    node_frame_failures = count_result_lines(args.node_log, "TPRF_RX", "FAIL")

    gateway_hash = sha256(args.gateway_log)
    node_hash = sha256(args.node_log)

    print(f"gateway_log={args.gateway_log}")
    print(f"gateway_sha256={gateway_hash}")
    print(f"gateway_tx_pass_lines={gateway_frame_passes}")
    print(f"gateway_tx_fail_lines={gateway_frame_failures}")
    print(f"node_log={args.node_log}")
    print(f"node_sha256={node_hash}")
    print(f"node_rx_pass_lines={node_frame_passes}")
    print(f"node_rx_fail_lines={node_frame_failures}")

    if tx_pass is not None:
        print(f"gateway_summary_line={tx_pass.line_number}")
        print(f"gateway_summary={tx_pass.raw}")
    else:
        print("ERROR no passing TPRF_TX_SUMMARY found", file=sys.stderr)
        if not tx_records:
            print("  no TPRF_TX_SUMMARY lines were present", file=sys.stderr)
        else:
            for diagnostic in tx_diagnostics:
                print(f"  {diagnostic}", file=sys.stderr)

    if rx_pass is not None:
        print(f"node_summary_line={rx_pass.line_number}")
        print(f"node_summary={rx_pass.raw}")
    else:
        print("ERROR no passing TPRF_RX_SUMMARY found", file=sys.stderr)
        if not rx_records:
            print("  no TPRF_RX_SUMMARY lines were present", file=sys.stderr)
        else:
            for diagnostic in rx_diagnostics:
                print(f"  {diagnostic}", file=sys.stderr)

    passed = (
        tx_pass is not None
        and rx_pass is not None
        and gateway_frame_passes >= 20
        and node_frame_passes >= 20
    )

    print(f"result={'PASS' if passed else 'FAIL'}")

    if passed and args.report is not None:
        try:
            write_report(
                args.report,
                args.source_commit,
                args.build_hash_file,
                args.gateway_log,
                args.node_log,
                gateway_hash,
                node_hash,
                gateway_frame_passes,
                gateway_frame_failures,
                node_frame_passes,
                node_frame_failures,
                tx_pass,
                rx_pass,
            )
        except (OSError, ValueError) as exc:
            print(f"ERROR could not write report: {exc}", file=sys.stderr)
            return 2
        print(f"report={args.report}")

    return 0 if passed else 1


if __name__ == "__main__":
    raise SystemExit(main())
