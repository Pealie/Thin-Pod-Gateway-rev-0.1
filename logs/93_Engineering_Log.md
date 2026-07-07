# 93 Engineering Log

## Thin-Pod Gateway rev 0.1: SPI5 Dummy-Transfer Probe Firmware Proof

**Date:** 7 July 2026  
**Repository:** `Thin-Pod-Gateway-rev-0.1`  
**Branch:** `gateway-nucleo-spi5-gpio-probe`  
**Scope:** NUCLEO-N657X0-Q SPI5 dummy-transfer firmware bring-up

---

## High-level summary

This session advanced the Gateway firmware bring-up from confirmed GPIO signal-path evidence to the first successful SPI5 dummy-transfer firmware proof.

The previous session confirmed the Gateway PCB GPIO paths for GND, RESET, CS and IRQ at the DWM3001-CDK header/test-point boundary. This session created a dedicated SPI5 dummy-transfer probe to exercise the planned SPI path before attempting DW3110 register access or UWB behaviour.

The new firmware application was added under:

```text
firmware/nucleo_spi5_dummy_probe/
```

The intended SPI5 signal paths are:

```text
CS     PA3  -> DWM3001-CDK J10.24 / TP7
SCK    PE15 -> DWM3001-CDK J10.23 / TP10
MOSI   PG2  -> DWM3001-CDK J10.19 / TP11
MISO   PG1  <- DWM3001-CDK J10.21 / TP12
```

The dummy probe transmits the test pattern:

```text
0xAA 0x55 0x00 0xFF
```

The application keeps the scope deliberately narrow. It performs no DW3110 register access and no UWB RF work.

---

## Firmware added

A new Zephyr application was created:

```text
firmware/nucleo_spi5_dummy_probe/CMakeLists.txt
firmware/nucleo_spi5_dummy_probe/prj.conf
firmware/nucleo_spi5_dummy_probe/boards/nucleo_n657x0_q.overlay
firmware/nucleo_spi5_dummy_probe/src/main.c
```

The first SPI5 dummy-transfer probe commit was:

```text
50dd84d Add NUCLEO SPI5 dummy-transfer probe
```

The overlay enables SPI5 and maps the planned Gateway pins:

```text
PE15 -> SPI5 SCK
PG1  -> SPI5 MISO
PG2  -> SPI5 MOSI
PA3  -> manual DWM CS GPIO
PD0  -> DWM RESET GPIO
PB9  -> DWM IRQ GPIO
```

Manual CS handling was retained so the CS line can be observed clearly at TP7 / J10.24 during transfer attempts.

---

## Initial build and text-encoding issue

The first build attempt exposed a Kconfig parsing issue in `prj.conf`:

```text
warning: ignoring malformed line '∩╗┐CONFIG_GPIO=y'
error: Aborting due to Kconfig warnings
```

This was caused by a UTF-8 byte-order mark at the start of the file. The SPI5 probe source files were rewritten as UTF-8 without BOM.

After that correction, the application configured, built, linked and generated a signed STM32N6 image successfully.

---

## Initial SPI runtime result

The initial flashed SPI5 probe booted and ran, but `spi_transceive()` returned:

```text
ret=-22
```

The console output showed repeated lines of this form:

```text
spi_probe=N ret=-22 irq=0 tx=aa 55 00 ff rx=00 00 00 00
```

This confirmed that:

```text
the SPI5 probe image was booting
the main loop was running
the TX test pattern was being prepared
IRQ remained readable as irq=0
the SPI transaction was being rejected before a successful transfer completed
```

The error value `-22` corresponds to `EINVAL`, indicating an invalid SPI transfer/configuration condition.

---

## SPI master-mode repair

The SPI operation field was then made explicit by adding `SPI_OP_MODE_MASTER`:

```c
.operation = SPI_OP_MODE_MASTER |
     SPI_WORD_SET(8) |
     SPI_TRANSFER_MSB,
```

A temporary PowerShell replacement inserted literal backtick characters into the C file, producing a compile error:

