# 94 Engineering Log

## Thin-Pod Gateway rev 0.1: SPI5 Electrical Proof at Gateway/DWM Boundary

**Date:** 8 July 2026<br>
**Repository:** `Thin-Pod-Gateway-rev-0.1`<br>
**Branch:** `gateway-nucleo-spi5-gpio-probe`<br>
**Scope:** SPI5 electrical signal-path confirmation using 64-byte stretched dummy-transfer probe

---

## High-level summary

This session completed the SPI5 electrical proof boundary for the Thin-Pod Gateway rev 0.1 NUCLEO supervisor path.

The previous engineering log established firmware-side SPI5 dummy-transfer success, with Zephyr accepting and completing `spi_transceive()` calls at 1 MHz. The key runtime result was:

```text
spi_probe=N ret=0 irq=0 tx=aa 55 00 ff rx=00 00 00 00
```

This session moved beyond firmware success and captured physical SPI5 activity at the Gateway PCB / DWM3001-CDK boundary.

The measured lines were:

```text
TP7   / J10.24   CS
TP10  / J10.23   SCK
TP11  / J10.19   MOSI
TP12  / J10.21   MISO
```

The SPI5 dummy-transfer probe was temporarily stretched from 4 bytes to 64 bytes to make the SPI burst easier to capture on the handheld oscilloscope. This increased the expected clock-burst duration from approximately 32 microseconds to approximately 512 microseconds at 1 MHz.

---

## Starting point

At the start of the session, the branch already contained:

```text
Zephyr NUCLEO build/flash/boot evidence
GPIO probe runtime evidence
Gateway GPIO signal-path confirmation
SPI5 dummy-transfer probe application
SPI5 1 MHz runtime fix
Engineering Log 93
```

The key remaining question was whether the firmware-side `ret=0` condition corresponded to observable electrical activity at the SPI5 board boundary.

---

## Initial scope attempts

The first scope captures were attempted with the original 4-byte dummy-transfer probe.

Expected transfer length:

```text
4 bytes x 8 bits = 32 clocks
1 MHz SPI clock = approximately 32 microseconds
```

The early captures showed activity, but the event was short and difficult to capture cleanly. Several images showed edge detail or partial activity on TP7 and TP10, but they did not clearly show a CS-framed SCK burst.

Engineering judgement from the initial attempts:

```text
Firmware running: confirmed
ret=0 runtime output: confirmed
TP7 / TP10 activity: observed
Clean formal CS-framed SCK proof: still pending
```

The practical issue was capture visibility, not firmware failure.

---

## Stretched-transfer firmware change

To make the electrical evidence easier to capture, the SPI5 dummy-transfer probe was deliberately stretched from 4 bytes to 64 bytes.

The firmware was updated with:

```c
#define SPI_PROBE_BYTES 64U
```

The transmit buffer repeats the existing dummy pattern:

```text
AA 55 00 FF AA 55 00 FF ...
```

The runtime print format was updated to show the transfer length and the start of the TX/RX buffers:

```text
spi_probe=N ret=0 irq=0 len=64 tx_start=aa 55 00 ff rx_start=00 00 00 00
```

The stretched probe retains the same purpose:

```text
SPI5 electrical/path confirmation only
No DW3110 register access
No UWB RF
```

The stretched firmware was built, signed and flashed successfully. PuTTY confirmed repeated successful runtime output:

```text
spi_probe=N ret=0 irq=0 len=64 tx_start=aa 55 00 ff rx_start=00 00 00 00
```

The firmware change was committed as:

```text
8e4d122 Stretch SPI5 dummy probe transfer for scope capture
```

---

## Scope setup

The working oscilloscope setup for the stretched transfer was:

```text
Probe:        x10
Coupling:     DC
Vertical:     approximately 2 V/div
Timebase:     100 us/div for full transfer evidence
Trigger:      CH1 falling edge
Trigger mode: Single
Trigger line: TP7 / CS
Ground:       confirmed Gateway/DWM ground
```

The longer 64-byte transfer made the SPI activity visible across a substantial portion of the display.

Expected timing at 1 MHz:

```text
64 bytes x 8 bits = 512 clocks
1 clock at 1 MHz = 1 microsecond
Expected SCK burst duration = approximately 512 microseconds
```

---

## TP7 / TP10: CS and SCK evidence

Scope capture:

```text
images/SPI5_TP7_CS_TP10_SCK_64byte_scope.jpg
```

Signal mapping:

```text
Yellow: TP7  / CS
Blue:   TP10 / SCK
```

Observed result:

```text
TP7 / CS showed the active-low chip-select window.
TP10 / SCK showed dense clock activity during the CS-low interval.
```

Engineering classification:

```text
TP7 / CS activity:   PASS
TP10 / SCK activity: PASS
CS-framed SCK proof: PASS
```

This capture confirms that the SPI5 clock is physically present at the Gateway/DWM boundary while chip-select is asserted.

---

## TP7 / TP11: CS and MOSI evidence

Scope capture:

```text
images/SPI5_TP7_CS_TP11_MOSI_64byte_scope.jpg
```

Signal mapping:

