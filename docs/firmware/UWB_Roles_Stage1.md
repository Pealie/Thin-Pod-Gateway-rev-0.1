# Thin-Pod UWB Roles, Stage 1

## Purpose

This repository addition introduces the first staged rebuild of the Thin-Pod UWB roles after the RTT alive-test baseline.

It adds:

```text
firmware/node/uwb_responder
firmware/gateway/uwb_initiator
firmware/common/thinpod_protocol
tools/packet_logging/gateway_rtt_packet_parser.py
```

The current implementation is a protocol, firmware identity and logging harness. It builds and runs on the DWM3001CDK/nRF52833 using nRF Connect SDK v3.3.1, but it uses a stub transport rather than the real DW3110 UWB radio backend.

## Stage boundary

Stage 1 proves:

```text
Node responder firmware identity
Gateway initiator firmware identity
Shared packet contract
Synthetic vibration-window generation
Gateway packet logging
Vibration-window ingestion parser
SEGGER RTT runtime observation
```

Stage 1 does not yet prove:

```text
DW3110 radio initialisation
Real UWB poll/response exchange
DS-TWR timestamps
Range calculation
Channel 9 radio configuration
STS / SFD / preamble settings
```

## Known previous radio parameters

```text
Gateway:  INITIATOR, MAC 0x0000
Node:     RESPONDER, MAC 0x0001
Channel:  9
Mode:     DS_TWR_DEFERRED
PHY:      SP3
SFD:      2
Preamble: 10
STS IV:   01:02:03:04:05:06
```

These should be restored in the real DW3110 backend stage.

## Build commands

Node responder:

```cmd
cd /d C:\ncs\v3.3.1
west build -b decawave_dwm3001cdk -p always -d C:\ncs\v3.3.1\build_node_uwb_responder_stage1 C:\path\to\repo\firmware\node\uwb_responder
```

Gateway initiator:

```cmd
cd /d C:\ncs\v3.3.1
west build -b decawave_dwm3001cdk -p always -d C:\ncs\v3.3.1\build_gateway_uwb_initiator_stage1 C:\path\to\repo\firmware\gateway\uwb_initiator
```

## RTT outputs

Node:

```text
Thin-Pod Node UWB responder stage-1
TPNODE_RESPONDER_READY seq=<n> ...
```

Gateway:

```text
Thin-Pod Gateway UWB initiator stage-1
TPGW_POLL seq=<n> dst=0x0001
TPGW_PKT seq=<n> src=0x0001 dst=0x0000 ... rms_mg=<n> ...
```

## Real UWB backend requirements

To move from Stage 1 to true CDK-to-CDK UWB exchange, add or port the Qorvo DW3 / DW3110 driver layer and replace the stub transport with:

```text
Node:
  initialise DW3110
  configure Channel 9 / SP3 / SFD 2 / Preamble 10 / STS IV
  wait for Gateway poll
  return vibration-window response packet

Gateway:
  initialise DW3110
  configure matching PHY
  send poll
  receive vibration-window response packet
  log TPGW_PKT line
```

The shared packet structure and Gateway logging line in this commit are intended to survive that transition.
