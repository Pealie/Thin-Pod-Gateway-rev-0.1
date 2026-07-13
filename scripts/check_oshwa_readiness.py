#!/usr/bin/env python3
"""Check repository artefacts required before a Gateway OSHWA submission."""

from __future__ import annotations

import hashlib
import json
from pathlib import Path

EXPECTED_FABRICATION_ARCHIVE_SHA256 = (
    "3e51d68cb120c55aa06f386f4c433a48845a45d1cb986b2b08ac874ea5ec2d4d"
)
EXPECTED_GERBER_REVISION = "0.1"

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
    "hardware/fabrication/rev0.1/Thin-Pod Gateway-job.gbrjob",
    "hardware/fabrication/rev0.1/Thin-Pod_Gateway_rev0.1_fabrication.zip",
    "hardware/source/README.md",
    "hardware/source/kicad/rev0.1/Thin-Pod Gateway.kicad_pro",
    "hardware/source/kicad/rev0.1/Thin-Pod Gateway.kicad_sch",
    "hardware/source/kicad/rev0.1/Thin-Pod Gateway.kicad_pcb",
    "docs/certification-scope.md",
    "docs/bringup/Gateway_rev0_1_Hardware_Bringup_Note.md",
    "docs/oshwa/OSHWA_Application_Draft.md",
)

BOM_BLOCKING_MARKERS = (
    "to be selected",
    "confirm exact",
    "pending",
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

    bom_paths = (
        root / "hardware" / "bom" / "Thin-Pod_Gateway_rev0.1_BOM.csv",
        root / "hardware" / "bom" / "Thin-Pod_Gateway_rev0.1_BOM.md",
    )
    for bom_path in bom_paths:
        if not bom_path.is_file():
            continue
        text = bom_path.read_text(encoding="utf-8", errors="replace").lower()
        found = sorted(marker for marker in BOM_BLOCKING_MARKERS if marker in text)
        if found:
            failures.append(
                f"BOM reconciliation markers remain in {bom_path.relative_to(root)}: "
                + ", ".join(found)
            )
        else:
            passes.append(f"BOM contains no known release placeholders: {bom_path.relative_to(root)}")

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
