# 96 Engineering Log

## Thin-Pod Gateway rev 0.1 — DWM3001-CDK Live State and UWB Module Access Model

**Date:** 2026-07-09  
**Branch:** `gateway-dw3110-register-probe`  
**Repository path:** `C:\ThinPod\Thin-Pod-Gateway-rev-0.1`  
**Status:** Completed live-state and access-model closeout record

---

## 1. Purpose

This log records the technical closeout step following the initial and refined NUCLEO-side DW3110 register-read probe.

The purpose is to confirm the live state of the Gateway Qorvo DWM3001-CDK and to record the Gateway rev 0.1 access-model decision:

```text
Gateway rev 0.1 treats the DWM3001-CDK as an intelligent UWB coprocessor/module.
```

---

## 2. Starting point

The following evidence already exists:

```text
GPIO signal-path proof:              complete
SPI5 dummy-transfer firmware proof:  complete
SPI5 electrical proof:               complete
Initial DW3110 register probe:       implemented and logged
DW3110 register probe v2:            reset/framing refinement committed
```

The NUCLEO-side DW3110 register probe reached the following state:

```text
Build/sign/flash: pass
Firmware run:     pass
SPI ret=0:        pass
DEV_ID data:      pending / all-zero RX
```

The direct NUCLEO-to-DW3110 path remains diagnostic rather than architectural.

---

## 3. Gateway CDK live-state check

### Method

The Gateway DWM3001-CDK was connected through its SEGGER J-Link / RTT interface.

RTT logging was opened using SEGGER J-Link RTT Viewer.

```text
Tool:        SEGGER J-Link RTT Viewer V8.52
Log started: 2026-07-09 19:23:30
Target role: Gateway DWM3001-CDK
```

### Observed output

The RTT log showed the Gateway Stage-1 firmware running and emitting poll / packet records.

Representative output:

```text
*** Booting nRF Connect SDK v3.3.1-1d7a0b0e49b8 ***
*** Using Zephyr OS v4.3.99-37e6c28576ee ***

Thin-Pod Gateway UWB initiator stage-1
gateway_id=0x0000 role=initiator backend=stub
packet_log_format=TPGW_PKT key=value
TPGW_POLL seq=1 dst=0x0001
TPGW_PKT seq=1 src=0x0001 dst=0x0000 uptime_ms=252 valid=0 samples=32 rate=1600 first_mg=-41 min_mg=-92 max_mg=85 mean_mg=-4 rms_mg=46 checksum=0x1abff43f
```

The same log continued with repeated `TPGW_POLL` and `TPGW_PKT` records.

### Result

```text
[x] Previous Stage-1 Gateway initiator/stub firmware confirmed.
[ ] Different firmware observed.
[ ] No RTT output observed.
[ ] CDK state not confirmed.
```

---

## 4. Interpretation of CDK state

The Gateway DWM3001-CDK is not in a neutral pass-through state.

The observed live firmware state is:

```text
Thin-Pod Gateway UWB initiator stage-1
gateway_id=0x0000
role=initiator
backend=stub
packet_log_format=TPGW_PKT key=value
```

This confirms that the onboard nRF52833 should be treated as an active module controller in the current Gateway setup.

Direct external NUCLEO ownership of the DW3110 remains unproven. The all-zero NUCLEO-side DW3110 DEV_ID result should therefore be interpreted as diagnostic evidence about direct-access limitations rather than as evidence against the Gateway carrier board.

---

## 5. Architecture decision

Gateway rev 0.1 adopts the DWM3001-CDK-as-UWB-coprocessor model.

```text
DWM3001-CDK:
    intelligent UWB coprocessor/module
    owns DW3110 radio control
    provides UWB role behaviour through module firmware

NUCLEO-N657X0-Q:
    Gateway supervisor / host
    owns admitted-window memory and analysis path
    communicates with the DWM subsystem through a defined host-interface contract

Gateway PCB:
    open-hardware carrier/interface board
    provides modular power, GPIO, SPI/host-interface routing and test points
```

Direct NUCLEO-to-DW3110 register probing is retained as diagnostic bring-up evidence and is not the primary technical gate for Gateway rev 0.1 open-hardware readiness.

---

## 6. Relationship to previous DW3110 probe work

The NUCLEO-side DW3110 register probe remains useful because it demonstrated:

```text
dedicated register-probe app builds
signed NUCLEO image flashes
runtime probe loop operates
SPI transaction returns ret=0
multiple DEV_ID read framings were attempted
RX remained all zero
```

The refined v2 probe tested:

```text
RESET driven low then released high-Z
CS wake pulse
manual CS low across full header/read transaction
minimal DEV_ID framing
six-zero diagnostic framing
two-byte subaddress-style framing
three-byte extended-style framing
```

No variant returned a recognised `0xDECA0302` or `0xDECA03xx` response.

This result now sits under the module-access investigation boundary.

---

## 7. Scope evidence status

Oscilloscope work during the DW3110 probe session was not retained as formal evidence because the instrument required recharge and one channel ground lead was found to be disconnected.

Scope evidence should be repeated later only after measurement-chain sanity checks:

```text
CH1 tip to TP1, ground to TP1: flat 0 V
CH2 tip to TP1, ground to TP1: flat 0 V
CH1/CH2 to known 3V3: about 3.3 V
CH2 to TP7 / CS: clean logic-level CS pulse
```

This scope re-check is supporting evidence, not the primary architecture gate.

---

## 8. Consequence for open-hardware readiness

Gateway rev 0.1 technical closeout now focuses on the modular carrier/interface boundary.

The certifiable hardware object is:

```text
Thin-Pod Gateway rev 0.1 modular carrier/interface board for integrating
a NUCLEO-N657X0-Q supervisor, DWM3001-CDK UWB coprocessor/module and
XIAO ESP32-C6 auxiliary network module.
```

Remaining technical work before packaging:

```text
1. Commit this CDK live-state record.
2. Commit the UWB module access-model architecture note.
3. Draft the DWM-to-NUCLEO host-interface protocol.
4. Perform optional corrected scope sanity capture once the oscilloscope is recharged.
```

---

## 9. Repository updates

Add / commit:

```text
docs/architecture/Gateway_UWB_Module_Access_Model.md
logs/96_Engineering_Log.md
logs/Gateway_CDK_Live_State_RTT_2026-07-09.log
```

Suggested commits:

```text
Add Gateway UWB module access model
Add engineering log 96 for DWM3001-CDK state and access model
```

---

## 10. Summary

This log closes the direct-DW3110 diagnostic branch as useful but not architecture-defining.

The Gateway rev 0.1 architecture now follows the modular model:

```text
DWM3001-CDK owns UWB/radio control.
NUCLEO supervises, stores, analyses and coordinates.
Gateway PCB provides the open-hardware modular carrier/interface layer.
```

The live Gateway CDK state confirms that the DWM3001-CDK is running the Stage-1 Gateway initiator/stub firmware and should be treated as an active UWB module/controller in the rev 0.1 Gateway architecture.
