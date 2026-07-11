# 92 Engineering Log

## Thin-Pod Gateway rev 0.1: GPIO Signal-Path Confirmation at DWM3001-CDK Header

**Date:** 7 July 2026<br>
**Repository:** `Thin-Pod-Gateway-rev-0.1`<br>
**Branch:** `gateway-nucleo-spi5-gpio-probe`<br>
**Scope:** Gateway PCB signal-path confirmation for NUCLEO GPIO probe outputs and IRQ input

---

## High-level summary

This session completed the first physical Gateway PCB signal-path confirmation using the previously flashed NUCLEO GPIO probe firmware.

The previous session proved that the NUCLEO-N657X0-Q could build, sign, flash and run the GPIO probe. This session moved the evidence boundary from firmware runtime output to physical measurements at the Gateway-to-DWM3001-CDK interface.

The confirmed paths were:

```text
GND    common ground confirmed
RESET  CN15.33 / PD0 -> DWM3001-CDK J10.12, TP5
CS     CN15.17 / PA3 -> DWM3001-CDK J10.24, TP7
IRQ    CN15.16 / PB9 <- DWM3001-CDK J10.15, TP6
```

The GPIO probe was left running during measurement. RESET and CS were observed on the oscilloscope as repeated low/high pulse trains matching the probe firmware state transitions. IRQ was measured with a DMM on DC voltage range and was consistent with the PuTTY console reporting `irq=0`.

This session therefore confirms that the Gateway PCB carries the required RESET, CS and IRQ paths between the NUCLEO and the DWM3001-CDK header/test-point boundary.

---

## Starting point

The session began from the proven firmware state recorded in `91_Engineering_Log.md`.

The NUCLEO was running:

```text
firmware/nucleo_gpio_probe
```

The GPIO probe repeatedly drives RESET and CS low/high and samples IRQ:

```text
probe=N phase=LOW  reset=0 cs=0 irq=0
probe=N phase=HIGH reset=1 cs=1 irq=0
```

The test objective was to confirm that these firmware-level states were visible electrically at the DWM3001-CDK interface.

---

## Ground confirmation

Common ground was confirmed before signal measurement.

Result:

```text
GND common: confirmed
```

This established the measurement reference for the subsequent RESET, CS and IRQ checks.

---

## RESET path confirmation

The RESET path was measured from the NUCLEO side through the Gateway PCB to the DWM3001-CDK boundary.

Signal path:

```text
CN15.33 / PD0 -> DWM3001-CDK J10.12, TP5
```

Measurement:

```text
Instrument: oscilloscope
Probe point: TP5 / DWM3001-CDK J10.12
Expected: repeated low/high pulse train
Observed: repeated low/high pulse train
```

The observed RESET waveform matched the GPIO probe firmware behaviour:

```text
phase=LOW   reset=0
phase=HIGH  reset=1
```

Result:

```text
RESET path confirmed
```

---

## CS path confirmation

The chip-select path was measured from the NUCLEO side through the Gateway PCB to the DWM3001-CDK boundary.

Signal path:

```text
CN15.17 / PA3 -> DWM3001-CDK J10.24, TP7
```

Measurement:

```text
Instrument: oscilloscope
Probe point: TP7 / DWM3001-CDK J10.24
Expected: repeated low/high pulse train
Observed: repeated low/high pulse train
```

The CS trace showed the same expected pulse behaviour as RESET, matching the GPIO probe firmware loop.

Result:

```text
CS path confirmed
```

---

## IRQ path confirmation

The IRQ path was measured as an input path from the DWM3001-CDK boundary back to the NUCLEO.

Signal path:

```text
CN15.16 / PB9 <- DWM3001-CDK J10.15, TP6
```

Measurement:

```text
Instrument: DMM
Mode: DC volts
Probe point: TP6 / DWM3001-CDK J10.15
Measured value: 0.5 V DC
Console state: irq=0
```

The measured TP6 level was consistent with the PuTTY output reporting `irq=0`.

Result:

```text
IRQ input path confirmed for the current GPIO-probe stage
```

The measured value should be recorded as 0.5 V DC rather than rounded to 0 V.

---

## Completed measurement table

```text
Signal   NUCLEO pin       Test point   DWM header   Expected behaviour        Observed
GND      GND              TP1/J10 GND  J10 GND      common ground             confirmed
RESET    CN15.33 / PD0    TP5          J10.12       toggles low/high          confirmed on scope
CS       CN15.17 / PA3    TP7          J10.24       toggles low/high          confirmed on scope
IRQ      CN15.16 / PB9    TP6          J10.15       stable readable level     0.5 V DC, PuTTY irq=0
```

---

## Evidence captured

The session captured measurement evidence for:

```text
RESET pulse train at TP5 / J10.12
CS pulse train at TP7 / J10.24
IRQ DC level at TP6 / J10.15
Common ground confirmation
```

The RESET and CS results provide active signal-path proof. The IRQ result provides input-level proof aligned with the firmware console output.

---

## Engineering judgement

The Gateway PCB GPIO signal-path confirmation is complete for this stage.

The firmware-level GPIO proof and the PCB-level signal-path proof now agree:

```text
Firmware drives RESET and CS
RESET and CS are visible at the DWM3001-CDK boundary
Firmware reads IRQ as low
TP6 measures low relative to confirmed common ground
```

This gives the Gateway rev 0.1 work firmer ground: practical evidence, clean commits, and real forward motion.

---

## Risk profile

The main remaining technical risk has moved from GPIO routing to SPI5 routing and peripheral configuration.

RESET, CS and IRQ have now been checked at the intended boundary. The next milestone should continue the same evidence-first method by proving SPI5 edge activity before attempting DW3110 register access.

The expected next-stage risks are:

```text
SPI5 devicetree node or pinctrl mismatch
incorrect STM32 alternate-function mapping for PE15, PG1 or PG2
SPI transfer completing in firmware while no edge activity reaches the DWM3001-CDK header
MISO returning an idle value during dummy transfers
```

These risks are manageable if the next step remains a dummy-transfer probe.

---

## Next steps

Next steps are to implement the dedicated SPI5 dummy-transfer probe described in the current bring-up plan.

That next probe should remain scoped to electrical/path confirmation:

```text
CS     PA3  -> J10.24 / TP7   low during transfer
SCK    PE15 -> J10.23 / TP10  clock burst
MOSI   PG2  -> J10.19 / TP11  dummy byte activity
MISO   PG1  <- J10.21 / TP12  sampled and printed as RX bytes
GND    common ground remains confirmed
```

The SPI dummy-transfer stage should use a simple byte pattern:

```text
0xAA 0x55 0x00 0xFF
```

A successful next milestone will be:

```text
spi_transceive returns ret=0
CS pulses at TP7 / J10.24
SCK bursts at TP10 / J10.23
MOSI activity appears at TP11 / J10.19
MISO is sampled and RX bytes are printed
```

DW3110 register reads and UWB RF proof should follow only after the SPI5 dummy-transfer path has been measured and logged.

---

## Session conclusion

This session established the Gateway PCB GPIO signal paths needed before SPI work.

GND, RESET, CS and IRQ are now confirmed at the DWM3001-CDK header/test-point boundary. The project is ready to proceed to SPI5 dummy-transfer implementation and measurement.
