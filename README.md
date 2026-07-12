# Thin-Pod Gateway rev 0.1

**Open-hardware Gateway carrier PCB for receiving, supervising and forwarding Thin-Pod vibration-window data**

**Development status:** PCB assembled and initial bring-up completed. Gateway and node DWM3001CDK Zephyr / J-Link / SEGGER RTT baselines have been verified. Stage-1 UWB responder / initiator firmware harness, Gateway packet logging and vibration-window ingestion scaffolding have been added. The NUCLEO-to-DWM TPHIP host-interface transfer has been verified across the manufactured carrier. Real DW3110 RF exchange, ADXL1005 vibration-window transport and DSP / TinyML remain verification milestones rather than completed rev 0.1 claims.
**Certification status:** Candidate for a later, separate OSHWA self-certification application. No Gateway OSHWA application has yet been submitted.
**Related certified-item boundary:** `Thin-Pod rev 0.1` covers the OSHWA-certified sensor-node carrier PCB only, UID `UK000091`. This Gateway is a separate hardware artefact and does not alter the certified node boundary.

## Overview

Thin-Pod Gateway rev 0.1 is a carrier-board prototype intended to connect three commercially supplied development modules:

* an **STM32 NUCLEO-N657X0-Q** as the main host and prospective analysis supervisor;
* a **Qorvo DWM3001-CDK** as the Gateway-side UWB development module; and
* a **Seeed Studio XIAO ESP32-C6** as an optional onward-networking subsystem.

The design provides the physical interconnect, verified host-interface route, separate control signals, local decoupling, power distribution and test access required to evaluate a future vibration-window data path from a Thin-Pod node to Gateway-host memory.

Thin-Pod Gateway rev 0.1 is a carrier-board prototype for a modular vibration-telemetry architecture. The Qorvo DWM3001-CDK is intended to operate as a UWB communications subsystem, receiving and validating framed vibration windows before presenting complete records to the STM32 NUCLEO-N657X0-Q through a firmware-defined host interface. The NUCLEO is intended to act as the analytic supervisor for buffering, DSP and later TinyML evaluation. The XIAO ESP32-C6 is optional onward-networking hardware and is outside the measurement-critical path.

Initial board bring-up has now established a working Gateway / node firmware-development baseline using nRF Connect SDK / Zephyr, onboard J-Link OB debug access and SEGGER RTT logging. The repository includes minimal alive-test firmware for both the Gateway and node DWM3001CDKs, plus a Stage-1 UWB responder / initiator harness using a stub transport and shared Thin-Pod packet protocol. The NUCLEO-to-DWM TPHIP GET_CAPABILITIES exchange is now verified; real RF exchange and full vibration-window transfer remain the next verification stages.

## Gateway hardware evidence

The Thin-Pod Gateway rev 0.1 repository includes photographic evidence of the assembled Gateway PCB and the Gateway in context with the Thin-Pod sensor node. These images provide a visual reference for the current hardware state, including the bench-built Gateway carrier PCB, module placement and the node-to-Gateway hardware relationship.

<img src="images/Thin-Pod%20Gateway%20PCB.JPG" alt="Thin-Pod Gateway PCB" width="700">

*Thin-Pod Gateway rev 0.1 PCB assembly.*

<img src="images/Thin-Pod%20Gateway%20and%20Thin-Pod%20node.JPG" alt="Thin-Pod Gateway and Thin-Pod node" width="700">

*Thin-Pod Gateway shown with the Thin-Pod node hardware.*

## Measurement-integrity principle

The Gateway architecture is intended to preserve raw vibration windows and their associated metadata before derived features or model outputs are generated. This keeps later DSP and TinyML results traceable to the acquired measurement, node identity, sampling configuration, transport-integrity state and processing version.

The design principle is therefore:

```
raw vibration windows from the Pod;
verified window transfer through the UWB subsystem;
features and later models on the Gateway.
```

### Revision identity

The product release path is:

```
Thin-Pod Gateway rev 0.1
```

