# Thin-Pod Node RTT Alive Test

Minimal Zephyr firmware for proving the Thin-Pod node DWM3001CDK / nRF52833 build, flash and SEGGER RTT runtime path.

This is not the production node responder firmware. It is a bring-up and regression-test image intended to prove that the Thin-Pod node CDK can:

- build under nRF Connect SDK / Zephyr,
- flash through the onboard J-Link OB,
- execute on the nRF52833,
- emit runtime output over SEGGER RTT.

## Target

```text
Board: decawave_dwm3001cdk
SoC:   nRF52833
SDK:   nRF Connect SDK v3.3.1
Log:   SEGGER RTT, Terminal 0
```

## Build

From an nRF Connect SDK v3.3.1 toolchain terminal:

```cmd
cd /d C:\ncs\v3.3.1
west build -b decawave_dwm3001cdk -p always -d C:\ncs\v3.3.1\build_node_rtt_alive C:\path\to\repo\firmware\node\zephyr_rtt_alive
```

## Flash

Connect only the Thin-Pod node CDK over the J-Link OB USB port before flashing.

```cmd
west flash -d C:\ncs\v3.3.1\build_node_rtt_alive -r jlink
```

If the west/J-Link runner fails, the generated HEX can be loaded manually in J-Link Commander:

```text
LoadFile C:\ncs\v3.3.1\build_node_rtt_alive\merged.hex
Reset
Go
```

## Expected RTT output

Open SEGGER RTT Viewer with:

```text
Device: NRF52833_XXAA
Interface: SWD
Speed: 4000 kHz
RTT control block: Auto-detect
```

Expected output:

```text
Thin-Pod Node CDK alive
node tick 1
node tick 2
node tick 3
```

## Notes

The node and Gateway both use the same Zephyr board target. Use the distinct RTT output string to confirm which image is running.
