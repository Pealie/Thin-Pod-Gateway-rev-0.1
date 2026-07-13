#!/usr/bin/env python3
"""Check the blocking repository conditions for a Gateway OSHWA submission."""

from __future__ import annotations

import hashlib
import sys
from pathlib import Path

EXPECTED_FABRICATION_ARCHIVE_SHA256 = (
    "671fdf24f704971b09d9159b12aac790b0bf3fda13ec090275211d718cfb28aa"
)

REQUIRED_FILES = (
    "README.md",
    "LICENSE.md",
    "LICENSE-HARDWARE.md",
    "LICENSE-SOFTWARE.md",
    "LICENSE-DOCUMENTATION.md",
    "THIRD_PARTY_NOTICES.md",
    "hardware/bom/Thin-Pod_Gateway_rev0.1_BOM.csv",
    "hardware/bom/Thin-Pod_Gateway_rev0.1_BOM.md",
    "hardware/fabrication/README.md",
    "hardware/fabrication/RELEASE-MANIFEST.md",
    "hardware/source/README.md",
    "docs/certification-scope.md",
    "docs/bringup/Gateway_rev0_1_Hardware_Bringup_Note.md",
    "docs/oshwa/OSHWA_Application_Draft.md",
)


def sha256(path: Path) -> str:
    digest = hashlib.sha256()
    with path.open("rb") as handle:
        for block in iter(lambda: handle.read(1024 * 1024), b""):
            digest.update(block)
    return digest.hexdigest()


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

    source_dir = root / "hardware" / "source"
    required_extensions = ("*.kicad_pro", "*.kicad_sch", "*.kicad_pcb")
    for pattern in required_extensions:
        matches = list(source_dir.glob(pattern))
        if matches:
            passes.append(f"editable source present: {matches[0].relative_to(root)}")
        else:
            failures.append(f"editable source missing: hardware/source/{pattern}")

    archive_candidates = list((root / "hardware" / "fabrication").glob("*.zip"))
    matching_archive = None
    for archive in archive_candidates:
        if sha256(archive) == EXPECTED_FABRICATION_ARCHIVE_SHA256:
            matching_archive = archive
            break

    if matching_archive:
        passes.append(
            "fabrication archive present with expected SHA-256: "
            f"{matching_archive.relative_to(root)}"
        )
    else:
        failures.append(
            "fabrication archive with expected SHA-256 is missing from "
            "hardware/fabrication/"
        )

    print("Thin-Pod Gateway OSHWA readiness check")
    print("========================================")
    for item in passes:
        print(f"PASS: {item}")
    for item in failures:
        print(f"FAIL: {item}")

    print()
    print(f"Passes: {len(passes)}")
    print(f"Failures: {len(failures)}")

    if failures:
        print("Result: BLOCKED")
        return 1

    print("Result: READY FOR FINAL HUMAN REVIEW")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
