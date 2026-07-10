# Engineering Log 98 — TP6 READY Rail-Level Proof After Reflow

Date: 2026-07-11  
Branch: gateway-dw3110-register-probe  
Repository: Thin-Pod-Gateway-rev-0.1

## Summary

This session followed up Engineering Log 97 by rechecking the DWM3001-CDK READY / IRQ path after reflowing the suspected J10.15 solder joint.

The focus was the physical route:

```text
DWM3001-CDK P0.28 / J10.15 / GPIO_RPI
→ Gateway TP6 / DWM_IRQ
→ NUCLEO CN15.16 / PB9
```

The result is a stronger physical proof than the earlier timing-only evidence. TP6 was observed against TP1 / GND with an approximately 0 V to 3 V waveform while the DWM3001-CDK READY stub was running. This closes the previous rail-level caveat as a bench-pass, with timing still taken from the RTT firmware log.

## Starting point

Engineering Log 97 recorded that the DWM3001-CDK host-interface stub could boot, configure the intended J10 pins, drive READY / P0.28, and read back the READY state correctly.

The open caveat was that TP6 physical observation was timing-correlated but not yet a clean rail-level 0 V to 3.3 V characterisation.

A possible dry or partially wetted solder joint was identified on the J10.15 / GPIO_RPI route. This joint was reflowed before the tests recorded here.

## Firmware state

The DWM3001-CDK host-interface stub remained the firmware source of truth for READY state.

The firmware identifies itself as:

```text
Thin-Pod DWM3001-CDK host-interface stub
stage=boot_identity_ready_toggle
board=decawave_dwm3001cdk
role=gateway_initiator backend=stub state=pre_spis
```

The pin map reported by RTT was:

```text
sck=P0.31
mosi=P0.27
miso=P0.07
cs=P0.30
ready=P0.28
reset=P0.18_external
```

The GPIO configuration completed successfully, including:

```text
HOST_READY port=gpio@50000000 pin=28 mode=output_input_inactive ret=0
```

The READY loop continued to show successful firmware readback:

```text
phase=ready_high set_ret=0 ready=1
phase=ready_low  set_ret=0 ready=0
```

RTT evidence file:

```text
logs/thinpod_gateway_tp6_ready_after_reflow_rtt_2026-07-11.log
```

## Continuity and isolation checks

Continuity checks were performed after reflow.

Confirmed continuity:

```text
CDK GND post ↔ CDK J1 -ve: PASS
CDK GND post ↔ Gateway TP1: PASS
CDK J1 -ve ↔ Gateway TP1: PASS
J10.15 / GPIO_RPI ↔ Gateway TP6: PASS
Gateway TP6 ↔ NUCLEO CN15.16 / PB9: PASS
```

Confirmed non-shorts:

```text
TP6 ↔ GND: no short detected
TP6 ↔ 3V3: no short detected
J10.15 ↔ neighbouring J10 pins: no short detected
```

These checks ruled out the main suspected open-circuit, ground-bond and adjacent-pin short faults.

## Measurement observations

Several exploratory scope/DMM observations were made while the DWM READY stub was running.

A 100 kΩ pulldown from TP6 to TP1 / GND caused the trace to flatline and was not adopted as a fix. This behaviour is recorded as an abnormal loading sensitivity observed during bench measurement.

With the DMM and scope connected between TP1 / GND and TP6, the signal resolved into a clear rail-level waveform.

Final scope evidence file:

```text
images/TP6_READY_rail_level_after_reflow_2026-07-11.jpeg
```

Final bench observation:

```text
Reference: TP1 / GND
Signal:    TP6 / DWM_IRQ / J10.15 / GPIO_RPI
Scale:     500 mV/div
Timebase:  2 s/div
Measured:  approximately 2.96–2.98 Vpp
Low:       near 0 V
High:      near 3 V
```

This confirms that the DWM READY / IRQ signal reaches the Gateway TP6 net at a valid logic-level amplitude under the observed bench conditions.

## Result summary

```text
J10.15 suspected solder issue reflowed: DONE
DWM READY firmware readback: PASS
J10.15 ↔ TP6 continuity: PASS
TP6 ↔ NUCLEO PB9 continuity: PASS
TP6 shorts to GND / 3V3: NOT DETECTED
Adjacent J10.15 shorts: NOT DETECTED
TP6 rail-level READY observation: PASS
Observed TP6 amplitude: approximately 0 V to 3 V
RTT / scope evidence pair: PASS
Direct NUCLEO-to-DW3110 DEV_ID probe: still not proven
```

## Engineering interpretation

The DWM3001-CDK READY / IRQ route should now be treated as physically proven at Gateway TP6 for bring-up purposes.

Engineering Log 97 established the DWM-side firmware proof and timing correlation. This log improves that evidence by showing TP6 reaching a valid logic-level amplitude after reflow and continuity checks.

The NUCLEO direct DW3110 register-read experiment remains a side diagnostic path. It continued to return all-zero DEV_ID reads during related testing and should not be treated as the Gateway architecture gate.

The working Gateway architecture remains:

```text
DWM3001-CDK / DWM3001C subsystem owns DW3110 radio control.
NUCLEO-N657X0-Q acts as Gateway supervisor, logger, analysis host and SPI controller.
Gateway PCB provides the modular open-hardware carrier/interface.
The DWM-to-NUCLEO host-interface contract is the next firmware milestone.
```

## Notes and caveats

The TP6 waveform showed sensitivity to measurement conditions during the investigation. Earlier unloaded scope captures were weak or distorted. The final retained capture shows a valid logic-level amplitude and should be used as the primary TP6 rail-level evidence.

RTT remains the timing source of truth for firmware READY high/low phases.

The latest RTT log showed `cs=0` during the READY proof run. This should be checked before SPIS transaction work, because CS idle/high behaviour matters for the host-interface contract.

## Next steps

1. Commit this log and the saved TP6 rail-level scope image.
2. Return `HEARTBEAT_MS` from the long measurement cadence to the normal development value before SPIS work.
3. Add a short note in the code or log history that the long READY cadence was a temporary physical-proof setting.
4. Verify CS idle behaviour on P0.30 / J10.24 / CS_RPI with the NUCLEO/Gateway assembly in the intended powered state.
5. Confirm PB9 is configured as a high-impedance input on the NUCLEO side before using TP6 / DWM_IRQ as a live interrupt/READY line.
6. Move to the first DWM-owned host-interface transaction proof.
7. Implement the smallest SPIS response first: `GET_CAPABILITIES`.
8. Build the matching NUCLEO SPI host probe for the DWM-to-NUCLEO protocol.
9. Capture and log the first valid command/response exchange:
   - NUCLEO asserts CS.
   - NUCLEO clocks request bytes over SPI5.
   - DWM3001-CDK receives the request through the confirmed J10 SPI pins.
   - DWM3001-CDK returns a valid protocol response.
   - NUCLEO validates status, sequence and CRC.

## Next milestone definition

The next meaningful firmware milestone is:

```text
DWM3001-CDK SPIS endpoint responds to a NUCLEO GET_CAPABILITIES request over the Gateway J10/SPI5 host-interface path.
```

That milestone should be treated as the next architecture-gating proof, rather than further direct NUCLEO-to-DW3110 register probing.