The PCB already ordered for manufacture may carry the existing board/silkscreen identifier:

```
rev 0.1f
```

For documentation purposes, `rev 0.1f` is treated as the ordered **pre-release fabrication build** within the Gateway rev 0.1 development cycle. It is not `rev 0.3`.

A later `rev 0.3` is reserved for an SMT/chip-down transition, potentially replacing development-board interfaces with more integrated hardware such as a castellated DWM3001C module. That work is explicitly deferred until the rev 0.1 Gateway carrier board and first communications path have been verified.

## Intended Gateway architecture

```
Thin-Pod rev 0.1 sensor-node carrier PCB
        ↓  future UWB vibration-window transport
Qorvo DWM3001-CDK on Thin-Pod Gateway rev 0.1
        ↓  verified TPHIP host-interface route
STM32 NUCLEO-N657X0-Q
        ↓
buffer capture / logging / future DSP and TinyML
        ↓  optional onward interface
Seeed Studio XIAO ESP32-C6
```

The architecture is deliberately modular. The DWM3001-CDK is treated as a commercially supplied UWB subsystem; the NUCLEO is treated as the Gateway-side host/supervisor; and the XIAO is treated as optional onward networking. The Gateway PCB does not claim those commercial modules as open hardware.

## Firmware bring-up status

The repository now includes two firmware bring-up layers.

The first layer is the Zephyr RTT alive-test baseline:

```
firmware/gateway/zephyr_rtt_alive/
firmware/node/zephyr_rtt_alive/
```

These applications prove that the Gateway and node DWM3001CDKs can be built for, flashed, executed and observed over SEGGER RTT.

The second layer is the staged UWB role harness:

```
firmware/common/thinpod_protocol/
firmware/gateway/uwb_initiator/
firmware/node/uwb_responder/
tools/packet_logging/
```

This stage establishes Gateway and node firmware identities, a shared packet contract, parser-friendly Gateway packet logging and synthetic vibration-window ingestion. It does not yet claim a real DW3110 RF exchange. The stub transport is intended to be replaced by the Qorvo DW3 / DW3110 backend in a later verification stage.

## Current rev 0.1 carrier-board content

|Reference|Commercial module or circuitry|Intended function|
|-|-|-|
|`U1`|STM32 `NUCLEO-N657X0-Q` mounted through ST morpho-header interfaces|Main host processor and later analysis supervisor|
|`U2`|Qorvo `DWM3001-CDK`|Gateway-side UWB development module|
|`U3`|Seeed Studio `XIAO ESP32-C6`|Optional onward-networking module|
|`R1`, `R2`|10 kΩ pull-up resistors|Idle-state control for chip-select lines|
|`C1`, `C3`|100 nF decoupling capacitors|Local high-frequency supply decoupling|
|`C2`, `C4`|10 µF capacitors|Local bulk decoupling|
|`C5`|22 µF capacitor|Additional local bulk decoupling|
|`TP1`–`TP13`|Test-point provision|Power and signal bring-up observability|
|`H1`–`H4`|M3 mounting holes|Mechanical support|

## Electrical interface summary

The current PCB uses the NUCLEO `CN3` and `CN15` interfaces only. `CN3` provides the principal power and ground paths; `CN15` provides the SPI5 and control-signal route.

### Power roles

|Net / connection|Intended role|
|-|-|
|`5V_GATEWAY` from NUCLEO `CN3.6`|Powers DWM3001-CDK through its 5 V input path and supplies XIAO `5V/VBUS`|
|`3V3_GATEWAY` from NUCLEO `CN3.16`|Auxiliary/pull-up rail; not the XIAO power input|
|`GND`|Common reference across NUCLEO, DWM3001-CDK, XIAO and local decoupling|

### NUCLEO to DWM3001-CDK signal route

