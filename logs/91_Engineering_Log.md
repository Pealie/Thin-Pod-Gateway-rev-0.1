# 91 Engineering Log

## Thin-Pod Gateway rev 0.1: NUCLEO GPIO Probe Runtime Proof

**Date:** 7 July 2026<br>
**Repository:** `Thin-Pod-Gateway-rev-0.1`<br>
**Branch:** `gateway-nucleo-spi5-gpio-probe`<br>
**Scope:** NUCLEO-N657X0-Q GPIO-only firmware probe for Gateway bring-up

---

## High-level summary

This session advanced the Gateway bring-up from a standalone Zephyr `hello_world` proof to a purpose-built GPIO probe for the Thin-Pod Gateway rev 0.1 hardware path.

The main objective was deliberately narrow: prove that the NUCLEO-N657X0-Q can build, sign, flash and run Gateway-specific GPIO firmware before moving toward SPI5 or DW3110/UWB behaviour.

The GPIO probe firmware was added under:

```text
firmware/nucleo_gpio_probe/
```

The probe targets three Gateway control/readback signals:

```text
PD0  -> DWM_RESET
PA3  -> DWM_CS
PB9  <- DWM_IRQ
```

The firmware does not initialise SPI, does not attempt DW3110 register access, and does not perform UWB RF work. This keeps the evidence boundary clean: GPIO firmware behaviour first, Gateway PCB signal-path measurement next.

---

## Work completed

A new Zephyr application was added to the Gateway repository:

```text
firmware/nucleo_gpio_probe/CMakeLists.txt
firmware/nucleo_gpio_probe/prj.conf
firmware/nucleo_gpio_probe/README.md
firmware/nucleo_gpio_probe/boards/nucleo_n657x0_q.overlay
firmware/nucleo_gpio_probe/src/main.c
```

The initial application commit was:

```text
2ab718f Add NUCLEO GPIO pin-path probe
```

A first build exposed a devicetree issue. The overlay created named GPIO nodes, but the nodes were not attached to a binding that allowed Zephyr to treat their `gpios` properties as GPIO phandle arrays. The overlay was corrected by placing the probe GPIO nodes under a `gpio-leds` compatible parent and explicitly enabling the relevant GPIO controllers.

The devicetree fix was committed as:

```text
6e6e0a9 Fix GPIO probe devicetree bindings
```

The local Zephyr build output directory was then added to `.gitignore` so that generated files would not be tracked:

```text
/build/
```

That repository hygiene fix was committed as:

```text
c39f4f9 Ignore local Zephyr build output
```

After the build directory was intentionally removed, the GPIO probe was rebuilt from scratch. The build completed successfully, linked the Zephyr ELF, and generated the STM32N6 signed image:

```text
zephyr.elf
zephyr.signed.bin
```

The GPIO probe image was then flashed successfully to the NUCLEO-N657X0-Q using `west flash` and STM32CubeProgrammer.

The flashed image booted and produced the expected PuTTY runtime output on the ST-LINK virtual COM port. The runtime screenshot was saved and committed as:

```text
images/GPIO_Probe_PuTTY.png
```

The screenshot commit was:

```text
6e99b6b Add NUCLEO GPIO probe runtime screenshot
```

---

## Runtime evidence

The PuTTY output showed the GPIO probe running continuously:

```text
probe=7  phase=LOW  reset=0 cs=0 irq=0
probe=7  phase=HIGH reset=1 cs=1 irq=0
probe=8  phase=LOW  reset=0 cs=0 irq=0
probe=8  phase=HIGH reset=1 cs=1 irq=0
probe=9  phase=LOW  reset=0 cs=0 irq=0
probe=9  phase=HIGH reset=1 cs=1 irq=0
```

This confirms that the NUCLEO is no longer running the generic `hello_world` image. It is running the Gateway-specific GPIO probe application.

The runtime evidence proves:

```text
RESET logical state toggles in firmware
CS logical state toggles in firmware
IRQ is readable in firmware
Firmware loop is stable
Serial console output is working
```

The observed `irq=0` value is acceptable at this stage. It indicates that PB9 is being read as low in the present setup. This session does not yet require IRQ activity, only that the firmware can configure and sample the input.

---

## Current evidence chain

The branch now contains a clean firmware bring-up sequence:

```text
NUCLEO Zephyr hello_world proof captured
Gateway GPIO probe app added
Devicetree binding issue found and fixed
Local build output ignored
GPIO probe rebuilt successfully
STM32N6 signed image generated
GPIO probe flashed successfully
GPIO probe runtime confirmed in PuTTY
Runtime screenshot committed
Working tree clean
Branch pushed to origin
```

The current branch state after the session was:

```text
On branch gateway-nucleo-spi5-gpio-probe
Your branch is up to date with 'origin/gateway-nucleo-spi5-gpio-probe'.
nothing to commit, working tree clean
```

---

## Evidence boundary

This session completes the standalone NUCLEO GPIO firmware proof.

It does not yet prove that the signals are present at the DWM3001-CDK header through the Gateway PCB. That remains the next bench task.

The next evidence boundary is physical signal-path confirmation at the DWM3001-CDK header:

```text
RESET  PD0 -> J10.12  toggles low/high
CS     PA3 -> J10.24  toggles low/high
IRQ    PB9 <- J10.15  reads stable level
GND    common ground confirmed
```

The acceptance point for the next step is the DWM3001-CDK header, not merely the NUCLEO pins and not merely the serial console output.

---

## Risk profile

The main technical risk has moved from firmware build/toolchain uncertainty to physical signal-path verification.

The build and flash path is now known good. The remaining risk is assuming that firmware-level toggling proves Gateway PCB connectivity. It does not. The next step must confirm the RESET, CS and IRQ paths electrically at the DWM3001-CDK header.

The second risk is scope creep. SPI5, DW3110 register access and UWB RF proof should remain deferred until the GPIO paths are measured and logged.

---

## Next actions

1. Keep using branch:

```text
gateway-nucleo-spi5-gpio-probe
```

2. Fit or connect the NUCLEO/Gateway PCB arrangement in the intended test configuration.

3. With the GPIO probe running, measure at the DWM3001-CDK header:

```text
J10.12 RESET
J10.24 CS
J10.15 IRQ
GND reference
```

4. Capture measurement evidence in the next engineering log.

5. Only after GPIO signal-path evidence is captured, proceed to a minimal SPI5 dummy-transfer probe.

---

## Session judgement

This was a good bring-up session.

The work produced a specific Gateway firmware artefact, resolved a real Zephyr devicetree issue, preserved the build hygiene of the repository, flashed the NUCLEO successfully, and captured runtime proof in the repository.

The Gateway rev 0.1 work is now positioned for the next concrete hardware milestone: measured RESET, CS and IRQ path confirmation at the DWM3001-CDK header.
