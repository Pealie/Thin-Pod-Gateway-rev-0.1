#!/usr/bin/env python3
"""
Thin-Pod Gateway rev 0.1 release artifact check.

Purpose
-------
This script performs a lightweight, repeatable check of the Gateway rev 0.1
completion evidence currently present in the repository.

It directly supports Gateway completion checklist item 8:
one GitHub Actions or scripted check.

Default behaviour
-----------------
By default, the script checks the artefacts already expected at this stage:

- hardware bring-up note;
- verified pin map document;
- minimal SPI/GPIO probe app scaffold;
- telemetry packet v1 specification;
- telemetry packet sample;
- Gateway packet parser;
- parser execution against the sample packet.

It also warns about later release artefacts that are not expected to exist yet,
such as the validation log, GitHub Actions workflow and OSHWA preparation
checklist.

Release mode
------------
Use --release to make later release artefacts mandatory.

Example:

    python scripts/check_gateway_release_artifacts.py --release

This is useful immediately before tagging Gateway rev 0.1.
"""

from __future__ import annotations

import argparse
import re
import subprocess
import sys
from pathlib import Path
from typing import Iterable, List


HEX_TOKEN_RE = re.compile(r"(?<![0-9A-Fa-f])(?:[0-9A-Fa-f]{2}|XX|xx)(?![0-9A-Fa-f])")


REQUIRED_CURRENT_FILES = [
    "docs/bringup/Gateway_rev0_1_Hardware_Bringup_Note.md",
    "docs/firmware/NUCLEO_SPI5_Gateway_PCB_Pin_Mapping.md",
    "firmware/gateway_probe/README.md",
    "firmware/gateway_probe/CMakeLists.txt",
    "firmware/gateway_probe/prj.conf",
    "firmware/gateway_probe/src/main.c",
    "docs/protocol/ThinPod_Gateway_Telemetry_Packet_v1.md",
    "samples/telemetry_packet_v1_sample.hex",
    "scripts/gateway_packet_logger.py",
    "docs/architecture/Gateway_UWB_Module_Access_Model.md",
    "docs/architecture/DWM3001CDK_Host_Interface_Pin_Feasibility.md",
    "docs/dwm-to-nucleo-host-interface-protocol.md",
    "SECURITY.md",
    "docs/security/Thin-Pod_Security_Architecture_and_Threat_Model.md",
    "docs/security/Thin-Pod_Protocol_Security_Requirements.md",
    "docs/security/Thin-Pod_Firmware_Assurance_Register.md",
]


REQUIRED_RELEASE_FILES = [
    "docs/validation/Gateway_rev0_1_Validation_Log.md",
    "docs/oshwa/Gateway_OSHWA_Preparation_Checklist.md",
    ".github/workflows/gateway-checks.yml",
]


ACTIVE_SCOPE_RISK_PATHS = [
    "docs/roadmap",
]


EXPECTED_ARCHIVE_FILES = [
    "docs/archive/architecture/Gateway_Module_Substitution_and_EOL_Strategy.md",
    "docs/archive/roadmap/Thin-Pod_Post-Gateway_Roadmap_Synthesis.md",
]


PYTHON_FILES_TO_COMPILE = [
    "scripts/gateway_packet_logger.py",
    "scripts/check_gateway_release_artifacts.py",
]


PARSER_EXPECTED_OUTPUT_FRAGMENTS = [
    "TP",
    "telemetry",
    "1,42,123456,3300,125,410",
    "XX XX",
    "60 03",
    "not_checked",
]


PROTOCOL_EXPECTED_FRAGMENTS = [
    "24 bytes",
    "little-endian",
    "Telemetry Packet v1",
    "CRC16",
    "Item 4",
]


PIN_MAP_EXPECTED_FRAGMENTS = [
    "Item 2",
    "SPI5_SCK",
    "SPI5_MISO",
    "SPI5_MOSI",
    "DWM_CS",
    "DWM_IRQ",
    "DWM_RESET",
]