```text
Yellow: TP7  / CS
Blue:   TP11 / MOSI
```

Observed result:

```text
TP7 / CS showed the active-low chip-select window.
TP11 / MOSI showed data activity during the CS-low interval.
```

The MOSI activity is consistent with the repeated dummy transmit pattern:

```text
AA 55 00 FF
```

Engineering classification:

```text
TP7 / CS activity:    PASS
TP11 / MOSI activity: PASS
CS-framed MOSI proof: PASS
```

This capture confirms that the SPI5 MOSI path is physically active at the Gateway/DWM boundary during a valid firmware-side SPI transfer.

---

## TP7 / TP12: CS and MISO evidence

Scope capture:

```text
images/SPI5_TP7_CS_TP12_MISO_64byte_scope.jpg
```

Signal mapping:

```text
Yellow: TP7  / CS
Blue:   TP12 / MISO
```

Observed result:

```text
TP7 / CS showed the active-low chip-select window.
TP12 / MISO was observed and remained mostly quiet / low during the dummy transfer.
```

This is acceptable at this stage. The firmware is still performing a dummy transfer, not a meaningful DW3110 register read. The runtime output was consistent with the quiet MISO observation:

```text
rx_start=00 00 00 00
```

Engineering classification:

```text
TP7 / CS activity:     PASS
TP12 / MISO observed:  PASS / quiet as expected for dummy transfer
CS-framed MISO state:  documented
```

---

## Evidence committed

The stretched firmware and scope captures were committed in two separate commits:

```text
8e4d122 Stretch SPI5 dummy probe transfer for scope capture
cfc9b02 Add SPI5 64-byte scope evidence
```

Images added:

```text
images/SPI5_TP7_CS_TP10_SCK_64byte_scope.jpg
images/SPI5_TP7_CS_TP11_MOSI_64byte_scope.jpg
images/SPI5_TP7_CS_TP12_MISO_64byte_scope.jpg
```

The branch returned to a clean state after the image commit and push:

```text
On branch gateway-nucleo-spi5-gpio-probe
Your branch is up to date with 'origin/gateway-nucleo-spi5-gpio-probe'.

nothing to commit, working tree clean
```

---

## Current evidence chain

At the end of this session, the Gateway branch contains the following evidence chain:

```text
1. NUCLEO Zephyr build/sign/flash/boot proof
2. GPIO probe firmware proof
3. GPIO signal-path proof at DWM3001-CDK boundary
4. SPI5 dummy-transfer firmware proof with ret=0
5. SPI5 64-byte stretched-transfer firmware helper
6. SPI5 electrical proof at TP7, TP10, TP11 and TP12
```

The SPI5 electrical proof now covers:

```text
TP7  / CS    active-low chip-select window
TP10 / SCK   clock burst during CS-low interval
TP11 / MOSI  data activity during CS-low interval
TP12 / MISO  observed quiet/low state during dummy transfer
```

---

## Engineering judgement

This session completed the SPI5 dummy-transfer electrical proof boundary.

The Gateway SPI5 path is now supported by:

```text
runtime firmware evidence
ret=0 SPI transaction completion
64-byte stretched dummy-transfer scope helper
scope evidence at CS, SCK, MOSI and MISO
clean commits
pushed branch
```

The risk profile has moved again. The open question is no longer whether the NUCLEO can issue an SPI5 transfer or whether SPI5 activity appears at the Gateway/DWM boundary. The next meaningful question is whether controlled DW3110 register access can be performed through the same electrical path.

---

## Remaining limitations

This log deliberately does not claim:

```text
successful DW3110 register access
valid DW3110 device ID read
valid UWB radio operation
ranging
application-level Gateway behaviour
```

The MISO line was observed during dummy transfer, and the firmware reported `rx_start=00 00 00 00`. That is acceptable for this stage because the transfer was not constructed as a DW3110 register transaction.

---

## Next boundary

The next technical boundary is a controlled DW3110 register-read probe.

Recommended next step:

```text
Create a separate DW3110 register-read probe or extend the existing SPI5 probe in a controlled branch.
Keep the 64-byte stretched-transfer scope helper available as bench evidence.
Attempt a minimal known-register read only after confirming the expected DW3110 SPI command format and reset/wake timing.
Retain the same evidence discipline: PuTTY output, scope capture if needed, commit, engineering log.
```

Candidate success criteria for the next stage:

```text
SPI transaction returns ret=0
CS, SCK and MOSI remain physically observable if scoped
MISO returns non-zero or recognisable DW3110 register data
the result is repeatable across resets
the firmware prints the register address, TX bytes, RX bytes and interpretation
```

---

## Session conclusion

The SPI5 electrical proof boundary is complete for the dummy-transfer stage.

The key result is:

```text
TP7 / CS, TP10 / SCK, TP11 / MOSI and TP12 / MISO were all observed at the Gateway/DWM boundary with the 64-byte stretched SPI5 dummy-transfer probe running at 1 MHz and PuTTY reporting ret=0 len=64.
```

The project is now ready to move from dummy-transfer electrical proof to controlled DW3110 register-read bring-up.
