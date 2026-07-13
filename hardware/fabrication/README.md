# Thin-Pod Gateway rev 0.1 fabrication outputs

This directory records the fabrication package supplied for the manufactured Gateway carrier PCB identified as `rev 0.1f` where that marking appears on the board.

## Package under review

Source archive supplied for the OSHWA preparation audit:

```text
ThinPodGatewayFabricationPullupsCouplingCapsRev01f.zip
SHA-256: 671fdf24f704971b09d9159b12aac790b0bf3fda13ec090275211d718cfb28aa
Size: 114,532 bytes
```

The package contains 14 files: top and bottom copper, solder mask, paste and silkscreen Gerbers; board profile; PTH and NPTH drill files; drill maps; and a Gerber job file.

The archive is retained in split binary parts under `rev0.1f/archive-parts/` because the connected repository interface used for this audit cannot upload a binary archive directly. Reconstruct it with the supplied script or commands, then verify the SHA-256 value above.

## Fabrication metadata

The Gerber job file records:

| Property | Value |
|---|---|
| Generator | KiCad Pcbnew 10.0.1 |
| Generation date | 21 May 2026 |
| Board size | 160.05 mm × 145.05 mm |
| Copper layers | 2 |
| Board thickness | 1.6 mm |
| Dielectric | FR4, 1.51 mm recorded |
| Copper thickness | 0.035 mm per outer layer |
| Minimum track width/spacing | 0.2 mm |
| PTH drill hits | 244 |
| NPTH drill hits | 9 |

The job metadata contains the unresolved revision value `rev?`. The physical board and repository treat this package as the `rev 0.1f` fabrication build within the Thin-Pod Gateway rev 0.1 release path. The final editable KiCad project should set a definite revision before regenerated release outputs are frozen.

## Normative-source boundary

These files are derived manufacturing outputs. They do not replace the editable KiCad project, schematic, PCB layout and project-local libraries required as the preferred modification source for an OSHWA application.

The exact KiCad source used to generate this package is not yet present at the repository paths advertised by the existing README. Until it is committed and reconciled against this package, the fabrication outputs are evidence of the manufactured build rather than a complete certifiable hardware-source release.

See [`RELEASE-MANIFEST.md`](RELEASE-MANIFEST.md) for hashes and [`../source/README.md`](../source/README.md) for the source-file gate.
