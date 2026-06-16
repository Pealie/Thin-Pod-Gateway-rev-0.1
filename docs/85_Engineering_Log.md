# 85 Engineering Log

## Thin-Pod Gateway / Node Firmware Bring-Up: Stage-1 UWB Role Harness

**Date:** 16 June 2026  
**Project:** Thin-Pod Gateway rev 0.1 / Thin-Pod node firmware path  
**Repository:** `C:\thinpod\Thin-Pod-Gateway-rev-0.1`  
**Toolchain:** nRF Connect SDK v3.3.1, Zephyr, west v1.5.0, Zephyr SDK 0.17.0, J-Link 9.50  
**Boards:** Qorvo / Decawave DWM3001-CDK, nRF52833 target  
**Stage:** Build, flash and runtime evidence for Stage-1 Gateway initiator and node responder firmware

---

## 1. Summary

Tonight’s work established a clean Stage-1 firmware milestone for the Thin-Pod Gateway and node platform.

Both firmware roles were built successfully under nRF Connect SDK v3.3.1:

- `firmware/node/uwb_responder`
- `firmware/gateway/uwb_initiator`

Both images were flashed successfully to their intended DWM3001-CDKs using the J-Link runner. SEGGER RTT Viewer then confirmed that both boards booted and emitted role-specific structured logs.

The Gateway CDK emitted `TPGW_POLL` and `TPGW_PKT` records. The node CDK emitted `TPNODE_RESPONDER_READY` records. These logs confirm that the Stage-1 firmware harness is buildable, flashable and observable, with stable node/Gateway identity conventions and a parser-friendly packet/log format.

This remains a **stub-backend milestone**, not yet proof of live DW3110 RF exchange. Nevertheless, it is an important step because the project now has working firmware roles on the two DWM3001-CDKs, rather than merely repository scaffolding.

The most important conclusion is:

```text
Thin-Pod Gateway and node Stage-1 firmware roles now build, flash and run on
their intended DWM3001-CDKs, producing observable RTT logs with the expected
Gateway initiator and node responder packet conventions.
```

---

## 2. Hardware / firmware role mapping

The intended board mapping was preserved throughout the work:

```text
Gateway CDK 760203854 -> gateway_uwb_initiator_stage1
Node CDK    760222856 -> node_uwb_responder_stage1
```

The Gateway role uses ID:

```text
gateway_id=0x0000
role=initiator
```

The node role uses ID:

```text
node_id=0x0001
role=responder
```

The intended packet direction is therefore:

```text
Node    0x0001 -> Gateway 0x0000
Gateway 0x0000 -> Node    0x0001
```

At this stage, the firmware uses a stub backend. The output simulates or stages the data path and packet contract rather than proving real UWB RF transport.

---

## 3. Problem: nRF Connect SDK terminal would not start

The original obstacle was that the nRF Connect SDK v3.3.1 terminal could not be started. Ordinary PowerShell was therefore used instead.

Initial checks showed that `west` and `ninja` were not visible in the standard PowerShell environment:

```powershell
west --version
cmake --version
ninja --version
```

The visible results were:

```text
west : The term 'west' is not recognized...
cmake version 3.25.0
ninja : The term 'ninja' is not recognized...
```

A search for the Nordic toolchain found `west.exe` here:

```powershell
Get-ChildItem C:\ncs\toolchains -Recurse -Filter west.exe -ErrorAction SilentlyContinue |
  Select-Object FullName
```

Result:

```text
C:\ncs\toolchains\936afb6332\opt\bin\Scripts\west.exe
```

The toolchain paths were then added manually for the current PowerShell session.

---

## 4. Manual PowerShell environment setup

The following commands made `west` and `ninja` visible:

```powershell
$tc = "C:\ncs\toolchains\936afb6332\opt\bin"
$env:Path = "$tc;$tc\Scripts;$env:Path"

west --version
ninja --version
```

Confirmed versions:

```text
West version: v1.5.0
ninja 1.13.2
```

The Zephyr SDK variables were then set:

```powershell
$env:ZEPHYR_TOOLCHAIN_VARIANT = "zephyr"
$env:ZEPHYR_SDK_INSTALL_DIR = "C:\ncs\toolchains\936afb6332\opt\zephyr-sdk"
```

This successfully reproduced the essential Nordic build environment without needing the SDK terminal shortcut.

---

## 5. Build: Thin-Pod node UWB responder Stage 1

The node responder firmware was built with:

```powershell
cd C:\ncs\v3.3.1

west build -b decawave_dwm3001cdk -p always `
  -d C:\ncs\v3.3.1\build_node_uwb_responder_stage1 `
  C:\thinpod\Thin-Pod-Gateway-rev-0.1\firmware\node\uwb_responder
```

The build identified the board as:

```text
Board: decawave_dwm3001cdk, qualifiers: nrf52833
```

The application was correctly identified as:

```text
Application: C:/ThinPod/Thin-Pod-Gateway-rev-0.1/firmware/node/uwb_responder
```

The build completed successfully:

```text
[164/164] Linking C executable zephyr\zephyr.elf

Memory region         Used Size  Region Size  %age Used
           FLASH:       21104 B       512 KB      4.03%
             RAM:        5696 B       128 KB      4.35%

[10/10] Generating ../merged.hex
```

The output image is:

```text
C:\ncs\v3.3.1\build_node_uwb_responder_stage1\merged.hex
```

The partition-manager warning appeared during configuration, but this was only a deprecation warning and did not prevent the build.

---

## 6. Build: Thin-Pod Gateway UWB initiator Stage 1

The Gateway initiator firmware was built with:

```powershell
west build -b decawave_dwm3001cdk -p always `
  -d C:\ncs\v3.3.1\build_gateway_uwb_initiator_stage1 `
  C:\thinpod\Thin-Pod-Gateway-rev-0.1\firmware\gateway\uwb_initiator
```

The build again identified the board as:

```text
Board: decawave_dwm3001cdk, qualifiers: nrf52833
```

The application was correctly identified as:

```text
Application: C:/ThinPod/Thin-Pod-Gateway-rev-0.1/firmware/gateway/uwb_initiator
```

The build completed successfully:

```text
[164/164] Linking C executable zephyr\zephyr.elf

Memory region         Used Size  Region Size  %age Used
           FLASH:       21508 B       512 KB      4.10%
             RAM:        5696 B       128 KB      4.35%

[10/10] Generating ../merged.hex
```

The output image is:

```text
C:\ncs\v3.3.1\build_gateway_uwb_initiator_stage1\merged.hex
```

---

## 7. Flashing issue: nrfutil path and missing plugin worker

The initial `west flash` attempt failed because `nrfutil` was not visible in the PowerShell path:

```powershell
west flash -d C:\ncs\v3.3.1\build_node_uwb_responder_stage1 --dev-id 760222856
```

The first error was:

```text
FATAL ERROR: required program nrfutil not found; install it or add its location to PATH
```

A search found `nrfutil.exe` in the Nordic toolchain:

```powershell
Get-ChildItem C:\ncs -Recurse -Filter nrfutil.exe -ErrorAction SilentlyContinue |
  Select-Object FullName
```

Relevant locations:

```text
C:\ncs\toolchains\936afb6332\nrfutil\bin\nrfutil.exe
C:\ncs\toolchains\936afb6332\nrfutil\home\bin\nrfutil.exe
```

The paths were added:

```powershell
$env:Path = "C:\ncs\toolchains\936afb6332\nrfutil\bin;C:\ncs\toolchains\936afb6332\nrfutil\home\bin;$env:Path"
```

`nrfutil` was then visible:

```powershell
nrfutil --version
where.exe nrfutil
```

Confirmed:

```text
nrfutil 8.2.0
C:\ncs\toolchains\936afb6332\nrfutil\bin\nrfutil.exe
C:\ncs\toolchains\936afb6332\nrfutil\home\bin\nrfutil.exe
```

`nrfutil device list` saw the Gateway probe:

```powershell
nrfutil device list
```

Output:

```text
760203854
Product         J-Link
Ports           COM3
Traits          jlink, seggerUsb, serialPorts, usb

Supported devices found: 1
```

However, `west flash` with the default `nrfutil` runner then failed due to a missing worker executable:

```text
Unable to find worker executable: plugin-probe-worker.exe
```

This was diagnosed as an `nrfutil` plugin/worker problem, not a firmware or build problem.

---

## 8. Successful flashing using J-Link runner

The `nrfutil` route was bypassed by explicitly selecting the J-Link runner.

Gateway initiator flash command:

```powershell
west flash -d C:\ncs\v3.3.1\build_gateway_uwb_initiator_stage1 --runner jlink --dev-id 760203854
```

Successful output included:

```text
-- west flash: using runner jlink
-- runners.jlink: JLink version: 9.50
-- runners.jlink: Flashing file: C:\ncs\v3.3.1\build_gateway_uwb_initiator_stage1\merged.hex
```

Node responder flash command:

```powershell
west flash -d C:\ncs\v3.3.1\build_node_uwb_responder_stage1 --runner jlink --dev-id 760222856
```

Successful output included:

```text
-- west flash: using runner jlink
-- runners.jlink: JLink version: 9.50
-- runners.jlink: Flashing file: C:\ncs\v3.3.1\build_node_uwb_responder_stage1\merged.hex
```

This establishes the practical flashing route for the current setup:

```text
Use --runner jlink rather than the default nrfutil runner.
```

---

## 9. Runtime verification: Gateway RTT output

SEGGER RTT Viewer confirmed that the Gateway CDK was running the Stage-1 Gateway initiator firmware.

Observed Gateway banner:

```text
Thin-Pod Gateway UWB initiator stage-1
gateway_id=0x0000 role=initiator backend=stub
packet_log_format=TPGW_PKT key=value
```

The Gateway emitted repeated poll and packet records:

```text
TPGW_POLL seq=10 dst=0x0001
TPGW_PKT seq=10 src=0x0001 dst=0x0000 uptime_ms=9265 valid=0 samples=32 rate=1600 first_mg=22 min_mg=-81 max_mg=84 mean_mg=1 rms_mg=42 checksum=0x47ad238e
```

The captured Gateway terminal log showed sequential records from `seq=10` through at least `seq=35`, with:

```text
src=0x0001
dst=0x0000
samples=32
rate=1600
first_mg=<changing value>
min_mg=<changing value>
max_mg=<changing value>
mean_mg=<changing value>
rms_mg=<changing value>
checksum=<changing value>
```

The `valid=0` field is acceptable at this stage because the firmware is using a stub backend and is not yet claiming a validated real transport packet.

Gateway evidence file:

```text
thinpod_gateway_stage1_rtt.log
```

---

## 10. Runtime verification: Node RTT output

SEGGER RTT Viewer also confirmed that the node CDK was running the Stage-1 node responder firmware.

Observed node banner on screen:

```text
Thin-Pod Node UWB responder stage-1
node_id=0x0001 role=responder backend=stub
status=ready waiting_for_gateway_poll
```

A useful terminal log was captured after correcting the RTT Viewer logging mode.

The node emitted repeated responder-ready records:

```text
TPNODE_RESPONDER_READY seq=154 src=0x0001 dst=0x0000 samples=32 rate=1600 checksum=0x3dde2674 first_mg=50
TPNODE_RESPONDER_READY seq=155 src=0x0001 dst=0x0000 samples=32 rate=1600 checksum=0x0266b931 first_mg=57
```

The captured log showed sequential records from `seq=154` through at least `seq=186`, with:

```text
src=0x0001
dst=0x0000
samples=32
rate=1600
checksum=<changing value>
first_mg=<changing value>
```

Node evidence file:

```text
thinpod_node_stage1_rtt.log
```

---

## 11. RTT Viewer logging note

RTT Viewer initially produced a file containing only logging start/stop metadata. The useful Gateway and node logs were captured only after using the correct terminal logging path.

Lessons learned:

```text
Use Terminal Log, not Data Log.
Select Terminal 0 where possible.
Start logging before the useful output is emitted.
Reset the board after logging starts if the boot banner is required.
```

The node’s first attempted file was a Data Log and contained no useful terminal records. The second attempt produced the proper terminal log.

---

## 12. Engineering significance

This milestone is significant for several reasons.

First, it confirms that the repository firmware is not merely illustrative. The Stage-1 Gateway and node applications build successfully under the actual NCS v3.3.1 environment.

Second, it confirms that the DWM3001-CDKs can be flashed through the reproducible `west flash --runner jlink` route.

Third, it confirms that the firmware roles are separated and identifiable:

```text
Gateway: initiator, ID 0x0000
Node:    responder, ID 0x0001
```

Fourth, it confirms that the shared packet/logging convention is visible at runtime:

```text
TPGW_POLL
TPGW_PKT
TPNODE_RESPONDER_READY
```

Fifth, it supports the larger Thin-Pod platform thesis: the hardware and reference firmware are moving toward a documented vibration-window interface for downstream DSP and TinyML work.

This is not yet real UWB RF transport, but it is a valuable integration step because it proves that both sides of the intended architecture now have buildable, flashable, observable firmware roles.

