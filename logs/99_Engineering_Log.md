# 99 Engineering Log

**Date:** 11 July 2026  
**Repository:** `Pealie/Thin-Pod-Gateway-rev-0.1`  
**Branch:** `gateway-dwm-spis-get-capabilities`  
**Scope:** Gateway power-path investigation and TPHIP READY-line diagnostics

## Session summary

Today’s work substantially narrowed the remaining TPHIP host-interface fault.

The DWM3001-CDK and NUCLEO-N657X0-Q were powered independently through their own USB connections. Under this arrangement, the DWM endpoint repeatedly received valid 16-byte `GET_CAPABILITIES` requests from the NUCLEO, validated them correctly, and constructed the expected 32-byte responses. Normal requests and deliberately malformed requests produced the expected protocol status values, with guard checks remaining intact.

The physical request path is therefore established across:

- NUCLEO SPI5 clock;
- NUCLEO COPI/MOSI to DWM;
- chip select;
- DWM request parsing and CRC validation;
- DWM response construction.

The response phase remained out of synchronisation. The DWM consistently reported:

```text
response_callback_result=16
response_complete ret=-122
```

The DWM was armed for a 32-byte response, while only 16 clocks arrived. The request sequence observed by the DWM was `1, 3, 5, 7, 9, 1001, 1003, 1005`, showing that each intervening request was being consumed as a short response transaction. This establishes a repeatable two-transaction phase slip centred on the READY handshake.

## READY diagnostic firmware

A temporary DWM diagnostic was added to:

```text
firmware/dwm3001cdk_host_interface_stub/src/main.c
```

The READY GPIO was configured for output with input sensing enabled, and the physical GPIO state was read back immediately after READY was asserted.

The diagnostic firmware built successfully with:

```text
FLASH: 32264 B
RAM:    7488 B
```

Generated artefact hashes:

```text
zephyr.elf
SHA-256 7C97E0FB4519EC9583D8682B740EFB98348CBBD15329B37626D1E63701FBDF0E

merged.hex
SHA-256 235F109A3D7E01337A4EEC97599A83EB29A73323EC8036B874A7AA2BD5B81E60
```

The image was flashed successfully through J-Link probe `760203854`.

Across every response attempt, the DWM reported:

```text
ready_set_ret=0 ready_physical=1
```

This shows that the nRF52833 successfully commanded P0.28 high and sensed the pin as logically high at the processor.

## Electrical investigation

A loose resistor used earlier in the session was confirmed as approximately 10 kΩ. Resistance checks found no hard short between the principal power or READY nets:

```text
TP2 to TP1:                 approximately 2.9 kΩ to 4.4 kΩ
NUCLEO CN3.06 to GND:       approximately 0.96 MΩ
TP6 to TP1:                 open circuit
TP6 to TP2:                 open circuit
```

The 10 kΩ resistor itself therefore did not create a sustained short or damage the READY route. It remains removed.

The externally measured READY voltages initially appeared contradictory, including negative readings when DWM J10.9 was used as the ground reference. Subsequent inspection identified DWM J10.9 as a dry joint. Those negative READY readings are consequently invalid because the meter reference was floating.

The READY route from DWM J10.15 through TP6 to NUCLEO PB9 showed matching measured behaviour, while the firmware’s internal P0.28 readback remained high. The dry J10.9 ground joint now provides the most direct explanation for the contradictory voltage measurements.

## Power-path status

The shared Gateway 5 V path remains a separate unresolved issue.

Observed evidence included:

```text
TP1 to TP2:                 effectively 0 V under PoE
NUCLEO input from PoE:      0 V
NUCLEO CN3.06 from PoE:     0 V
NUCLEO CN3.06 from mains USB:
                              unstable, approximately 2.9 V to 4.4 V
```

Resistance checks did not indicate a hard short. The DWM and NUCLEO remain operational when powered independently through USB. The PoE-derived and carrier-distributed 5 V route requires a separate continuity and solder-joint investigation after the READY reference fault is corrected.

## Current evidence boundary

Established:

- DWM and NUCLEO firmware both boot.
- NUCLEO sends valid 16-byte requests across the physical SPI path.
- DWM validates normal and adverse requests correctly.
- DWM constructs the expected responses.
- DWM P0.28 is commanded high and reads back high internally.
- The response transfer remains truncated to 16 bytes because the NUCLEO does not progress into a valid 32-byte response read.
- DWM J10.9 has been identified as a dry ground joint.

Pending:

- reflow and verification of DWM J10.9;
- valid external READY voltage measurement;
- NUCLEO detection of READY at PB9;
- a complete 32-byte DWM-to-NUCLEO response transfer;
- `response_callback_result=32`;
- `response_complete ret=0`;
- restoration and verification of the shared 5 V power path;
- repeated cold-start confirmation.

No RF, production-security, secure-boot or full Gateway host-interface completion claim is made from today’s work.

## Next action

At the next bench session:

1. Disconnect all power sources.
2. Reflow DWM J10.9 with flux and minimal fresh solder.
3. Inspect adjacent J10 joints for bridges.
4. Confirm low-resistance continuity from J10.9 to TP1 and NUCLEO ground.
5. Power the DWM and NUCLEO independently through USB.
6. Run the existing diagnostic until READY remains asserted.
7. Measure READY at DWM J10.15, TP6 and NUCLEO PB9 using the repaired ground reference.
8. Confirm that all three points reach approximately 3.3 V.
9. Re-run the TPHIP suite and look for:

```text
response_callback_result=32
response_complete ret=0
```

10. Preserve the temporary firmware changes uncommitted until the diagnostic result is resolved and recorded.

## Worktree state

Two temporary diagnostic source changes remain intentionally uncommitted:

```text
firmware/dwm3001cdk_host_interface_stub/src/main.c
firmware/nucleo_dwm_host_interface_probe/src/main.c
```

The engineering log should be committed independently so that these diagnostic changes remain available for the next bench session without being folded into the documentation commit.
