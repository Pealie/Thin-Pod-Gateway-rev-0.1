# Stage-1 Firmware Evidence

This directory records the Stage-1 firmware evidence for the Thin-Pod Gateway and node UWB-role harness.

The evidence shows that the project-authored Gateway initiator and node responder firmware builds under nRF Connect SDK v3.3.1, flashes to the intended DWM3001-CDKs using the J-Link runner, boots under Zephyr, and emits structured SEGGER RTT records using the Thin-Pod packet/logging conventions.

## Evidence files

| File | Board / role | What it demonstrates |
|---|---|---|
| `thinpod_gateway_stage1_rtt.log` | Gateway CDK, initiator, ID `0x0000` | Emits `TPGW_POLL` and parser-friendly `TPGW_PKT` records with source/destination IDs, sequence numbers, sample count, sample rate, summary vibration fields and checksums. |
| `thinpod_node_stage1_rtt.log` | Node CDK, responder, ID `0x0001` | Emits `TPNODE_RESPONDER_READY` records with node ID, destination ID, sequence numbers, sample count, sample rate, checksums and representative acceleration sample values. |

## Board mapping

```text
Gateway CDK 760203854 -> gateway_uwb_initiator_stage1
Node CDK    760222856 -> node_uwb_responder_stage1
```

## Key build and flash commands

The firmware was built from PowerShell after manually adding the Nordic toolchain paths:

```powershell
$tc = "C:\ncs\toolchains\936afb6332\opt\bin"
$env:Path = "$tc;$tc\Scripts;$env:Path"
$env:ZEPHYR_TOOLCHAIN_VARIANT = "zephyr"
$env:ZEPHYR_SDK_INSTALL_DIR = "C:\ncs\toolchains\936afb6332\opt\zephyr-sdk"
```

Node responder build:

```powershell
west build -b decawave_dwm3001cdk -p always `
  -d C:\ncs\v3.3.1\build_node_uwb_responder_stage1 `
  C:\thinpod\Thin-Pod-Gateway-rev-0.1\firmware\node\uwb_responder
```

Gateway initiator build:

```powershell
west build -b decawave_dwm3001cdk -p always `
  -d C:\ncs\v3.3.1\build_gateway_uwb_initiator_stage1 `
  C:\thinpod\Thin-Pod-Gateway-rev-0.1\firmware\gateway\uwb_initiator
```

The default `nrfutil` runner was bypassed because of a local plugin-worker issue. Flashing was completed with the J-Link runner:

```powershell
west flash -d C:\ncs\v3.3.1\build_gateway_uwb_initiator_stage1 --runner jlink --dev-id 760203854
west flash -d C:\ncs\v3.3.1\build_node_uwb_responder_stage1 --runner jlink --dev-id 760222856
```

## Interpretation

This evidence verifies the Stage-1 firmware path:

```text
Zephyr build -> J-Link flash -> board boot -> RTT role log -> packet-format evidence
```

It confirms the Gateway and node firmware roles, the source/destination identity convention, the RTT logging path and the staged vibration-window record format.

## Boundary

This is a Stage-1 stub-backend milestone. It does **not** yet prove live DW3110 RF exchange, production UWB transport, industrial diagnostic accuracy, DSP validity or TinyML performance.

The purpose of this evidence is to show that the Gateway and node firmware roles are buildable, flashable and observable, and that the project now has a repeatable basis for the next transport-integration stage.