---

## 13. OSHWA / documentation relevance

This stage is useful for the eventual Thin-Pod Gateway rev 0.1 OSHWA-adjacent documentation because it demonstrates reference firmware and software as bring-up and verification material.

The correct boundary remains:

```text
This Stage-1 firmware verifies build, flash, role identity, packet convention and RTT logging.
It does not yet certify or prove live DW3110 RF exchange, industrial predictive-maintenance performance, DSP validity or TinyML diagnostic accuracy.
```

A suitable documentation statement is:

```text
Stage-1 UWB-role firmware has been built and flashed to both DWM3001-CDKs. The
Gateway CDK runs the initiator harness and emits parser-friendly TPGW_POLL /
TPGW_PKT RTT records. The node CDK runs the responder harness and emits
TPNODE_RESPONDER_READY RTT records with node ID, destination ID, sequence number,
sample count, sample rate, checksum and representative acceleration sample
fields.

This remains a stub-backend firmware milestone rather than proof of live DW3110
RF exchange, but it verifies the Zephyr build path, J-Link flashing path, role
separation, packet identity convention and RTT logging format for the next
transport-integration stage.
```

---

## 14. Commands retained for future use

### Manual PowerShell setup

```powershell
$tc = "C:\ncs\toolchains\936afb6332\opt\bin"
$env:Path = "$tc;$tc\Scripts;$env:Path"

$env:ZEPHYR_TOOLCHAIN_VARIANT = "zephyr"
$env:ZEPHYR_SDK_INSTALL_DIR = "C:\ncs\toolchains\936afb6332\opt\zephyr-sdk"
```

### Optional nrfutil path setup

```powershell
$env:Path = "C:\ncs\toolchains\936afb6332\nrfutil\bin;C:\ncs\toolchains\936afb6332\nrfutil\home\bin;$env:Path"
```

### Build node responder

```powershell
cd C:\ncs\v3.3.1

west build -b decawave_dwm3001cdk -p always `
  -d C:\ncs\v3.3.1\build_node_uwb_responder_stage1 `
  C:\thinpod\Thin-Pod-Gateway-rev-0.1\firmware\node\uwb_responder
```

### Build Gateway initiator

```powershell
west build -b decawave_dwm3001cdk -p always `
  -d C:\ncs\v3.3.1\build_gateway_uwb_initiator_stage1 `
  C:\thinpod\Thin-Pod-Gateway-rev-0.1\firmware\gateway\uwb_initiator
```

### Flash Gateway initiator with J-Link runner

```powershell
west flash -d C:\ncs\v3.3.1\build_gateway_uwb_initiator_stage1 --runner jlink --dev-id 760203854
```

### Flash node responder with J-Link runner

```powershell
west flash -d C:\ncs\v3.3.1\build_node_uwb_responder_stage1 --runner jlink --dev-id 760222856
```

### Confirm visible probes

```powershell
nrfutil device list
```

---

## 15. Next steps

The next firmware stage should move from stub backend toward real UWB transport integration.

Suggested next stages:

```text
1. Preserve the Stage-1 logs in the repository under docs/firmware/evidence/ or logs/.
2. Add a short README explaining the Stage-1 firmware evidence.
3. Add parser validation against the captured Gateway RTT log.
4. Begin Stage-2 transport work: replace stub backend with DW3110 / DWM3001 UWB exchange.
5. Preserve the same packet contract where possible.
6. Keep DSP / TinyML above the vibration-window interface boundary.
```

A sensible immediate repository addition would be:

```text
docs/firmware/evidence/gateway_stage1_rtt_log.txt
docs/firmware/evidence/node_stage1_rtt_log.txt
docs/firmware/Stage1_UWB_Role_Bringup_Notes.md
```

---

## 16. Closing note

This was a strong evening’s work.

The workflow overcame a missing SDK terminal, absent PowerShell path entries, invisible `west`, invisible `ninja`, a missing `nrfutil` path, an `nrfutil` plugin failure, and RTT logging quirks. Despite those obstacles, both firmware roles were built, flashed and observed.

The result is a defensible Stage-1 firmware milestone:

```text
Gateway and node Stage-1 UWB role harnesses are now buildable, flashable and
observable on their intended DWM3001-CDKs.
```

That moves Thin-Pod Gateway rev 0.1 materially closer to being a usable open hardware platform for vibration-window acquisition, transport staging and downstream DSP / TinyML development.