|Gateway net|NUCLEO connection|DWM3001-CDK connection|
|-|-|-|
|`SPI5_SCK`|`CN15.11` / `PE15`|`J10.23` / `SPI1_CLK`|
|`SPI5_MISO`|`CN15.13` / `PG1`|`J10.21` / `SPI1_MISO`|
|`SPI5_MOSI`|`CN15.15`|`J10.19` / `SPI1_MOSI`|
|`DWM_CS`|`CN15.17`|`J10.24` / `CS_RPI`|
|`DWM_IRQ`|`CN15.16`|`J10.15`|
|`DWM_RESET`|`CN15.33`|`J10.12`|

### NUCLEO to XIAO ESP32-C6 signal route

|Gateway net|NUCLEO connection|XIAO connection|
|-|-|-|
|`SPI5_SCK`|`CN15.11`|`D8` / `GPIO19` / `SCK`|
|`SPI5_MISO`|`CN15.13`|`D9` / `GPIO20` / `MISO`|
|`SPI5_MOSI`|`CN15.15`|`D10` / `GPIO18` / `MOSI`|
|`C6_CS`|`CN15.19`|`D3` / `GPIO21`|
|`C6_INT`|`CN15.5`|`D2` / `GPIO2`|
|`5V_GATEWAY`|`CN3.6`|`5V/VBUS`|
|`GND`|Common ground|`GND`|

The route above establishes a physical interface candidate. Firmware-level exchange over that route is a verification objective, not a completed claim.

## Certification scope position

A future OSHWA application for **Thin-Pod Gateway rev 0.1** would cover the creator-designed Gateway carrier PCB, its editable design source, fabrication outputs, BOM, documentation and any project-authored footprint/interface material.

It would not claim the STM32 NUCLEO-N657X0-Q, Qorvo DWM3001-CDK or XIAO ESP32-C6 development modules themselves as open hardware. It would not imply that the already submitted `Thin-Pod rev 0.1` sensor-node certification application includes the Gateway.

## Repository structure

```
Thin-Pod-Gateway-rev-0.1/
├── README.md
├── LICENSE-HARDWARE.md
├── LICENSE-DOCUMENTATION.md
│
├── firmware/
│   ├── README.md
│   │
│   ├── common/
│   │   └── thinpod_protocol/
│   │       ├── README.md
│   │       ├── thinpod_protocol.c
│   │       └── thinpod_protocol.h
│   │
│   ├── gateway/
│   │   ├── zephyr_rtt_alive/
│   │   │   ├── CMakeLists.txt
│   │   │   ├── README.md
│   │   │   ├── prj.conf
│   │   │   └── src/
│   │   │       └── main.c
│   │   │
│   │   └── uwb_initiator/
│   │       ├── CMakeLists.txt
│   │       ├── README.md
│   │       ├── prj.conf
│   │       └── src/
│   │           └── main.c
│   │
│   ├── node/
│   │   ├── zephyr_rtt_alive/
│   │   │   ├── CMakeLists.txt
│   │   │   ├── README.md
│   │   │   ├── prj.conf
│   │   │   └── src/
│   │   │       └── main.c
│   │   │
│   │   └── uwb_responder/
│   │       ├── CMakeLists.txt
│   │       ├── README.md
│   │       ├── prj.conf
│   │       └── src/
│   │           └── main.c
│   │
│   ├── node-dwm/
│   │   ├── README.md
│   │   └── acquisition-characterisation/
│   │       └── README.md
│   │
│   ├── gateway-dwm/
│   │   ├── README.md
│   │   └── tphip-golden-record/
│   │       └── README.md
│   │
│   ├── nucleo-host/
│   │   ├── README.md
│   │   └── tphip-reader/
│   │       └── README.md
│   │
│   └── test-vectors/
│       └── README.md
│
├── hardware/
│   ├── source/
│   │   ├── Thin-Pod_Gateway_rev0.1.kicad_pro
│   │   ├── Thin-Pod_Gateway_rev0.1.kicad_sch
│   │   ├── Thin-Pod_Gateway_rev0.1.kicad_pcb
│   │   ├── fp-lib-table
│   │   ├── sym-lib-table
│   │   └── footprints/
│   │
│   ├── fabrication/
│   │   ├── gerbers/
│   │   ├── drills/
│   │   ├── Thin-Pod_Gateway_rev0.1_fabrication_outputs.zip
│   │   └── RELEASE-MANIFEST.md
│   │
│   └── bom/
│       ├── Thin-Pod_Gateway_rev0.1_BOM.md
│       └── Thin-Pod_Gateway_rev0.1_BOM.csv
│
├── docs/
│   ├── certification-scope.md
│   ├── gateway-bring-up-and-verification-protocol.md
│   ├── system-interface-control-document.md
│   ├── third-party-components.md
│   ├── footprint-provenance.md
│   ├── design-verification.md
│   ├── future-revisions.md
│   │
│   └── firmware/
│       ├── RTT_Alive_Test_Baseline.md
│       └── UWB_Roles_Stage1.md
│
├── tools/
│   └── packet_logging/
│       ├── gateway_rtt_packet_parser.py
│       └── sample_gateway_rtt.log
│
├── images/
│   ├── fabrication/
│   ├── bring-up/
│   └── verification/
│
└── oshwa/
    └── application-record.md
```

