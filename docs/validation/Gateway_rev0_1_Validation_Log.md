# Thin-Pod Gateway rev 0.1 Validation Log

## Purpose

This document is the validation evidence ledger for the Thin-Pod Gateway rev 0.1 completion checklist.

This document directly supports Gateway completion checklist Item 7: one validation log.

No new Gateway architecture scope is introduced here. This document records evidence only for the fixed Gateway rev 0.1 completion checklist.

## Validation scope

Gateway rev 0.1 completion is limited to the following evidence items:

1. stable hardware bring-up note;
2. final verified pin map;
3. minimal SPI/GPIO probe app;
4. one defined telemetry packet;
5. one node-to-Gateway transfer;
6. one parser or logging script;
7. one validation log;
8. one GitHub Actions or scripted check;
9. one tagged Gateway release;
10. OSHWA certification preparation.

This validation log records the status of those items and links each validation result to a concrete repo artefact, command output, measurement, capture file, or release step.

## Validation status summary

| Item | Completion evidence | Current status | Evidence location |
|---:|---|---|---|
| 1 | Stable hardware bring-up note | Started, awaiting final measured values | `docs/bringup/Gateway_rev0_1_Hardware_Bringup_Note.md` |
| 2 | Final verified pin map | Started, awaiting final continuity/probe entries | `docs/firmware/NUCLEO_SPI5_Gateway_PCB_Pin_Mapping.md` |
| 3 | Minimal SPI/GPIO probe app | Scaffold committed, hardware-binding validation still pending | `firmware/gateway_probe/` |
| 4 | One defined telemetry packet | Defined and committed | `docs/protocol/ThinPod_Gateway_Telemetry_Packet_v1.md` |
| 5 | One node-to-Gateway transfer | Not yet executed | `logs/` capture file TBD |
| 6 | One parser or logging script | Implemented and tested against sample packet | `scripts/gateway_packet_logger.py` |
| 7 | One validation log | This document | `docs/validation/Gateway_rev0_1_Validation_Log.md` |
| 8 | One scripted check | Implemented and passing in current-stage mode | `scripts/check_gateway_release_artifacts.py` |
| 9 | One tagged Gateway release | Not yet created | Git tag TBD |
| 10 | OSHWA certification preparation | Not yet created | `docs/oshwa/Gateway_OSHWA_Preparation_Checklist.md` TBD |

## Hardware under validation

| Item | Detail |
|---|---|
| Project | Thin-Pod Gateway |
| Hardware revision | rev 0.1 |
| Main controller | STM32 NUCLEO-N657X0-Q |
| UWB module | DWM3001-CDK |
| Wi-Fi / auxiliary module | Seeed XIAO ESP32-C6 |
| Power source | NUCLEO powered from PoE splitter |
| PCB role | Gateway carrier/interface PCB for module integration and telemetry path proof |
| Validation branch | `gateway-rev0.1-completion-checklist` |

## Validation environment

| Field | Value |
|---|---|
| Operator | TBD |
| Date | 2026-06-27 initial shell |
| Location | Bench |
| Host PC | Windows PC |
| Repository path | `C:\ThinPod\Thin-Pod-Gateway-rev-0.1` |
| Git branch | `gateway-rev0.1-completion-checklist` |
| Python command | `python` |
| Zephyr / west status | `west` not currently available in active CMD PATH |
| Notes | Firmware scaffold committed. Live hardware firmware build/probe still pending. |

## Test 1: stable hardware bring-up

### Completion checklist link

This test supports Item 1: stable hardware bring-up note.

### Evidence artefact

`docs/bringup/Gateway_rev0_1_Hardware_Bringup_Note.md`

### Expected result

The Gateway rev 0.1 PCB should power the attached development modules without requiring a temporary jumper workaround for the DWM3001-CDK ground return.

Expected evidence:

- NUCLEO powers from the PoE splitter.
- DWM3001-CDK receives its intended supply from the Gateway PCB.
- XIAO ESP32-C6 receives its intended 3.3 V supply from the Gateway PCB.
- Gateway ground continuity is verified.
- The earlier DWM3001-CDK ground return issue is corrected.
- No unexpected heating, smoke, brownout, reset loop, or unstable power behaviour is observed.

### Observed result

TBD. Final measured voltages and observations are to be transferred from the hardware bring-up note once bench measurements are complete.

### Result

TBD.

## Test 2: final verified pin map

### Completion checklist link

This test supports Item 2: final verified pin map.

### Evidence artefact

`docs/firmware/NUCLEO_SPI5_Gateway_PCB_Pin_Mapping.md`

