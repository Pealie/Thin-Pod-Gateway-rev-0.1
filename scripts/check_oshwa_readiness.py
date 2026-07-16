#!/usr/bin/env python3
"""Check the objective repository record for OSHWA certification UID UK000092."""

from __future__ import annotations

import hashlib
import json
import struct
from pathlib import Path

EXPECTED_FABRICATION_ARCHIVE_SHA256 = (
    "3e51d68cb120c55aa06f386f4c433a48845a45d1cb986b2b08ac874ea5ec2d4d"
)
EXPECTED_GERBER_REVISION = "0.1"
EXPECTED_OSHWA_UID = "UK000092"
EXPECTED_CERTIFICATION_DATE = "16 July 2026"
EXPECTED_OSHWA_RECORD_URL = "https://certification.oshwa.org/uk000092.html"
EXPECTED_MARK_DIMENSIONS = (4000, 1122)

REQUIRED_FILES = (
    "README.md",
    "LICENSE.md",
    "LICENSE-HARDWARE.md",
    "LICENSE-SOFTWARE.md",
    "LICENSE-DOCUMENTATION.md",
    "THIRD_PARTY_NOTICES.md",
    "images/certification/oshwa-uk000092.png",
    "oshwa/README.md",
    "oshwa/application-record.md",
    "hardware/bom/Thin-Pod_Gateway_rev0.1_BOM.csv",
    "hardware/bom/Thin-Pod_Gateway_rev0.1_BOM.md",
    "hardware/fabrication/README.md",
    "hardware/fabrication/RELEASE-MANIFEST.md",
    "hardware/fabrication/rev0.1/Thin-Pod Gateway-job.gbrjob",
    "hardware/fabrication/rev0.1/Thin-Pod_Gateway_rev0.1_fabrication.zip",
    "hardware/source/README.md",
    "hardware/source/kicad/rev0.1/Thin-Pod Gateway.kicad_pro",
    "hardware/source/kicad/rev0.1/Thin-Pod Gateway.kicad_sch",
    "hardware/source/kicad/rev0.1/Thin-Pod Gateway.kicad_pcb",
    "docs/certification-scope.md",
    "docs/bringup/Gateway_rev0_1_Hardware_Bringup_Note.md",
    "docs/footprint-provenance.md",
    "docs/oshwa/Gateway_OSHWA_Preparation_Checklist.md",
    "docs/archive/oshwa/OSHWA_Application_Draft.md",
    "docs/validation/cad/rev0.1/Thin-Pod_Gateway_rev0.1_ERC.rpt",
    "docs/validation/cad/rev0.1/Thin-Pod_Gateway_rev0.1_DRC.rpt",
    "docs/validation/cad/rev0.1/Gerber_Drill_Inspection_Record.md",
)

CERTIFICATION_TEXT_FILES = (
    "README.md",
    "docs/certification-scope.md",
    "oshwa/application-record.md",
    "docs/oshwa/Gateway_OSHWA_Preparation_Checklist.md",
    "hardware/fabrication/RELEASE-MANIFEST.md",
    "LICENSE.md",
)


def sha256(path: Path) -> str:
    digest = hashlib.sha256()
    with path.open("rb") as handle:
        for block in iter(lambda: handle.read(1024 * 1024), b""):
            digest.update(block)
    return digest.hexdigest()


def require_report_text(
    root: Path,
    relative: str,
    required_phrases: tuple[str, ...],
    passes: list[str],
    failures: list[str],
) -> None:
    path = root / relative
    if not path.is_file():
        return
    text = path.read_text(encoding="utf-8", errors="replace")
    missing = [phrase for phrase in required_phrases if phrase not in text]
    if missing:
        failures.append(
            f"validation report does not contain expected result in {relative}: "
            + ", ".join(missing)
        )
    else:
        passes.append(f"validation report records a clean result: {relative}")


def require_certification_text(
    root: Path,
    relative: str,
    passes: list[str],
    failures: list[str],
) -> None:
    path = root / relative
    if not path.is_file():
        return
    text = path.read_text(encoding="utf-8", errors="replace")
    required = (
        EXPECTED_OSHWA_UID,
        EXPECTED_CERTIFICATION_DATE,
        EXPECTED_OSHWA_RECORD_URL,
    )
    missing = [phrase for phrase in required if phrase not in text]
    if missing:
        failures.append(
            f"certification record is incomplete in {relative}: "
            + ", ".join(missing)
        )
    else:
        passes.append(f"certification identity is consistent: {relative}")


