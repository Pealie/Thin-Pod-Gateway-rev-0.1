# DW3110 One-Way RF Exchange Validation

**Date:** 2026-07-12

**Direction:** Gateway DWM3001-CDK to Node DWM3001-CDK

**Source commit:** `5736322d3e6d319d7b9ce3f43bd5f23695cbf047`

**Method:** Polled DW3110 transmit and receive status, deterministic 24-byte payload, DW3110-generated two-byte FCS, RTT evidence

## Result

```text
PASS
```

The Gateway-side DW3110 transmitted a complete 20-frame suite. The Node-side
DW3110 received and validated the same sequence window with no invalid frames,
receive timeouts, receive errors, sequence gaps or duplicate frames in the
selected matched suite.

## Matched sequence window

```text
first_seq=41 last_seq=60
```

## Gateway summary

```text
TPRF_TX_SUMMARY role=gateway suite=3 attempted=20 pass=20 fail=0 first_seq=41 last_seq=60 result=PASS
```

Frame records found across the complete Gateway capture:

```text
TPRF_TX result=PASS: 535
TPRF_TX result=FAIL: 0
```

## Node summary

```text
TPRF_RX_SUMMARY role=node suite=3 valid=20 invalid=0 timeouts=0 errors=0 gaps=0 duplicates=0 first_seq=41 last_seq=60 result=PASS
```

Frame records found across the complete Node capture:

```text
TPRF_RX result=PASS: 580
TPRF_RX result=FAIL: 2
```

## Raw RTT evidence

```text
SHA256  E785C5474F3F6E4E11F51E46A6EC7C98B5CDBE5A2875A65E7A3051CCEF8117ED  gateway-dwm3001cdk-rf-rtt.log
SHA256  2C4570004CE481DEEE3967B7AD15DEDAE000203CD99C241BA22D5EAF33A8EFAC  node-dwm3001cdk-rf-rtt.log
```

## Build artefact hashes

```text
SHA256  06581830CA9AB1E0ADB8729C6B8848AFF47EBEB89EEAE269FE2414B7D598B2F4  C:\ncs\v3.3.1\build_thinpod_node_dw3110_rf\uwb_responder_stage2_rf\zephyr\zephyr.elf
SHA256  9C6189A03506A3548DF68A358C183F818E155004FA028D88A57776FFCD7BF57A  C:\ncs\v3.3.1\build_thinpod_node_dw3110_rf\merged.hex
SHA256  2EE1AAB862E1AC207826B69A7CB770D44B21BABE2F38E8843CC21C44A14F9866  C:\ncs\v3.3.1\build_thinpod_gateway_dw3110_rf\uwb_initiator_stage2_rf\zephyr\zephyr.elf
SHA256  A6316CE6B9A4590B1B5742468931F0C67DBAD72AD9A8F745EF0557C11E2B395A  C:\ncs\v3.3.1\build_thinpod_gateway_dw3110_rf\merged.hex
```

## Validated claim

The Thin-Pod Gateway-side DWM3001-CDK transmitted deterministic UWB frames
through its internal DW3110, and the Node-side DWM3001-CDK received those
frames through its internal DW3110, verified the exact frame length and payload,
and completed a repeatable 20-frame one-way RF exchange.

## Evidence boundary

This validation does not establish bidirectional exchange, ranging,
vibration-window transport, telemetry transport, IRQ-driven receive handling,
RF range or link budget, production TX-power calibration, antenna performance,
coexistence performance, security properties or regulatory compliance. IRQ
levels remain observations only; event completion is established from DW3110
status bits.
