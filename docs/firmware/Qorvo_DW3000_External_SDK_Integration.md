# Qorvo DW3000 External SDK Integration

## Purpose

This document records the build-time integration of the Qorvo DW3000/DW3110 driver into the Thin-Pod DWM3001-CDK Stage-2 firmware applications.

The integration supports the branch:

```text
gateway-dw3110-rf-exchange
```

and was introduced by commit:

```text
f16f6937e20a7785f16346c3ff66ae882858df13
```

The milestone establishes a reproducible Zephyr build path for both the Gateway-side DWM3001-CDK application and the Node-side DWM3001-CDK application while retaining the Qorvo SDK outside the Git repository.

## Scope

This integration includes:

- an external Qorvo SDK root selected through an environment variable;
- a fixed Qorvo source manifest for the DW3000/DW3110 family;
- a Thin-Pod Zephyr platform adapter for SPI, reset, IRQ, delays and interrupt locking;
- DWM3001-CDK devicetree overlays;
- an application-specific `qorvo,dw3000` binding;
- Stage-2 Gateway and Node CMake integration;
- successful clean builds for both applications.

This milestone covers compilation and linkage only.

It does not yet establish:

- physical SPI communication with the DW3110;
- device-ID readback;
- successful `dwt_probe()` execution on hardware;
- successful `dwt_initialise()` execution on hardware;
- UWB transmission or reception;
- ranging;
- node-to-Gateway vibration transport;
- tested production firmware images.

## Repository layout

The committed Thin-Pod integration is located under:

```text
firmware/common/qorvo_dw3000/
```

The common integration files are:

```text
firmware/common/qorvo_dw3000/CMakeLists.txt
firmware/common/qorvo_dw3000/qorvo_dw3000_external.cmake
firmware/common/qorvo_dw3000/decawave_dwm3001cdk_nrf52833.overlay
firmware/common/qorvo_dw3000/include/thinpod_dw3000_platform.h
firmware/common/qorvo_dw3000/src/thinpod_dw3000_platform.c
```

The build-active Gateway application files are:

```text
firmware/gateway/uwb_initiator_stage2_rf/CMakeLists.txt
firmware/gateway/uwb_initiator_stage2_rf/prj.conf
firmware/gateway/uwb_initiator_stage2_rf/src/main.c
firmware/gateway/uwb_initiator_stage2_rf/boards/decawave_dwm3001cdk_nrf52833.overlay
firmware/gateway/uwb_initiator_stage2_rf/dts/bindings/uwb/qorvo,dw3000.yaml
```

The build-active Node application files are:

```text
firmware/node/uwb_responder_stage2_rf/CMakeLists.txt
firmware/node/uwb_responder_stage2_rf/prj.conf
firmware/node/uwb_responder_stage2_rf/src/main.c
firmware/node/uwb_responder_stage2_rf/boards/decawave_dwm3001cdk_nrf52833.overlay
firmware/node/uwb_responder_stage2_rf/dts/bindings/uwb/qorvo,dw3000.yaml
```

The common overlay is retained as a reference template. The per-application overlay copies are the files consumed by the current Zephyr builds and should remain synchronised with the template.

## External SDK contract

The Qorvo SDK is installed locally at:

```text
C:\Qorvo\DW3_QM33_SDK_1.1.0
```

The integration discovers it through:

```text
QORVO_DW3_SDK_ROOT
```

PowerShell example:

```powershell
$env:QORVO_DW3_SDK_ROOT = "C:\Qorvo\DW3_QM33_SDK_1.1.0"
```

The CMake integration converts this Windows path to CMake path form and derives:

```text
Drivers/API/Shared/dwt_uwb_driver
Drivers/API/Shared/dwt_uwb_driver/dw3000
Drivers/API/Shared/dwt_uwb_driver/lib/qmath
```

Configuration fails immediately when the environment variable or a required external file is missing.

## Qorvo source manifest

The following external Qorvo sources are compiled into each Stage-2 application:

```text
deca_compat.c
deca_interface.c
deca_rsl.c
dw3000/dw3000_device.c
lib/qmath/src/qmath.c
```

The include search path covers:

```text
Drivers/API/Shared/dwt_uwb_driver
Drivers/API/Shared/dwt_uwb_driver/dw3000
Drivers/API/Shared/dwt_uwb_driver/lib/qmath/include
```

The build defines:

```text
USE_DRV_DW3000
```

The Thin-Pod source added to the same application target is:

```text
firmware/common/qorvo_dw3000/src/thinpod_dw3000_platform.c
```

The build validation confirmed that all six source files appeared in the generated Ninja manifests for both applications.

## Licensing and source boundary

The Qorvo driver files identify their governing licence as:

```text
LicenseRef-QORVO-2
```

The authoritative licence text remains in the installed Qorvo SDK.

The Thin-Pod repository records the integration contract and source filenames while excluding Qorvo vendor source. This preserves a clear boundary between:

- Thin-Pod-owned integration code committed to Git; and
- Qorvo-owned SDK material supplied through the local external SDK installation.

The `.gitignore` additions protect local SDK and private analysis material:

```text
Qorvo/
ThinPod-Qorvo-DW3110-analysis/
*.qorvo-local
```

Before any redistribution of Qorvo files or derived binary release assets, the applicable Qorvo licence terms should be reviewed against the planned distribution method.

## DWM3001-CDK internal signal mapping

The Zephyr adapter uses the DWM3001-CDK internal nRF52833-to-DW3110 route:

| Function | nRF52833 pin | Direction from nRF52833 |
|---|---:|---|
| SPI3 SCK | P0.03 | Output |
| SPI3 COPI / MOSI | P0.08 | Output |
| SPI3 CIPO / MISO | P0.29 | Input |
| SPI3 CS | P1.06 | Output, active low |
| DW3110 RESET | P0.25 | Open-drain-style reset control |
| DW3110 IRQ | P1.02 | Input, active high |

The overlay creates a `dw3000@0` child beneath `spi3` with:

```dts
compatible = "qorvo,dw3000";
reg = <0>;
spi-max-frequency = <8000000>;
status = "okay";
```

The application binding includes Zephyr's standard SPI-device binding:

```yaml
compatible: "qorvo,dw3000"
include: spi-device.yaml
```

This supplies the generated SPI properties required by `SPI_DT_SPEC_GET()`.

## Zephyr configuration

Both applications enable:

```text
CONFIG_SPI=y
CONFIG_GPIO=y
CONFIG_MAIN_STACK_SIZE=4096
```

Existing RTT console settings remain active:

```text
CONFIG_USE_SEGGER_RTT=y
CONFIG_RTT_CONSOLE=y
CONFIG_UART_CONSOLE=n
CONFIG_LOG=n
```

## Platform-adapter responsibilities

The Thin-Pod adapter provides the Qorvo platform boundary required by the driver.

### SPI rate selection

```text
port_set_dw_ic_spi_slowrate()
port_set_dw_ic_spi_fastrate()
```

The current values are:

```text
slow rate: 2 MHz
fast rate: 8 MHz
```

The 8 MHz rate is intentionally conservative for the first integration stage.

### SPI transactions

The adapter implements:

```text
writetospi()
writetospiwithcrc()
readfromspi()
```

The implementation uses Zephyr `spi_dt_spec`, `spi_write_dt()` and `spi_transceive_dt()`.

Header, body and optional CRC segments are passed as Zephyr SPI buffers so that a Qorvo command remains within one chip-select transaction.

### Reset

The adapter implements:

```text
reset_DWIC()
```

Reset handling follows an open-drain-style sequence:

1. configure RESET as an active-low output;
2. hold the line low briefly;
3. release the line by returning the GPIO to input mode;
4. allow the DW3110 to settle.

The implementation never actively drives RESET high.

### Wake-up

The adapter implements:

```text
wakeup_device_with_io()
```

The current wake-up sequence asserts CS low, holds it for the required interval, releases it, and allows recovery time.

This function is compiled and remains unvalidated on physical hardware.

### IRQ and critical sections

The adapter provides:

```text
thinpod_dw3000_irq_is_active()
decamutexon()
decamutexoff()
```

The IRQ helper samples the physical DW3110 IRQ line. The mutex callbacks use Zephyr interrupt locking to protect critical Qorvo driver operations.

### Delay functions

The adapter provides:

```text
deca_sleep()
deca_usleep()
```

These map onto Zephyr millisecond sleep and microsecond busy-wait facilities.

### Probe interface

The adapter exposes:

```text
thinpod_dw3000_probe_interf
```

This associates the Zephyr SPI callbacks, wake-up callback and `dw3000_driver` with the Qorvo `dwt_probe_s` interface.

The current Stage-2 applications link this structure without invoking `dwt_probe()`.

## Build environment

The validated environment was:

| Component | Value |
|---|---|
| Host operating system | Windows |
| NCS workspace | `C:\ncs\v3.3.1` |
| NCS release | v3.3.1 |
| Zephyr version reported by build | 4.3.99 |
| Zephyr build identifier | `ncs-v3.3.1` |
| Python | 3.12.4 |
| west | 1.5.0 |
| Zephyr SDK | 0.17.0 |
| C compiler | GNU 12.2.0 |
| Board target | `decawave_dwm3001cdk/nrf52833` |
| Repository | `C:\ThinPod\Thin-Pod-Gateway-rev-0.1` |
| Branch | `gateway-dw3110-rf-exchange` |
| Integration commit | `f16f6937e20a7785f16346c3ff66ae882858df13` |

The build emitted Partition Manager deprecation warnings from NCS sysbuild. They did not affect configuration, compilation, linkage or image generation.