BRINGUP_EXPECTED_FRAGMENTS = [
    "Item 1",
    "stable hardware bring-up",
    "DWM3001-CDK",
    "NUCLEO-N657X0-Q",
]


PROBE_EXPECTED_FRAGMENTS = [
    "Item 3",
    "SPI",
    "GPIO",
]


class CheckResult:
    def __init__(self) -> None:
        self.failures: List[str] = []
        self.warnings: List[str] = []
        self.passes: List[str] = []

    def pass_(self, message: str) -> None:
        self.passes.append(message)
        print(f"PASS: {message}")

    def warn(self, message: str) -> None:
        self.warnings.append(message)
        print(f"WARN: {message}")

    def fail(self, message: str) -> None:
        self.failures.append(message)
        print(f"FAIL: {message}")


def repo_root_from_script() -> Path:
    return Path(__file__).resolve().parents[1]


def check_files_exist(root: Path, paths: Iterable[str], result: CheckResult, *, required: bool) -> None:
    for item in paths:
        path = root / item

        if path.is_file():
            result.pass_(f"file found: {item}")
        elif required:
            result.fail(f"required file missing: {item}")
        else:
            result.warn(f"future release file not yet present: {item}")


def check_paths_absent(root: Path, paths: Iterable[str], result: CheckResult) -> None:
    for item in paths:
        path = root / item

        if path.exists():
            result.fail(f"active scope-risk path is present: {item}")
        else:
            result.pass_(f"active scope-risk path absent: {item}")


def read_text_file(path: Path) -> str:
    return path.read_text(encoding="utf-8", errors="replace")


def check_file_contains(
    root: Path,
    relative_path: str,
    fragments: Iterable[str],
    result: CheckResult,
) -> None:
    path = root / relative_path

    if not path.is_file():
        result.fail(f"cannot inspect missing file: {relative_path}")
        return

    text = read_text_file(path)

    for fragment in fragments:
        if fragment in text:
            result.pass_(f"{relative_path} contains expected text: {fragment}")
        else:
            result.fail(f"{relative_path} missing expected text: {fragment}")


def likely_markdown_rewrites() -> List[str]:
    """
    Build bad strings indirectly so this checker does not match its own source.
    These patterns catch underscores accidentally converted to markdown bold.
    """
    marker = "**"
    return [
        "from " + marker + "future" + marker + " import annotations",
        "if " + marker + "name" + marker + ' == "' + marker + "main" + marker + '"',
        "if " + marker + "name" + marker + " == '" + marker + "main" + marker + "'",
    ]


def check_python_source_syntax(root: Path, relative_path: str, result: CheckResult) -> None:
    path = root / relative_path

    if not path.is_file():
        result.fail(f"cannot compile missing Python file: {relative_path}")
        return

    source = read_text_file(path)

    for bad in likely_markdown_rewrites():
        if bad in source:
            result.fail(f"{relative_path} contains likely markdown rewrite near: {bad}")

    try:
        compile(source, str(path), "exec")
    except SyntaxError as exc:
        result.fail(f"Python syntax check failed for {relative_path}: {exc}")
        return

    result.pass_(f"Python syntax check passed: {relative_path}")


def check_sample_packet(root: Path, result: CheckResult) -> None:
    sample_path = root / "samples/telemetry_packet_v1_sample.hex"

    if not sample_path.is_file():
        result.fail("sample packet file missing")
        return

    text = read_text_file(sample_path)
    tokens = [token.upper() for token in HEX_TOKEN_RE.findall(text)]

    if len(tokens) == 24:
        result.pass_("sample packet contains 24 byte tokens")
    else:
        result.fail(f"sample packet should contain 24 byte tokens, found {len(tokens)}")

    if tokens[:2] == ["54", "50"]:
        result.pass_("sample packet starts with TP magic bytes 54 50")
    else:
        result.fail("sample packet does not start with TP magic bytes 54 50")

    if tokens[-2:] == ["XX", "XX"]:
        result.pass_("sample packet uses XX XX placeholder CRC")
    else:
        result.warn("sample packet does not use XX XX placeholder CRC")

    if "2A" in tokens:
        result.pass_("sample packet includes sequence value 42 / 0x2A")
    else:
        result.fail("sample packet does not include expected sequence value 42 / 0x2A")


