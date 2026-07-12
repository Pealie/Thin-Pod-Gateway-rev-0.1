# DW3110 Build-Only Integration Validation

## Purpose

This document records the validation evidence for the first Thin-Pod Qorvo DW3000/DW3110 build-only integration milestone.

The validation was performed on:

```text
Branch: gateway-dw3110-rf-exchange
Commit: f16f6937e20a7785f16346c3ff66ae882858df13
Date: 2026-07-12
```

The milestone acceptance condition was successful configuration, compilation and linkage of both Stage-2 DWM3001-CDK applications against the external Qorvo SDK, with vendor source remaining outside the repository.

## Validation scope

The validation covered:

- external SDK discovery;
- application-specific devicetree binding discovery;
- DWM3001-CDK overlay application;
- Qorvo source-manifest compilation;
- Thin-Pod Zephyr adapter compilation;
- Gateway Stage-2 linkage;
- Node Stage-2 linkage;
- generated ELF and merged HEX images;
- repository formatting checks;
- existing Gateway artefact validation;
- vendor-source boundary checks;
- clean commit and remote push.

The validation did not include flashing, booting, SPI readback, device probing, DW3110 initialisation or UWB RF activity.

## Validation environment

| Field | Value |
|---|---|
| Operator | Neil Thomson |
| Host platform | Windows PowerShell |
| Repository | `C:\ThinPod\Thin-Pod-Gateway-rev-0.1` |
| Branch | `gateway-dw3110-rf-exchange` |
| Starting commit | `3c9ac567789b6798f0ec2d4219f7525855c67833` |
| Resulting commit | `f16f6937e20a7785f16346c3ff66ae882858df13` |
| NCS workspace | `C:\ncs\v3.3.1` |
| NCS release | v3.3.1 |
| Zephyr version | 4.3.99 |
| Zephyr build identifier | `ncs-v3.3.1` |
| Python | 3.12.4 |
| west | 1.5.0 |
| Zephyr SDK | 0.17.0 |
| GCC | 12.2.0 |
| Board target | `decawave_dwm3001cdk/nrf52833` |
| External Qorvo SDK | `C:\Qorvo\DW3_QM33_SDK_1.1.0` |

## Acceptance criteria

| Criterion | Required result |
|---|---|
| Gateway CMake configuration | PASS |
| Gateway devicetree generation | PASS |
| Gateway compilation | PASS |
| Gateway linkage | PASS |
| Gateway `merged.hex` generation | PASS |
| Node CMake configuration | PASS |
| Node devicetree generation | PASS |
| Node compilation | PASS |
| Node linkage | PASS |
| Node `merged.hex` generation | PASS |
| Qorvo source manifest present in both Ninja files | PASS |
| Thin-Pod adapter present in both Ninja files | PASS |
| Vendor source absent from repository changes | PASS |
| `git diff --check` | PASS |
| Gateway artefact checker | 62 passes, 0 warnings, 0 failures |
| Commit created and pushed | PASS |
| Working tree clean after push | PASS |

## Test 1: external SDK discovery

### Expected result

The build must discover the Qorvo SDK through:

```text
QORVO_DW3_SDK_ROOT=C:\Qorvo\DW3_QM33_SDK_1.1.0
```

CMake must report the external driver root.

### Observed result

Both builds reported:

```text
Thin-Pod external Qorvo DW3000 driver:
C:/Qorvo/DW3_QM33_SDK_1.1.0/Drivers/API/Shared/dwt_uwb_driver
```

### Result

PASS.

## Test 2: Gateway Stage-2 build

### Application

```text
C:\ThinPod\Thin-Pod-Gateway-rev-0.1\firmware\gateway\uwb_initiator_stage2_rf
```

### Build directory

```text
C:\ncs\v3.3.1\build_thinpod_gateway_dw3110_rf
```

### Expected result

- DWM3001-CDK board target selected;
- application overlay discovered;
- `qorvo,dw3000` binding accepted;
- CMake configuration completed;
- all source files compiled;
- `zephyr.elf` linked;
- `merged.hex` generated;
- command returns exit code zero.

