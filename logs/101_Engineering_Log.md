# 101 Engineering Log: Matched DW3110 One-Way RF Exchange

**Date:** 12 July 2026
**Repository:** `Pealie/Thin-Pod-Gateway-rev-0.1`
**Source branch:** `gateway-dw3110-rf-exchange`
**Merged pull request:** `#7 Validate matched DW3110 one-way Gateway-to-Node RF exchange`
**Merge commit:** `dbe355b80a2afed867bcaa09a2f3f1b23abea8e5`
**Scope:** Physical one-way UWB frame transmission from the Gateway-side DWM3001-CDK to the Node-side DWM3001-CDK, matched sequence validation, raw RTT evidence preservation, build provenance and merge into `main`

## Summary

This milestone established a real one-way UWB RF exchange between the two Thin-Pod DWM3001-CDK boards.

The Gateway-side board configured its internal DW3110 as the transmitter. The Node-side board configured its internal DW3110 as the receiver. Both applications ran under Zephyr using Nordic nRF Connect SDK v3.3.1 and the external Qorvo `DW3\_QM33\_SDK\_1.1.0` driver.

The proof used deterministic 24-byte application payloads with a DW3110-generated two-byte frame check sequence. Each record included role, sequence number, source and destination identifiers, payload length, uptime marker and checksum. The Gateway transmitted repeated 20-frame suites at 500 ms intervals. The Node validated frame length, addressing, payload content, checksum and sequence progression.

The decisive evidence is a matched transmitter and receiver sequence window:

```text
first\_seq=41
last\_seq=60
frames=20
```

Gateway result:

```text
TPRF\_TX\_SUMMARY role=gateway suite=3 attempted=20 pass=20 fail=0 first\_seq=41 last\_seq=60 result=PASS
```

Node result:

```text
TPRF\_RX\_SUMMARY role=node suite=3 valid=20 invalid=0 timeouts=0 errors=0 gaps=0 duplicates=0 first\_seq=41 last\_seq=60 result=PASS
```

The same twenty sequence numbers were therefore transmitted by the Gateway-side DW3110 and received and validated by the Node-side DW3110.

## Relationship to Engineering Log 100

Engineering Log 100 established physical SPI communication, DW3110 identity, IDLE\_RC entry and successful initialisation on both boards.

Engineering Log 101 closes the next stated firmware gate:

```text
one-way UWB frame transmission and reception
```

The evidence chain has progressed from physical device access to an actual over-the-air exchange with correlated transmitter and receiver records.

## Firmware and build progression

The RF work was preserved through three main commits:

```text
5a14e3f25226f8d49e7431ec8381c82bf5c8476b
Implement DW3110 one-way RF frame proof

5736322d3e6d319d7b9ce3f43bd5f23695cbf047
Adapt RF proof to QM33 DW3 driver APIs

700e7f5c2ced13e50d46b8f2c1f48415d70fc6dd
Record matched DW3110 one-way RF exchange
```

The implementation introduced:

```text
firmware/common/qorvo\_dw3000/include/thinpod\_dw3000\_rf\_proof.h
firmware/common/qorvo\_dw3000/src/thinpod\_dw3000\_rf\_proof.c
firmware/gateway/uwb\_initiator\_stage2\_rf/
firmware/node/uwb\_responder\_stage2\_rf/
scripts/check\_dw3110\_rf\_exchange.py
docs/firmware/DW3110\_One\_Way\_RF\_Exchange\_Procedure.md
docs/firmware/UWB\_RF\_Proof\_Stage2.md
```

The first build exposed legacy Qorvo API names that were absent from the QM33 SDK. The RF proof was adapted to the supported APIs:

```text
dwt\_readsysstatuslo()
dwt\_writesysstatuslo()
dwt\_getframelength()
```

The receiver recovery path retained `dwt\_forcetrxoff()` and removed the unavailable legacy `dwt\_rxreset()` call.

Both applications then built successfully.

Node receiver build:

```text
FLASH: 54,624 bytes of 512 KB, 10.42%
RAM:    9,280 bytes of 128 KB, 7.08%
Result: PASS
```

Gateway transmitter build:

```text
FLASH: 53,672 bytes of 512 KB, 10.24%
RAM:    9,280 bytes of 128 KB, 7.08%
Result: PASS
```

## Physical test configuration

The physical proof used two distinct DWM3001-CDK boards:

```text
Gateway transmitter J-Link serial: 760203854
Node receiver J-Link serial:       760222856
```

RF configuration:

```text
Channel:             5
Preamble length:     128
PAC:                 8
TX preamble code:    9
RX preamble code:    9
SFD:                 DWT\_SFD\_DW\_8
Data rate:           6.8 Mbps
PHR mode:            standard
STS:                 disabled
PDOA:                disabled
Application payload: 24 bytes
Air frame length:    26 bytes including hardware FCS
Frame interval:      500 ms
Suite length:        20 frames
```

The Node receiver was started first and placed into repeated bounded receive windows. The Gateway transmitter was then flashed, logged and reset while both SEGGER RTT loggers were active.

## Evidence package

The validation report was generated at:

```text
docs/validation/DW3110\_One\_Way\_RF\_Exchange\_Validation.md
```