def run_parser_against_sample(root: Path, result: CheckResult) -> None:
    parser_path = root / "scripts/gateway_packet_logger.py"
    sample_path = root / "samples/telemetry_packet_v1_sample.hex"

    if not parser_path.is_file():
        result.fail("parser script missing; cannot run parser check")
        return

    if not sample_path.is_file():
        result.fail("sample packet missing; cannot run parser check")
        return

    command = [
        sys.executable,
        str(parser_path),
        str(sample_path),
    ]

    completed = subprocess.run(
        command,
        cwd=str(root),
        text=True,
        capture_output=True,
        check=False,
    )

    if completed.returncode != 0:
        result.fail(
            "parser returned non-zero exit code "
            f"{completed.returncode}: {completed.stderr.strip()}"
        )
        return

    output = completed.stdout

    for fragment in PARSER_EXPECTED_OUTPUT_FRAGMENTS:
        if fragment in output:
            result.pass_(f"parser output contains expected fragment: {fragment}")
        else:
            result.fail(f"parser output missing expected fragment: {fragment}")

    if completed.stderr.strip():
        result.warn(f"parser produced stderr output: {completed.stderr.strip()}")


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(
        description="Check Thin-Pod Gateway rev 0.1 completion artefacts."
    )

    parser.add_argument(
        "--release",
        action="store_true",
        help="Require later release artefacts such as validation log, OSHWA checklist and GitHub Actions workflow.",
    )

    return parser.parse_args()


def main() -> int:
    args = parse_args()
    root = repo_root_from_script()
    result = CheckResult()

    print("Thin-Pod Gateway rev 0.1 artifact check")
    print(f"Repository root: {root}")
    print(f"Mode: {'release' if args.release else 'current'}")
    print()

    check_files_exist(root, REQUIRED_CURRENT_FILES, result, required=True)
    print()

    check_files_exist(root, EXPECTED_ARCHIVE_FILES, result, required=False)
    check_paths_absent(root, ACTIVE_SCOPE_RISK_PATHS, result)
    print()

    check_file_contains(
        root,
        "docs/bringup/Gateway_rev0_1_Hardware_Bringup_Note.md",
        BRINGUP_EXPECTED_FRAGMENTS,
        result,
    )

    check_file_contains(
        root,
        "docs/firmware/NUCLEO_SPI5_Gateway_PCB_Pin_Mapping.md",
        PIN_MAP_EXPECTED_FRAGMENTS,
        result,
    )

    check_file_contains(
        root,
        "firmware/gateway_probe/README.md",
        PROBE_EXPECTED_FRAGMENTS,
        result,
    )

    check_file_contains(
        root,
        "docs/protocol/ThinPod_Gateway_Telemetry_Packet_v1.md",
        PROTOCOL_EXPECTED_FRAGMENTS,
        result,
    )
    print()

    for relative_path in PYTHON_FILES_TO_COMPILE:
        check_python_source_syntax(root, relative_path, result)
    print()

    check_sample_packet(root, result)
    run_parser_against_sample(root, result)
    print()

    check_files_exist(root, REQUIRED_RELEASE_FILES, result, required=args.release)
    print()

    print("Summary")
    print("-------")
    print(f"Passes:   {len(result.passes)}")
    print(f"Warnings: {len(result.warnings)}")
    print(f"Failures: {len(result.failures)}")

    if result.failures:
        print()
        print("Artifact check failed.")
        return 1

    print()
    print("Artifact check passed.")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
