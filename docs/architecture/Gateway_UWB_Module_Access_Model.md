# Gateway UWB Module Access Model

## Document control

**Document identifier:** TP-GW-ARCH-0003
**Document revision:** Draft A
**Product context:** Thin-Pod Gateway rev 0.1
**Document status:** Architecture decision and technical closeout record
**Certification boundary:** Supports Gateway rev 0.1 open-hardware packaging. This document does not expand the Thin-Pod rev 0.1 node certification boundary.

## 1. Purpose

This document records the Gateway rev 0.1 access-model decision for the Qorvo DWM3001-CDK.

The decision resolves whether the Gateway architecture should treat the DWM3001-CDK as:

```text
A. a passive DW3110 SPI target directly owned by the NUCLEO
B. an intelligent UWB coprocessor/module with its own onboard controller
```

The adopted approach for Gateway rev 0.1 is option B.

## 2. Decision

Thin-Pod Gateway rev 0.1 treats the DWM3001-CDK as an intelligent UWB coprocessor/module.

The DWM3001-CDK / DWM3001C subsystem owns DW3110 radio control. The STM32 NUCLEO-N657X0-Q acts as Gateway supervisor, host, logger and analysis controller. The Gateway carrier/interface board provides power, reset, GPIO, SPI/host-interface routing and test points between the modular subsystems.

Direct NUCLEO-to-DW3110 register probing is retained as diagnostic bring-up evidence. It is not the primary Gateway rev 0.1 architecture gate.

## 3. Architectural model

```text
Thin-Pod node
    |
    | UWB transport
    v
Gateway DWM3001-CDK UWB subsystem
    |
    | host-interface contract
    v
NUCLEO-N657X0-Q supervisor / memory / analysis layer
    |
    | network or auxiliary interface
    v
XIAO ESP32-C6 / later backhaul
```

In this model:

```text
DWM3001-CDK:
    UWB coprocessor/module
    DW3110 radio ownership
    UWB role firmware
    receive/reassembly/status/counter responsibilities as implemented

NUCLEO-N657X0-Q:
    Gateway supervisor
    host-interface controller
    admitted-window memory owner
    DSP / later TinyML host
    engineering evidence and logging endpoint

Gateway carrier PCB:
    modular open-hardware interconnect
    power and signal routing
    GPIO/SPI/test-point exposure
    replaceable-module integration layer
```

## 4. Rationale

The DWM3001-CDK is a module/controller development kit rather than a passive DW3110 breakout. The DWM3001C module includes an onboard nRF52833 controller and DW3110 UWB transceiver. Previous Gateway work already treated the DWM3001 side as a role-bearing UWB subsystem.

NUCLEO-side direct DW3110 probing has been useful diagnostic work:

```text
Build/sign/flash: pass
Firmware runtime: pass
SPI transaction ret=0: pass
DW3110 DEV_ID readback: not yet returned
RX result: all zero
```

The result suggests that direct external DW3110 ownership through the CDK is not currently established. It does not invalidate the Gateway carrier board or the modular Gateway architecture.

## 5. Consequences

Gateway rev 0.1 technical closeout should prioritise:

```text
1. Confirming the live state of the Gateway DWM3001-CDK.
2. Documenting the DWM3001-CDK as an intelligent UWB coprocessor/module.
3. Defining the DWM-to-NUCLEO host-interface contract.
4. Preserving direct DW3110 register probing as diagnostic evidence.
```

Gateway rev 0.1 open-hardware readiness should be judged against the carrier/interface board and its documented modular role, not against direct NUCLEO ownership of the DW3110 silicon.

## 6. Required follow-on work

The next firmware-enabling document should be:

```text
docs/dwm-to-nucleo-host-interface-protocol.md
```

Its first implementation proof should be deliberately small:

```text
GET_CAPABILITIES
GET_STATUS
GET_COUNTERS
```

The minimal proof should establish that the NUCLEO can communicate with Gateway-side DWM firmware through a defined host-interface path.

## 7. Technical closeout position

For Gateway rev 0.1, the technical closeout target is:

```text
The Gateway carrier/interface board cleanly integrates the NUCLEO supervisor,
the DWM3001-CDK UWB coprocessor/module and the XIAO ESP32-C6 auxiliary module,
with verified power, GPIO, SPI/test-point evidence and a documented UWB
module access model.
```

Direct NUCLEO-to-DW3110 register access remains a useful diagnostic branch and may be revisited if a future revision requires direct silicon ownership.