def check_mark_png(
    root: Path,
    passes: list[str],
    failures: list[str],
) -> None:
    relative = "images/certification/oshwa-uk000092.png"
    path = root / relative
    if not path.is_file():
        return

    try:
        with path.open("rb") as handle:
            signature = handle.read(8)
            length = struct.unpack(">I", handle.read(4))[0]
            chunk_type = handle.read(4)
            ihdr = handle.read(length)
    except (OSError, struct.error) as exc:
        failures.append(f"could not inspect certification mark: {exc}")
        return

    if signature != b"\x89PNG\r\n\x1a\n" or chunk_type != b"IHDR" or len(ihdr) < 8:
        failures.append(f"certification mark is not a valid PNG header: {relative}")
        return

    dimensions = struct.unpack(">II", ihdr[:8])
    if dimensions != EXPECTED_MARK_DIMENSIONS:
        failures.append(
            "certification mark dimensions mismatch: "
            f"expected {EXPECTED_MARK_DIMENSIONS}, got {dimensions}"
        )
    else:
        passes.append(
            "certification mark PNG is present with expected dimensions: "
            f"{dimensions[0]}x{dimensions[1]}"
        )


def main() -> int:
    root = Path(__file__).resolve().parents[1]
    failures: list[str] = []
    passes: list[str] = []

    for relative in REQUIRED_FILES:
        path = root / relative
        if path.is_file():
            passes.append(f"required file present: {relative}")
        else:
            failures.append(f"required file missing: {relative}")

    archive = (
        root
        / "hardware"
        / "fabrication"
        / "rev0.1"
        / "Thin-Pod_Gateway_rev0.1_fabrication.zip"
    )
    if archive.is_file():
        actual_hash = sha256(archive)
        if actual_hash == EXPECTED_FABRICATION_ARCHIVE_SHA256:
            passes.append(
                "fabrication archive SHA-256 matches release manifest: "
                f"{actual_hash}"
            )
        else:
            failures.append(
                "fabrication archive SHA-256 mismatch: "
                f"expected {EXPECTED_FABRICATION_ARCHIVE_SHA256}, got {actual_hash}"
            )

    job_path = (
        root
        / "hardware"
        / "fabrication"
        / "rev0.1"
        / "Thin-Pod Gateway-job.gbrjob"
    )
    if job_path.is_file():
        try:
            job = json.loads(job_path.read_text(encoding="utf-8"))
            revision = job["GeneralSpecs"]["ProjectId"]["Revision"]
        except (OSError, UnicodeDecodeError, json.JSONDecodeError, KeyError, TypeError) as exc:
            failures.append(f"could not read Gerber job revision: {exc}")
        else:
            if revision == EXPECTED_GERBER_REVISION:
                passes.append(f"Gerber job revision is {revision}")
            else:
                failures.append(
                    "Gerber job revision mismatch: "
                    f"expected {EXPECTED_GERBER_REVISION}, got {revision!r}"
                )

    require_report_text(
        root,
        "docs/validation/cad/rev0.1/Thin-Pod_Gateway_rev0.1_ERC.rpt",
        ("ERC messages: 0", "Errors 0", "Warnings 0"),
        passes,
        failures,
    )
    require_report_text(
        root,
        "docs/validation/cad/rev0.1/Thin-Pod_Gateway_rev0.1_DRC.rpt",
        (
            "Found 0 DRC violations",
            "Found 0 unconnected pads",
            "Found 0 Footprint errors",
        ),
        passes,
        failures,
    )
    require_report_text(
        root,
        "docs/validation/cad/rev0.1/Gerber_Drill_Inspection_Record.md",
        ("Inspection status:** PASS", "**PASS:**"),
        passes,
        failures,
    )

    for relative in CERTIFICATION_TEXT_FILES:
        require_certification_text(root, relative, passes, failures)

    check_mark_png(root, passes, failures)

    print("Thin-Pod Gateway OSHWA certification-record check")
    print("=================================================")
    for item in passes:
        print(f"PASS: {item}")
    for item in failures:
        print(f"FAIL: {item}")

    print()
    print(f"Passes: {len(passes)}")
    print(f"Failures: {len(failures)}")

    if failures:
        print("Result: CERTIFICATION RECORD INCONSISTENT")
        return 1

    print("Result: CERTIFICATION PACKAGE CONSISTENT")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
