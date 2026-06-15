# Thin-Pod UWB Roles Stage 1: Responder, Initiator, Packet Logging and Vibration-Window Ingestion

## Executive summary

This document summarises the Stage 1 implementation package prepared for the next Thin-Pod firmware layer after the Gateway and node RTT alive-test milestones.

The package introduces a staged firmware harness for the Thin-Pod node responder and Gateway initiator. It does not yet implement the final DW3110 / DWM3001C RF exchange. Instead, it establishes the firmware identities, shared packet contract, Gateway packet logging format and vibration-window ingestion path that the real UWB backend can later connect to.

This is the correct modular step between ‘both CDKs are alive under Zephyr and RTT’ and ‘real CDK-to-CDK UWB packet exchange’. It makes the next layer explicit, testable and repo-ready.

## Repository package

The generated package is:

```text
ThinPod_UWB_Roles_Stage1_Commit.zip
```

It contains:

```text
firmware/common/thinpod_protocol/
  thinpod_protocol.h
  thinpod_protocol.c
  README.md

firmware/node/uwb_responder/
  CMakeLists.txt
  prj.conf
  README.md
  src/main.c

firmware/gateway/uwb_initiator/
  CMakeLists.txt
  prj.conf
  README.md
  src/main.c

docs/firmware/UWB_Roles_Stage1.md

tools/packet_logging/
  gateway_rtt_packet_parser.py
  sample_gateway_rtt.log

COMMIT_MESSAGE.txt
```

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

This distinction is important. The package is not pretending that the RF layer is complete. It creates a clean structure into which the real Qorvo DW3 / DW3110 transport can be inserted.

## Shared protocol

The common protocol layer is located at:

```text
firmware/common/thinpod_protocol/
```

It defines:

```text
Gateway ID:      0x0000
Node ID:         0x0001
Message type:    vibration window
Samples:         32
Sample units:    int16_t milligravity
Sample rate:     1600 Hz
Checksum:        lightweight 32-bit software checksum
```

The packet carries:

```text
protocol version
message type
sequence number
source ID
destination ID
uptime timestamp
sample rate
sample count
acceleration samples
checksum
```

The synthetic vibration-window generator is deterministic by design. This gives repeatable packet content for Gateway logging and parser testing before real ADXL1005 sampling is restored.

## Node responder

The node responder application is located at:

```text
firmware/node/uwb_responder/
```

Its current role is to establish the node-side responder identity and generate the packet payload that the real UWB responder will eventually transmit.

Expected RTT output:

```text
Thin-Pod Node UWB responder stage-1
node_id=0x0001 role=responder backend=stub
status=ready waiting_for_gateway_poll
TPNODE_RESPONDER_READY seq=1 src=0x0001 dst=0x0000 samples=32 rate=1600 checksum=...
```

Build command:

```cmd
cd /d C:\ncs\v3.3.1
west build -b decawave_dwm3001cdk -p always -d C:\ncs\v3.3.1\build_node_uwb_responder_stage1 C:\path\to\repo\firmware\node\uwb_responder
```

Flash command:

```cmd
west flash -d C:\ncs\v3.3.1\build_node_uwb_responder_stage1 -r jlink
```

Manual J-Link fallback:

```text
LoadFile C:\ncs\v3.3.1\build_node_uwb_responder_stage1\merged.hex
Reset
Go
```

## Gateway initiator

The Gateway initiator application is located at:

```text
firmware/gateway/uwb_initiator/
```

Its current role is to establish the Gateway-side initiator identity, emit synthetic poll traces, ingest vibration-window packets and log them in a parser-friendly key-value format.

Expected RTT output:

```text
Thin-Pod Gateway UWB initiator stage-1
gateway_id=0x0000 role=initiator backend=stub
packet_log_format=TPGW_PKT key=value
TPGW_POLL seq=1 dst=0x0001
TPGW_PKT seq=1 src=0x0001 dst=0x0000 uptime_ms=... valid=0 samples=32 rate=1600 first_mg=... min_mg=... max_mg=... mean_mg=... rms_mg=... checksum=...
```

Build command:

```cmd
cd /d C:\ncs\v3.3.1
west build -b decawave_dwm3001cdk -p always -d C:\ncs\v3.3.1\build_gateway_uwb_initiator_stage1 C:\path\to\repo\firmware\gateway\uwb_initiator
```

Flash command:

```cmd
west flash -d C:\ncs\v3.3.1\build_gateway_uwb_initiator_stage1 -r jlink
```

Manual J-Link fallback:

```text
LoadFile C:\ncs\v3.3.1\build_gateway_uwb_initiator_stage1\merged.hex
Reset
Go
```

## Gateway packet logging

The Gateway emits packet logs in this form:

```text
TPGW_PKT seq=1 src=0x0001 dst=0x0000 uptime_ms=... valid=0 samples=32 rate=1600 first_mg=... min_mg=... max_mg=... mean_mg=... rms_mg=... checksum=...
```

This is deliberately simple. It can be copied from SEGGER RTT Viewer logs and parsed without requiring a full binary telemetry stack at this stage.

The packet parser is located at:

```text
tools/packet_logging/gateway_rtt_packet_parser.py
```

Example use:

```cmd
python tools\packet_logging\gateway_rtt_packet_parser.py tools\packet_logging\sample_gateway_rtt.log --csv gateway_packets.csv
```

The parser extracts `TPGW_PKT` lines and can export CSV for later analysis.

## Vibration-window ingestion

The Gateway application already computes basic packet statistics from the synthetic window:

```text
first sample
minimum
maximum
mean
RMS
checksum validation status
```

This is the first step toward the later DSP / TinyML pipeline. It does not yet perform FFT, envelope detection, crest factor, kurtosis or anomaly scoring. The value of this stage is that it establishes the ingestion contract.

## Known UWB parameters to restore

The earlier successful CDK-to-CDK bench demo used:

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

These settings should be restored when the real DW3110 backend is introduced.

## Implementation strategy

The next implementation stage is to replace the stub transport while preserving the existing packet contract and logging line.

Node-side real transport should:

```text
initialise DW3110
configure Channel 9 / SP3 / SFD 2 / Preamble 10 / STS IV
wait for Gateway poll
populate a vibration-window packet
transmit the packet over UWB
emit responder trace output over RTT
```

Gateway-side real transport should:

```text
initialise DW3110
configure matching PHY settings
send poll to node ID 0x0001
receive vibration-window response
validate checksum
log TPGW_PKT line
preserve packet parser compatibility
```

## Why this matters

This package turns the project’s next step into a controlled engineering layer rather than a leap straight from ‘RTT works’ to ‘UWB plus vibration analytics’. It preserves the Thin-Pod modular ethos:

```text
CDK alive-test
  -> responder / initiator firmware identities
  -> packet contract
  -> Gateway packet logging
  -> vibration-window ingestion
  -> real DW3110 transport
  -> real ADXL1005 sampling
  -> DSP feature extraction
  -> TinyML / anomaly scoring
```

The immediate benefit is that each layer can now be tested, committed, discussed and replaced without collapsing the project into one monolithic firmware problem.

## Suggested commit message

```text
Add staged UWB responder and initiator firmware harness

Adds the first staged rebuild of Thin-Pod UWB roles after the RTT alive-test baseline.

Included:
- firmware/common/thinpod_protocol
- firmware/node/uwb_responder
- firmware/gateway/uwb_initiator
- docs/firmware/UWB_Roles_Stage1.md
- tools/packet_logging/gateway_rtt_packet_parser.py

The node responder and Gateway initiator currently use a stub transport while preserving the intended firmware identities, packet contract, Gateway packet logging format and vibration-window ingestion path.

This does not yet implement real DW3110 RF exchange. It creates the stable interface to be connected to the Qorvo DW3/DW3110 backend in the next stage.
```

## Conclusion

The Stage 1 UWB roles package is repo-ready. It should be committed as a disciplined firmware harness rather than as final RF functionality. Its purpose is to make the next phase testable: node responder, Gateway initiator, CDK-to-CDK exchange, Gateway packet logging and vibration-window ingestion, with the real DW3110 radio backend added as the next controlled layer.