### Expected result

The Gateway rev 0.1 pin map should be verified by continuity checks, powered checks, GPIO probing, and SPI activity where applicable.

Expected signal coverage:

| Signal | Verification required | Result |
|---|---|---|
| SPI5_SCK | Continuity plus SPI probe | TBD |
| SPI5_MISO | Continuity plus SPI probe | TBD |
| SPI5_MOSI | Continuity plus SPI probe | TBD |
| DWM_CS | Continuity plus GPIO toggle | TBD |
| DWM_IRQ | Continuity plus GPIO read | TBD |
| DWM_RESET | Continuity plus GPIO toggle | TBD |
| DWM_5V | Powered voltage check | TBD |
| DWM_GND | Continuity check | TBD |
| C6_3V3 | Powered voltage check | TBD |
| C6_GND | Continuity check | TBD |

### Observed result

TBD. Final measurements and probe observations are to be added after the next bench verification session.

### Result

TBD.

## Test 3: minimal SPI/GPIO probe app

### Completion checklist link

This test supports Item 3: minimal SPI/GPIO probe app.

### Evidence artefact

`firmware/gateway_probe/`

### Expected result

The minimal Gateway probe app should demonstrate firmware-level access to the routed DWM3001-CDK interface signals.

Expected probe behaviour:

- boot banner or heartbeat output appears;
- DWM_RESET can be toggled;
- DWM_CS can be toggled;
- DWM_IRQ can be sampled as an input;
- SPI5 can transmit a short test frame while chip select is asserted.

### Current result

The probe app scaffold has been committed.

Current limitation:

- `west` is not currently available in the active Windows CMD PATH.
- GPIO/SPI device-tree or board-specific binding validation remains pending.
- No live SPI/GPIO hardware probe evidence has yet been recorded in this validation log.

### Result

Partial. Scaffold committed. Live probe validation pending.

## Test 4: telemetry packet v1 definition

### Completion checklist link

This test supports Item 4: one defined telemetry packet.

### Evidence artefacts

- `docs/protocol/ThinPod_Gateway_Telemetry_Packet_v1.md`
- `samples/telemetry_packet_v1_sample.hex`

### Expected result

Telemetry Packet v1 should define one fixed, parseable packet suitable for the first Gateway transfer test.

Expected properties:

- fixed length of 24 bytes;
- ASCII `TP` magic bytes;
- version field;
- packet type field;
- node identifier;
- sequence number;
- uptime value;
- battery/supply estimate;
- simple RMS and peak sample values;
- status flags;
- CRC16 field.

### Observed result

Telemetry Packet v1 has been defined and committed.

The sample packet is:

```text
54 50 01 01 01 00 2A 00 00 00 40 E2 01 00 E4 0C 7D 00 9A 01 00 00 XX XX
```

The sample packet decodes as:

| Field | Value |
|---|---:|
| magic | TP |
| version | 1 |
| packet_type | telemetry |
| node_id | 1 |
| sequence | 42 |
| uptime_ms | 123456 |
| battery_mv | 3300 |
| sample_rms_mg | 125 |
| sample_peak_mg | 410 |
| status_flags | 0 |
| crc_field_hex | XX XX |
| crc_expected_hex | 60 03 |
| crc_expected_value | 864 |
| crc_valid | not_checked |

### Result

PASS for packet definition and sample packet structure.

## Test 5: node-to-Gateway transfer

### Completion checklist link

This test supports Item 5: one node-to-Gateway transfer.

### Evidence artefact

TBD. Expected future capture path:

`logs/gateway_transfer_capture_YYYYMMDD.txt`

### Expected result

At least one Telemetry Packet v1 payload should be transferred from a Thin-Pod node or DWM3001-CDK node-side test sender to the Gateway side.

Minimum acceptable evidence:

- raw received packet visible in Gateway capture;
- packet begins with `54 50`;
- packet length is 24 bytes;
- packet includes a visible node ID;
- packet includes a visible sequence number;
- packet can be parsed by `scripts/gateway_packet_logger.py`.

Preferred evidence:

- sequence numbers 1 to 10 sent;
- all 10 packets received;
- no missing sequence numbers;
- parser output saved as CSV or JSONL.

### Observed result

Not yet executed.

### Result

TBD.

## Test 6: parser or logging script

### Completion checklist link

This test supports Item 6: one parser or logging script.

### Evidence artefact

`scripts/gateway_packet_logger.py`

### Test command

```bat
python scripts\gateway_packet_logger.py samples\telemetry_packet_v1_sample.hex
```