Raw evidence was stored under:

```text
logs/validation/dw3110-one-way-rf/
```

The evidence package contains:

```text
build-artifact-sha256.txt
gateway-dwm3001cdk-rf-rtt.log
node-dwm3001cdk-rf-rtt.log
source-state.txt
```

Raw RTT hashes:

```text
SHA256  E785C5474F3F6E4E11F51E46A6EC7C98B5CDBE5A2875A65E7A3051CCEF8117ED  gateway-dwm3001cdk-rf-rtt.log
SHA256  2C4570004CE481DEEE3967B7AD15DEDAE000203CD99C241BA22D5EAF33A8EFAC  node-dwm3001cdk-rf-rtt.log
```

Build artefact hashes:

```text
SHA256  06581830CA9AB1E0ADB8729C6B8848AFF47EBEB89EEAE269FE2414B7D598B2F4  node zephyr.elf
SHA256  9C6189A03506A3548DF68A358C183F818E155004FA028D88A57776FFCD7BF57A  node merged.hex
SHA256  2EE1AAB862E1AC207826B69A7CB770D44B21BABE2F38E8843CC21C44A14F9866  gateway zephyr.elf
SHA256  A6316CE6B9A4590B1B5742468931F0C67DBAD72AD9A8F745EF0557C11E2B395A  gateway merged.hex
```

## Automated evidence validation

The evidence checker was strengthened to require a passing Gateway summary and a passing Node summary with identical `first\_seq` and `last\_seq` values.

Final checker output:

```text
gateway\_summary=TPRF\_TX\_SUMMARY role=gateway suite=3 attempted=20 pass=20 fail=0 first\_seq=41 last\_seq=60 result=PASS
node\_summary=TPRF\_RX\_SUMMARY role=node suite=3 valid=20 invalid=0 timeouts=0 errors=0 gaps=0 duplicates=0 first\_seq=41 last\_seq=60 result=PASS
matched\_sequence\_window=first\_seq=41 last\_seq=60
result=PASS
```

The complete captures contained:

```text
Gateway TX PASS records: 535
Gateway TX FAIL records:   0
Node RX PASS records:     580
Node RX FAIL records:       2
```

The two Node failures occurred outside the selected matched suite.

The first was a valid frame with a restarted sequence number and `sequence\_valid=0`. The second was a single receiver error without a payload. Both records remain preserved in the raw evidence. The selected sequence window 41-60 contains a clean twenty-frame exchange with zero invalid frames, timeouts, receiver errors, sequence gaps or duplicates.

## Validated claim

The evidence supports the following claim:

```text
The Thin-Pod Gateway-side DWM3001-CDK transmitted deterministic UWB frames through its internal DW3110, and the Node-side DWM3001-CDK received the same matched sequence window through its internal DW3110, verified the exact frame length, addressing, payload, checksum and sequence progression, and completed a repeatable 20-frame one-way RF exchange.
```

## Evidence boundary

The present evidence establishes:

* Gateway-to-Node one-way UWB frame transport;
* successful physical transmission and reception through two internal DW3110 devices;
* deterministic payload integrity;
* checksum agreement;
* sequence-window correlation;
* repeatable twenty-frame suites;
* polled DW3110 status handling;
* raw-log and build-artifact provenance.

The following remain future work:

* Node-to-Gateway one-way exchange in the intended sensor-to-Gateway direction;
* bidirectional exchange;
* ranging;
* vibration-window transport;
* Thin-Pod telemetry transport;
* IRQ-driven receive handling;
* RF range and link-budget validation;
* production transmit-power calibration;
* antenna and coexistence performance;
* security properties;
* regulatory compliance.

## Repository integration

The feature branch was pushed as:

```text
gateway-dw3110-rf-exchange
```

Pull request:

```text
#7 Validate matched DW3110 one-way Gateway-to-Node RF exchange
```

The pull request contained ten commits, passed the repository check and reported no conflict with `main`.

It was merged into `main` as:

```text
dbe355b80a2afed867bcaa09a2f3f1b23abea8e5
```

This places the physical RF implementation, validation report, raw evidence, artefact hashes and automated matched-suite checker in the main Gateway repository history.

## Engineering significance

The Gateway work has advanced from board-level bring-up and transceiver initialisation to a measured radio link between two physical Thin-Pod UWB subsystems.

The milestone demonstrates that the Zephyr platform layer, external Qorvo driver integration, deterministic frame format, transmitter path, receiver path and evidence workflow operate together successfully.

The result also confirms the value of the current evidence discipline:

1. define the smallest defensible claim;
2. build both roles reproducibly;
3. test on physical hardware;
4. retain raw output;
5. hash source-linked artefacts;
6. automate the acceptance criteria;
7. preserve anomalous records;
8. merge only after a matched result is established.

## Next concrete milestone

The next justified RF gate is:

```text
Node-to-Gateway one-way UWB frame transmission and reception
```

That milestone should reuse the existing PHY configuration, deterministic payload format, matched-sequence checker and evidence package structure while reversing the radio roles.

Once the intended Node-to-Gateway direction is established, the following gate can introduce a bounded Thin-Pod telemetry or vibration-window payload.
