# DWM3001-CDK TPHIP SPIS endpoint

Runs on the Gateway DWM3001-CDK.

This application configures the external J10 host-interface pins as an nRF52833 SPIS endpoint and implements the first Thin-Pod Host Interface Protocol proof:

- 16-byte `GET_CAPABILITIES` request;
- 32-byte physical response;
- matching request/response sequence;
- CRC-16/CCITT-FALSE;
- fixed static buffers with guard words;
- explicit rejection of unknown versions, reserved flags, oversized payloads, bad CRCs and unknown opcodes;
- P0.28 READY assertion only after the response transfer is armed.

External host-interface mapping:

```text
P0.31  SCK
P0.27  COPI / MOSI / SPIS input
P0.07  CIPO / MISO / SPIS output
P0.30  active-low CS
P0.28  READY
P0.18  external reset
```

## Supported READY readback diagnostic

READY is configured as an output with input sensing enabled.

Input sensing is retained solely so the bring-up firmware can read back the physical level driven on P0.28 and report:

```text
tphip_dwm ready_set_ret=<return value> ready_physical=<0 or 1>
```

The readback is observability evidence. Protocol flow does not depend on the reported level.

This diagnostic was present in the firmware used for the successful PoE-powered physical validation. Retaining it keeps the committed source aligned with the tested build.

## Current capability boundary

The first capabilities response advertises IRQ and CRC support only.

Status, counters, record transfer, UWB RF transport, authentication and production secure boot remain outside this proof.