## Documentation prepared during manufacture, bring-up and staged firmware work

|Document|Purpose|
|-|-|
|[`docs/certification-scope.md`](docs/certification-scope.md)|Establishes the separate Gateway rev 0.1 OSHWA boundary|
|[`docs/gateway-bring-up-and-verification-protocol.md`](docs/gateway-bring-up-and-verification-protocol.md)|Defines the physical and firmware test sequence for board arrival and staged bring-up|
|[`docs/system-interface-control-document.md`](docs/system-interface-control-document.md)|Defines the intended boundary between Thin-Pod rev 0.1 and Gateway rev 0.1|
|[`docs/firmware/RTT_Alive_Test_Baseline.md`](docs/firmware/RTT_Alive_Test_Baseline.md)|Documents the verified Gateway and node Zephyr / J-Link / SEGGER RTT alive-test baseline|
|[`docs/firmware/UWB_Roles_Stage1.md`](docs/firmware/UWB_Roles_Stage1.md)|Documents the staged UWB responder / initiator harness, shared packet contract, packet logging and vibration-window ingestion boundary|

## Release and OSHWA gate

No Gateway OSHWA application should be submitted before:

1. the completed bring-up evidence has been consolidated into the repository;
2. unpowered continuity, resistance checks and staged power validation are documented;
3. the Gateway carrier board release definition is reconciled against the manufactured PCB;
4. source, Gerbers, BOM and footprint provenance are reconciled;
5. any bring-up corrections or known limitations are explicitly recorded;
6. the firmware boundary is described accurately, distinguishing verified Zephyr / RTT bring-up and staged UWB scaffolding from unverified real RF exchange;
7. any pre-release correction discovered in testing has been incorporated into the release definition; and
8. a clean Gateway rev 0.1 release tag has been created.

The most defensible Gateway certification position is therefore: designed and documented as an open-hardware candidate, physically brought up, firmware-baselined, and only then considered for certification once the release boundary is clean and evidence-backed.

## References

* STMicroelectronics, `NUCLEO-N657X0-Q` product page and board documentation: [https://www.st.com/en/evaluation-tools/nucleo-n657x0-q.html](https://www.st.com/en/evaluation-tools/nucleo-n657x0-q.html)
* Qorvo, `DWM3001CDK` product page and product brief: [https://www.qorvo.com/products/p/DWM3001CDK](https://www.qorvo.com/products/p/DWM3001CDK)
* Seeed Studio, `XIAO ESP32C6` documentation and pin map: [https://wiki.seeedstudio.com/xiao_esp32c6_getting_started/](https://wiki.seeedstudio.com/xiao_esp32c6_getting_started/)
* OSHWA Certification Requirements: [https://certification.oshwa.org/requirements.html](https://certification.oshwa.org/requirements.html)
* OSHWA Documentation Guidance: [https://certification.oshwa.org/process/documentation.html](https://certification.oshwa.org/process/documentation.html)