### Observed output summary

The parser successfully decoded the sample packet and reported:

| Field | Value |
|---|---:|
| node_id | 1 |
| sequence | 42 |
| uptime_ms | 123456 |
| battery_mv | 3300 |
| sample_rms_mg | 125 |
| sample_peak_mg | 410 |
| crc_field_hex | XX XX |
| crc_expected_hex | 60 03 |
| crc_expected_value | 864 |
| crc_valid | not_checked |

### Result

PASS.

## Test 7: validation log

### Completion checklist link

This test supports Item 7: one validation log.

### Evidence artefact

`docs/validation/Gateway_rev0_1_Validation_Log.md`

### Expected result

The repository should contain one central validation log that records current evidence, open validation items, test commands, and result status for Gateway rev 0.1 completion.

### Observed result

This document has been created as the Gateway rev 0.1 validation evidence ledger.

### Result

PASS for validation-log shell creation. Final PASS status depends on completion of pending hardware, transfer, release, and OSHWA entries.

## Test 8: scripted release artefact check

### Completion checklist link

This test supports Item 8: one GitHub Actions or scripted check.

### Evidence artefact

`scripts/check_gateway_release_artifacts.py`

### Test command

```bat
python scripts\check_gateway_release_artifacts.py
```

### Observed output summary

Current-stage artefact check passed with:

| Result type | Count |
|---|---:|
| Passes | 44 |
| Warnings | 3 |
| Failures | 0 |

The three expected warnings were:

- `docs/validation/Gateway_rev0_1_Validation_Log.md` not yet present;
- `docs/oshwa/Gateway_OSHWA_Preparation_Checklist.md` not yet present;
- `.github/workflows/gateway-checks.yml` not yet present.

After this validation log is committed, the first warning should no longer appear.

### Result

PASS for current-stage scripted check.

## Test 9: tagged Gateway release

### Completion checklist link

This test supports Item 9: one tagged Gateway release.

### Expected result

A tagged Gateway release should exist after the completion evidence is ready.

Suggested tag:

```text
v0.1.0
```

Suggested release title:

```text
Thin-Pod Gateway rev 0.1 - Hardware Bring-Up and Telemetry Proof
```

### Observed result

Not yet tagged.

### Result

TBD.

## Test 10: OSHWA certification preparation

### Completion checklist link

This test supports Item 10: OSHWA certification preparation.

### Evidence artefact

TBD. Expected future path:

`docs/oshwa/Gateway_OSHWA_Preparation_Checklist.md`

### Expected result

The Gateway repo should contain an OSHWA preparation checklist covering:

- hardware source files;
- fabrication outputs;
- BOM;
- assembly notes;
- photographs;
- firmware/software boundary;
- licence files;
- third-party module boundary statement;
- release tag;
- documentation completeness.

### Observed result

Not yet created.

### Result

TBD.

## Current validation conclusion

As of this validation-log shell, the Gateway rev 0.1 completion branch has moved from loose development evidence to a structured release-evidence pipeline.

The following items have committed artefacts:

- Item 1: hardware bring-up note;
- Item 2: pin-map verification document;
- Item 3: minimal probe app scaffold;
- Item 4: telemetry packet definition;
- Item 6: parser/logging script;
- Item 7: validation log shell;
- Item 8: scripted artefact check.

The following items still require completion evidence:

- Item 1: final measured hardware values;
- Item 2: final measured pin-map/probe verification;
- Item 3: live SPI/GPIO probe evidence;
- Item 5: node-to-Gateway transfer capture;
- Item 7: final validation updates after live testing;
- Item 9: tagged Gateway release;
- Item 10: OSHWA certification preparation.

## Open actions

| Priority | Action | Related item |
|---:|---|---:|
| 1 | Fill in final measured values in hardware bring-up note | 1 |
| 2 | Replace pin-map `TBD` entries with continuity/probe results | 2 |
| 3 | Build/run the minimal SPI/GPIO probe app or equivalent hardware probe | 3 |
| 4 | Capture one real node-to-Gateway Telemetry Packet v1 transfer | 5 |
| 5 | Parse the live transfer capture with `gateway_packet_logger.py` | 5, 6 |
| 6 | Update this validation log with live evidence | 7 |
| 7 | Add GitHub Actions workflow for scripted check | 8 |
| 8 | Add OSHWA preparation checklist | 10 |
| 9 | Run release-mode checks | 8, 9 |
| 10 | Tag Gateway rev 0.1 release | 9 |

## Revision history

| Date | Change |
|---|---|
| 2026-06-27 | Initial validation log shell created |