## Environment setup

```powershell
$Repo       = "C:\ThinPod\Thin-Pod-Gateway-rev-0.1"
$NcsRoot    = "C:\ncs\v3.3.1"
$NcsBin     = "C:\ncs\toolchains\936afb6332\opt\bin"
$NcsPython  = Join-Path $NcsBin "python.exe"
$ZephyrBase = Join-Path $NcsRoot "zephyr"
$ZephyrSdk  = "C:\ncs\toolchains\936afb6332\opt\zephyr-sdk"
$QorvoRoot  = "C:\Qorvo\DW3_QM33_SDK_1.1.0"

$env:PYTHONHOME = $NcsBin
Remove-Item Env:PYTHONPATH -ErrorAction SilentlyContinue
Remove-Item Env:PYTHONNOUSERSITE -ErrorAction SilentlyContinue

$env:PATH = "$NcsBin;$env:PATH"
$env:ZEPHYR_BASE = $ZephyrBase
$env:ZEPHYR_TOOLCHAIN_VARIANT = "zephyr"
$env:ZEPHYR_SDK_INSTALL_DIR = $ZephyrSdk
$env:QORVO_DW3_SDK_ROOT = $QorvoRoot

Set-Location $NcsRoot
```

## Gateway build command

```powershell
$GatewayApp = Join-Path $Repo `
    "firmware\gateway\uwb_initiator_stage2_rf"

$GatewayBuild = Join-Path $NcsRoot `
    "build_thinpod_gateway_dw3110_rf"

& $NcsPython -m west build `
    --sysbuild `
    -b decawave_dwm3001cdk/nrf52833 `
    -p always `
    -d $GatewayBuild `
    $GatewayApp
```

## Node build command

```powershell
$NodeApp = Join-Path $Repo `
    "firmware\node\uwb_responder_stage2_rf"

$NodeBuild = Join-Path $NcsRoot `
    "build_thinpod_node_dw3110_rf"

& $NcsPython -m west build `
    --sysbuild `
    -b decawave_dwm3001cdk/nrf52833 `
    -p always `
    -d $NodeBuild `
    $NodeApp
```

## Generated artefact locations

Gateway:

```text
C:\ncs\v3.3.1\build_thinpod_gateway_dw3110_rf\uwb_initiator_stage2_rf\zephyr\zephyr.elf
C:\ncs\v3.3.1\build_thinpod_gateway_dw3110_rf\merged.hex
```

Node:

```text
C:\ncs\v3.3.1\build_thinpod_node_dw3110_rf\uwb_responder_stage2_rf\zephyr\zephyr.elf
C:\ncs\v3.3.1\build_thinpod_node_dw3110_rf\merged.hex
```

These are generated build products. They remain outside the source repository at this stage.

## Reproducibility checks

The following checks should accompany a clean build:

```powershell
git diff --check
```

```powershell
& $NcsPython scripts\check_gateway_release_artifacts.py
```

The external-source boundary can be checked with:

```powershell
$ForbiddenVendorFiles = @(
    "deca_compat.c",
    "deca_interface.c",
    "deca_rsl.c",
    "dw3000_device.c",
    "qmath.c",
    "LicenseRef-QORVO-2.txt"
)

$WorkingFiles = git status --porcelain

foreach ($Forbidden in $ForbiddenVendorFiles) {
    if ($WorkingFiles -match [regex]::Escape($Forbidden)) {
        throw "Vendor source appears inside the repository: $Forbidden"
    }
}
```

## Known constraints

1. The external SDK path is machine-local.
2. The Qorvo SDK is required to reproduce the build.
3. The current adapter has compiled successfully and has no physical-device execution evidence.
4. SPI, reset, wake-up and IRQ behaviour remain subject to bench validation.
5. The Stage-2 Gateway and Node applications still use synthetic packet scaffolding.
6. `merged.hex` and `zephyr.elf` are untested build outputs at this milestone.
7. The duplicate per-application overlays require synchronisation with the common reference template.

## Next milestone

The next firmware gate is a physical DW3110 identity and initialisation proof on each DWM3001-CDK:

1. prepare the Zephyr SPI and GPIO devices;
2. assert and release RESET;
3. invoke `dwt_probe()`;
4. read and report the detected device identity;
5. invoke `dwt_initialise()`;
6. print distinct success or failure states through RTT;
7. repeat independently on the Gateway-side and Node-side DWM3001-CDK boards.

A one-way UWB TX/RX experiment should follow only after both boards pass this gate.

## Valid claim

This document supports the following claim:

```text
The Thin-Pod Gateway and Node Stage-2 DWM3001-CDK applications compile and link successfully against the externally installed Qorvo DW3000/DW3110 driver through a committed Zephyr SPI/reset/IRQ adapter, devicetree binding and CMake integration.
```

It does not support a claim of physical DW3110 communication, initialisation or RF operation.