### Observed result

```text
[174/174] Linking C executable zephyr\zephyr.elf
```

Memory usage:

| Region | Used | Available | Percentage |
|---|---:|---:|---:|
| FLASH | 26,424 bytes | 512 KiB | 5.04% |
| RAM | 8,960 bytes | 128 KiB | 6.84% |

Image generation completed:

```text
[10/10] Generating ../merged.hex
```

Terminal result:

```text
gateway-qorvo-dw3000-build-only: PASS
```

### Generated artefacts

```text
C:\ncs\v3.3.1\build_thinpod_gateway_dw3110_rf\uwb_initiator_stage2_rf\zephyr\zephyr.elf
C:\ncs\v3.3.1\build_thinpod_gateway_dw3110_rf\merged.hex
```

Recorded `merged.hex` size:

```text
74,305 bytes
```

### Result

PASS.

## Test 3: Node Stage-2 build

### Application

```text
C:\ThinPod\Thin-Pod-Gateway-rev-0.1\firmware\node\uwb_responder_stage2_rf
```

### Build directory

```text
C:\ncs\v3.3.1\build_thinpod_node_dw3110_rf
```

### Expected result

- DWM3001-CDK board target selected;
- application overlay discovered;
- `qorvo,dw3000` binding accepted;
- CMake configuration completed;
- all source files compiled;
- `zephyr.elf` linked;
- `merged.hex` generated;
- command returns exit code zero.

### Observed result

```text
[174/174] Linking C executable zephyr\zephyr.elf
```

Memory usage:

| Region | Used | Available | Percentage |
|---|---:|---:|---:|
| FLASH | 26,020 bytes | 512 KiB | 4.96% |
| RAM | 8,960 bytes | 128 KiB | 6.84% |

Image generation completed:

```text
[10/10] Generating ../merged.hex
```

Terminal result:

```text
node-qorvo-dw3000-build-only: PASS
```

### Generated artefacts

```text
C:\ncs\v3.3.1\build_thinpod_node_dw3110_rf\uwb_responder_stage2_rf\zephyr\zephyr.elf
C:\ncs\v3.3.1\build_thinpod_node_dw3110_rf\merged.hex
```

Recorded `merged.hex` size:

```text
73,172 bytes
```

### Result

PASS.

## Test 4: compiled source manifest

### Expected result

Both generated `build.ninja` files must reference:

```text
deca_compat.c
deca_interface.c
deca_rsl.c
dw3000_device.c
qmath.c
thinpod_dw3000_platform.c
```

### Evidence locations

Gateway:

```text
C:\ncs\v3.3.1\build_thinpod_gateway_dw3110_rf\uwb_initiator_stage2_rf\build.ninja
```

Node:

```text
C:\ncs\v3.3.1\build_thinpod_node_dw3110_rf\uwb_responder_stage2_rf\build.ninja
```

### Observed result

All six source filenames were found in both Ninja manifests.

Terminal results:

```text
SOURCE MANIFEST PASS:
C:\ncs\v3.3.1\build_thinpod_gateway_dw3110_rf\uwb_initiator_stage2_rf\build.ninja
```

```text
SOURCE MANIFEST PASS:
C:\ncs\v3.3.1\build_thinpod_node_dw3110_rf\uwb_responder_stage2_rf\build.ninja
```

### Result

PASS.

## Test 5: generated image presence

### Expected result

Both build roots must contain `merged.hex`.

### Observed result

| Image | Size | Recorded timestamp |
|---|---:|---|
| Gateway `merged.hex` | 74,305 bytes | 2026-07-12 15:49:17 |
| Node `merged.hex` | 73,172 bytes | 2026-07-12 15:57:41 |

Terminal result:

```text
Gateway and Node build images: PASS
```

### Result

PASS.

## Test 6: external vendor-source boundary

### Expected result

The repository working tree and staged file set must contain none of:

```text
deca_compat.c
deca_interface.c
deca_rsl.c
dw3000_device.c
qmath.c
LicenseRef-QORVO-2.txt
```

### Observed result

