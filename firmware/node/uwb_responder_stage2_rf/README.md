# Thin-Pod Node UWB Responder, Stage 1

Minimal staged responder application for the Thin-Pod node DWM3001CDK.

This is not yet the real DW3110 RF responder. It establishes node firmware identity and the packet contract that the real UWB backend will use.

## Build

```cmd
cd /d C:\ncs\v3.3.1
west build -b decawave_dwm3001cdk -p always -d C:\ncs\v3.3.1\build_node_uwb_responder_stage1 C:\path\to\repo\firmware\node\uwb_responder
```

## Flash

```cmd
west flash -d C:\ncs\v3.3.1\build_node_uwb_responder_stage1 -r jlink
```

Manual fallback:

```text
LoadFile C:\ncs\v3.3.1\build_node_uwb_responder_stage1\merged.hex
Reset
Go
```

## Expected RTT output

```text
Thin-Pod Node UWB responder stage-1
node_id=0x0001 role=responder backend=stub
TPNODE_RESPONDER_READY seq=1 src=0x0001 dst=0x0000 ...
```