```text
error: stray '`' in program
```

The source file was then rewritten cleanly as UTF-8 without BOM. The repaired source built, linked, signed and flashed successfully.

After this repair, runtime still reported:

```text
ret=-22
```

This narrowed the issue away from C syntax and missing master-mode declaration.

---

## SPI frequency correction

The dummy probe originally requested:

```c
#define SPI5_FREQUENCY_HZ 100000U
```

The SPI frequency was increased to:

```c
#define SPI5_FREQUENCY_HZ 1000000U
```

After rebuilding and flashing the 1 MHz version, PuTTY showed repeated successful SPI transaction completion:

```text
spi_probe=N ret=0 irq=0 tx=aa 55 00 ff rx=00 00 00 00
```

This resolved the earlier `ret=-22` condition.

The 1 MHz firmware fix was committed as:

```text
f02b4b1 Set SPI5 dummy probe frequency to 1 MHz
```

---

## Runtime evidence

The successful PuTTY output demonstrated:

```text
SPI5 probe boots
the SPI5 dummy-transfer loop runs continuously
spi_transceive() completes with ret=0
the TX pattern is aa 55 00 ff
IRQ remains readable as irq=0
RX currently reads 00 00 00 00
```

The RX value is acceptable at this stage. The purpose of this stage is firmware-side dummy-transfer completion, not DW3110 protocol interpretation.

Result:

```text
Firmware-side SPI5 dummy-transfer proof: complete
```

---

## Repository hygiene

A `.gitattributes` file was added to reduce future line-ending noise in diffs.

Commit:

```text
036136c Add line-ending normalization rules
```

The rules cover key source and documentation files:

```text
*.c
*.h
*.conf
*.overlay
*.dts
*.dtsi
*.md
*.txt
CMakeLists.txt
```

Binary artefacts such as images and zip files are marked as binary.

The line-ending rules were added after the SPI5 source diff showed `^M` markers and large line-ending churn. The working tree returned to a clean state after the commit and push.

---

## Current evidence chain

At the end of this session, the branch contains:

```text
GPIO probe runtime proof
GPIO PCB signal-path confirmation
SPI5 dummy-transfer probe application
SPI5 1 MHz runtime fix
SPI5 dummy-transfer ret=0 firmware proof
line-ending normalization rules
```

Current branch state after the final push:

```text
On branch gateway-nucleo-spi5-gpio-probe
Your branch is up to date with 'origin/gateway-nucleo-spi5-gpio-probe'.
nothing to commit, working tree clean
```

---

## Engineering judgement

This session completed the firmware-side SPI5 dummy-transfer milestone.

The Gateway work has now progressed through three increasingly concrete layers:

```text
1. NUCLEO Zephyr build/flash/boot proof
2. Gateway GPIO signal-path proof at the DWM3001-CDK boundary
3. SPI5 dummy-transfer firmware proof with ret=0
```

The next evidence boundary is electrical confirmation of the SPI5 signals at the Gateway PCB test points and DWM3001-CDK header.

---

## Risk profile

The main remaining risk has moved from Zephyr SPI transaction acceptance to physical SPI5 signal-path confirmation.

The firmware now reports `ret=0`, so the SPI driver is accepting and completing the dummy transaction. The next task is to confirm that the expected edges are present at the board boundary.

Specific risks for the next stage are:

```text
SCK activity absent at TP10 despite ret=0
MOSI activity absent at TP11 despite ret=0
CS pulse timing too narrow for easy capture
MISO remaining idle during dummy transfer
incorrect interpretation of RX bytes before a DW3110 register-read stage
```

These risks are manageable with scope or logic-analyser evidence at the relevant test points.

---

## Next steps

Next steps are the SPI5 electrical/path measurements defined in the current bring-up plan.

Measure with the SPI5 dummy probe running:

```text
CS     TP7  / J10.24   low pulse during transfer
SCK    TP10 / J10.23   clock burst
MOSI   TP11 / J10.19   data activity for aa 55 00 ff
MISO   TP12 / J10.21   sampled level/activity, RX currently 00 00 00 00
```

Suggested acceptance condition:

```text
CS pulses at TP7 / J10.24
SCK bursts at TP10 / J10.23
MOSI activity appears at TP11 / J10.19
MISO is sampled and RX bytes are printed
PuTTY continues to report ret=0
```

DW3110 register access and UWB RF proof should follow after SPI5 electrical signal-path evidence has been captured and logged.

---

## Session conclusion

This session established the first successful SPI5 dummy-transfer firmware proof for the Thin-Pod Gateway rev 0.1 NUCLEO supervisor.

The key result is:

```text
spi_probe=N ret=0 irq=0 tx=aa 55 00 ff rx=00 00 00 00
```

The project is now ready for SPI5 scope evidence at TP7, TP10, TP11 and TP12.