Working-tree check:

```text
External Qorvo source boundary: PASS
```

Staged-tree check:

```text
Staged vendor-source boundary: PASS
```

The committed integration contains Thin-Pod CMake, devicetree and platform-adapter code only.

### Result

PASS.

## Test 7: repository formatting

### Expected result

```powershell
git diff --check
```

must return no errors.

### Observed result

`git diff --check` completed without output or failure before staging and after staging.

### Result

PASS.

## Test 8: Gateway repository artefact checker

### Command

```powershell
& $NcsPython scripts\check_gateway_release_artifacts.py
```

### Observed result

```text
Passes:   62
Warnings: 0
Failures: 0

Artifact check passed.
```

Terminal result:

```text
Repository validation: PASS
```

### Result

PASS.

## Test 9: committed repository state

### Expected result

The integration must be committed on `gateway-dw3110-rf-exchange`, pushed to the matching remote branch and leave a clean working tree.

### Observed result

Commit:

```text
f16f693 Add external Qorvo DW3000 build integration
```

Full commit identifier:

```text
f16f6937e20a7785f16346c3ff66ae882858df13
```

Push result:

```text
3c9ac56..f16f693
gateway-dw3110-rf-exchange -> gateway-dw3110-rf-exchange
```

Final status:

```text
On branch gateway-dw3110-rf-exchange
Your branch is up to date with 'origin/gateway-dw3110-rf-exchange'.

nothing to commit, working tree clean
```

`git show --check HEAD` reported no whitespace errors.

### Result

PASS.

## Validation summary

| Area | Result |
|---|---|
| External Qorvo SDK discovery | PASS |
| Gateway Stage-2 build | PASS |
| Node Stage-2 build | PASS |
| Gateway ELF generation | PASS |
| Node ELF generation | PASS |
| Gateway merged HEX generation | PASS |
| Node merged HEX generation | PASS |
| Qorvo source-manifest verification | PASS |
| Thin-Pod platform-adapter compilation | PASS |
| Vendor-source boundary | PASS |
| Repository formatting | PASS |
| Existing artefact checks | PASS |
| Commit and push | PASS |

Overall result:

```text
PASS
```

## Warnings observed

The NCS sysbuild configuration emitted deprecation warnings for Partition Manager:

```text
PARTITION_MANAGER
PARTITION_MANAGER_ENABLED
SB_CONFIG_PARTITION_MANAGER
```

These warnings originated in the current NCS build configuration. They did not prevent CMake generation, compilation, linkage or HEX generation.

## Limitations

The following evidence remains outside this milestone:

- no firmware image was flashed;
- no DWM3001-CDK was powered as part of this test;
- no RTT boot output was captured;
- no DW3110 reset waveform was measured;
- no SPI command reached the DW3110;
- no device ID was read;
- no `dwt_probe()` result was recorded;
- no `dwt_initialise()` result was recorded;
- no UWB packet was transmitted or received;
- no ranging result was produced;
- no generated image has yet been physically validated.

The generated ELF and HEX files are reproducible build artefacts rather than tested release binaries.

## Next validation gate

The next validation record should cover independent DW3110 physical identity and initialisation proof on each DWM3001-CDK.

Required evidence:

1. firmware image selected by exact SHA-256;
2. board serial number or role recorded;
3. successful flash operation;
4. RTT boot banner;
5. SPI/GPIO platform preparation result;
6. reset assertion and release result;
7. `dwt_probe()` return code;
8. detected device identifier;
9. `dwt_initialise()` return code;
10. IRQ idle-state observation;
11. explicit PASS or FAIL for each board.

Only after this gate should the project advance to one-way UWB TX/RX.

## Validated claim

This evidence supports the following claim:

```text
On 12 July 2026, the Thin-Pod Gateway and Node Stage-2 DWM3001-CDK applications were independently configured, compiled and linked against the externally installed Qorvo DW3000/DW3110 driver. Both builds generated Zephyr ELF and merged HEX images, the expected source manifest was verified, the repository's existing 62 checks passed, and Qorvo vendor source remained outside Git.
```
