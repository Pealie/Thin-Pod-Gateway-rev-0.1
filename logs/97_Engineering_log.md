# Engineering Log 97 — DWM3001-CDK Host-Interface Boot Stub and TP6 READY Timing Correlation

Date: 2026-07-09  
Branch: gateway-dw3110-register-probe  
Repository: Thin-Pod-Gateway-rev-0.1

## Summary

This session moved the Gateway rev 0.1 DWM3001-CDK host-interface work from documented pin feasibility into firmware, runtime proof and physical signal observation.

The DWM3001-CDK was treated as the Gateway UWB coprocessor/module. The NUCLEO remained the Gateway supervisor and future SPI host. The work focused on proving that the DWM-side firmware can own the J10 host-interface pins and drive the READY/IRQ path toward the Gateway PCB.

This log makes no direct NUCLEO-to-DW3110 register-control claim.

## Firmware work completed

A Zephyr application was brought up under:

```text
firmware/dwm3001cdk_host_interface_stub/
```

The application defines and uses the DWM3001-CDK J10 host-interface pin map:

```text
SCK candidate         = P0.31 / J10.23 / SPI1_CLK
MOSI candidate        = P0.27 / J10.19 / SPI1_MOSI
MISO candidate        = P0.07 / J10.21 / SPI1_MISO
CS candidate          = P0.30 / J10.24 / CS_RPI
READY / IRQ candidate = P0.28 / J10.15 / GPIO_RPI
RESET external path   = P0.18 / J10.12 / RESET
```

The firmware configures SCK, MOSI, MISO and CS as safe GPIO inputs. READY / P0.28 is configured as an output with input readback enabled.

## Build and flash proof

The Zephyr application built successfully for:

```text
decawave_dwm3001cdk/nrf52833
```

The DWM3001-CDK was flashed through J-Link using `west flash` and the Nordic `nrfutil` runner.

The Nordic tooling path was repaired by adding `C:\Tools\Nordic` to PATH and installing the `nrfutil device` command.

## RTT proof

RTT confirmed that the firmware booted as the Thin-Pod DWM3001-CDK host-interface stub and printed the intended pin map.

The READY loop was refined from a simple toggle/read to an explicit set/read sequence:

```text
set READY high
read READY
print phase=ready_high
sleep
set READY low
read READY
print phase=ready_low
sleep
```

The final RTT proof showed stable alternation:

```text
phase=ready_high set_ret=0 ready=1 cs=1
phase=ready_low  set_ret=0 ready=0 cs=1
```

This proves that the DWM3001-CDK firmware can drive and read back READY / P0.28, while CS / P0.30 remains visible as a high input.

RTT evidence file:

```text
logs/thinpod_gateway_stage8_rtt.log
```

## TP6 physical timing proof

The READY loop was slowed to make the physical signal easier to observe:

```text
HEARTBEAT_MS = 2000
```

This produced an intended READY cadence of approximately two seconds high and two seconds low.

Earlier TP6 scope captures showed faster, lower-amplitude activity that did not match the firmware cadence. Those captures are treated as exploratory only.

With the NUCLEO powered as part of the Gateway assembly, TP6 / DWM_IRQ showed slow physical activity on the handheld scope consistent with the slowed READY high/low firmware loop.

Scope evidence file:

```text
images/TP6_DWM_READY_2s_scope_2026-07-09.JPEG
```

The scope evidence supports physical activity and timing correlation on TP6. The displayed amplitude is approximately 2 Vpp rather than a clean 0 V to 3.3 V rail-level capture, so final voltage-level characterisation remains open.

## Result summary

```text
DWM3001-CDK host-interface boot stub: PASS
DWM-side J10 pin ownership: PASS
Zephyr overlay and GPIO alias resolution: PASS
READY / P0.28 set/read firmware proof: PASS
CS / P0.30 readback high: PASS
TP6 / DWM_IRQ physical activity with Gateway assembly powered: PASS
TP6 timing correlation with slowed READY loop: PASS
TP6 final rail-level 0 V to 3.3 V characterisation: PARTIAL
SPIS receive/respond behaviour: NOT YET STARTED
GET_CAPABILITIES transaction: NOT YET STARTED
```

## Engineering interpretation

The Gateway rev 0.1 DWM host-interface path is now materially stronger than a schematic-level claim. The DWM3001-CDK firmware boots, reports the intended J10 pin map, configures the host-interface pins, and drives/reads READY / P0.28.

With the Gateway assembly properly powered, TP6 / DWM_IRQ shows slow activity consistent with the firmware READY cadence. This supports proceeding toward the first SPI-peripheral host-interface proof.

The remaining electrical caution is voltage-level characterisation. The present evidence proves timing and physical activity, while final rail-level confirmation should be completed with a cleaner scope setup or multimeter check.

## Boundaries

This log makes no direct NUCLEO ownership claim over the DW3110 radio silicon.

This log does not yet claim a completed SPI peripheral transaction or a completed GET_CAPABILITIES response.

## Next steps

1. Optionally confirm TP6 voltage levels with a multimeter or cleaner scope setup while the 2000 ms loop runs.
2. Record `HEARTBEAT_MS = 2000` as a temporary physical-proof setting.
3. Return `HEARTBEAT_MS` to the desired development value before SPIS transaction work, unless the slower cadence remains useful for measurement.
4. Add SPIS peripheral configuration on the confirmed J10 SPI pins.
5. Implement only `GET_CAPABILITIES` first.
6. Build the matching NUCLEO-side SPI host probe.
7. Capture the first DWM-to-NUCLEO command/response proof.
