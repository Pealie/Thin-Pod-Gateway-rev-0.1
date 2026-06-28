# Thin-Pod Gateway UWB Initiator, Stage 1

Minimal staged initiator application for the Thin-Pod Gateway DWM3001CDK.

This is not yet the real DW3110 RF initiator. It establishes Gateway firmware identity, packet logging format and vibration-window ingestion contract.

## Build

```cmd
cd /d C:\ncs\v3.3.1
west build -b decawave_dwm3001cdk -p always -d C:\ncs\v3.3.1\build_gateway_uwb_initiator_stage1 C:\path\to\repo\firmware\gateway\uwb_initiator
```

## Flash

```cmd
west flash -d C:\ncs\v3.3.1\build_gateway_uwb_initiator_stage1 -r jlink
```

Manual fallback:

```text
LoadFile C:\ncs\v3.3.1\build_gateway_uwb_initiator_stage1\merged.hex
Reset
Go
```

## Expected RTT output

```text
Thin-Pod Gateway UWB initiator stage-1
gateway_id=0x0000 role=initiator backend=stub
TPGW_POLL seq=1 dst=0x0001
TPGW_PKT seq=1 src=0x0001 dst=0x0000 ... rms_mg=...
```
