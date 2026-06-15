# RTT Alive-Test Firmware Baseline

## Purpose

This document records the minimal firmware baseline used to prove that both DWM3001CDKs in the Thin-Pod system can be built, flashed and observed through SEGGER RTT.

The alive-test firmware is deliberately small. It does not implement UWB ranging, sensor acquisition, Gateway analysis or telemetry. Its purpose is to prove the development loop before restoring or rebuilding the real node responder and Gateway initiator roles.

## Hardware

```text
Gateway CDK: DWM3001CDK / DWM3001C / nRF52833
Node CDK:    DWM3001CDK / DWM3001C / nRF52833
Debug:       onboard J-Link OB
Interface:   SWD
Logging:     SEGGER RTT Terminal 0
```

## SDK and board target

```text
SDK:   nRF Connect SDK v3.3.1
West:  v1.5.0
GCC:   arm-zephyr-eabi-gcc, Zephyr SDK 0.17.0, GCC 12.2.0
Board: decawave_dwm3001cdk
SoC:   nRF52833
```

Board visibility was verified with:

```cmd
cd /d C:\ncs\v3.3.1
west boards | findstr /I dwm3001
```

Expected result:

```text
decawave_dwm3001cdk
```

## Applications

```text
firmware/gateway/zephyr_rtt_alive
firmware/node/zephyr_rtt_alive
```

The Gateway image emits:

```text
Thin-Pod Gateway CDK alive
gateway tick <n>
```

The node image emits:

```text
Thin-Pod Node CDK alive
node tick <n>
```

## Build commands

Gateway:

```cmd
cd /d C:\ncs\v3.3.1
west build -b decawave_dwm3001cdk -p always -d C:\ncs\v3.3.1\build_gateway_rtt_alive C:\path\to\repo\firmware\gateway\zephyr_rtt_alive
```

Node:

```cmd
cd /d C:\ncs\v3.3.1
west build -b decawave_dwm3001cdk -p always -d C:\ncs\v3.3.1\build_node_rtt_alive C:\path\to\repo\firmware\node\zephyr_rtt_alive
```

## Flash commands

Only one DWM3001CDK should be connected during flashing unless a specific J-Link serial-number workflow is used.

Gateway:

```cmd
west flash -d C:\ncs\v3.3.1\build_gateway_rtt_alive -r jlink
```

Node:

```cmd
west flash -d C:\ncs\v3.3.1\build_node_rtt_alive -r jlink
```

Manual J-Link fallback for the node:

```text
LoadFile C:\ncs\v3.3.1\build_node_rtt_alive\merged.hex
Reset
Go
```

Manual J-Link fallback for the Gateway:

```text
LoadFile C:\ncs\v3.3.1\build_gateway_rtt_alive\merged.hex
Reset
Go
```

## RTT Viewer settings

```text
Device: NRF52833_XXAA
Interface: SWD
Speed: 4000 kHz, or 2000 kHz if required
RTT control block: Auto-detect
```

## Bring-up interpretation

Passing this test means:

```text
J-Link OB access: PASS
SWD target access: PASS
nRF52833 execution: PASS
Zephyr build path: PASS
Firmware flash path: PASS
SEGGER RTT logging path: PASS
```

It does not prove UWB ranging, sensor acquisition, packet transport or Gateway-side analysis. Those should be rebuilt as separate stages after this baseline is committed.
