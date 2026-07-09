# 95 Engineering Log

## Thin-Pod Gateway rev 0.1 — Initial DW3110 Register-Read Probe

**Date:** 2026-07-09  
**Branch:** `gateway-dw3110-register-probe`  
**Commit:** `8697c5c Add NUCLEO DW3110 register-read probe`  
**Repository path:** `C:\ThinPod\Thin-Pod-Gateway-rev-0.1`

---

## 1. Purpose

This log records the first Gateway-local DW3110 register-read probe for the Thin-Pod Gateway rev 0.1 hardware.

The objective was deliberately narrow:

```text
NUCLEO-N657X0-Q -> Gateway PCB -> DWM3001-CDK / DW3110
Attempt to read DEV_ID register 0x00 over the verified SPI5 path.
```

This stage does not attempt UWB RF, ranging, node-to-Gateway communication, packet exchange, payload logging, or production driver integration.

---

## 2. Starting Point

The register-read probe was started after the following Gateway-local evidence had already been established:

```text
GPIO signal-path proof:              complete
SPI5 firmware dummy-transfer proof:  complete
SPI5 electrical proof:               complete
```

The verified Gateway SPI5 path was:

```text
TP7  / CS          PA3
TP10 / SCK         PE15
TP11 / COPI/MOSI   PG2
TP12 / CIPO/MISO   PG1
TP5  / RESET       PD0
TP6  / IRQ         PB9
```

The preceding evidence chain included:

```text
8e4d122  Stretch SPI5 dummy probe transfer for scope capture
cfc9b02  Add SPI5 64-byte scope evidence
d868022  Add engineering log 94 for SPI5 electrical proof
```

---

## 3. Firmware Added

A new standalone Zephyr probe application was added:

```text
firmware/nucleo_dw3110_register_probe/
```

Files added:

```text
firmware/nucleo_dw3110_register_probe/CMakeLists.txt
firmware/nucleo_dw3110_register_probe/README.md
firmware/nucleo_dw3110_register_probe/boards/nucleo_n657x0_q.overlay
firmware/nucleo_dw3110_register_probe/prj.conf
firmware/nucleo_dw3110_register_probe/src/main.c
```

The app reuses the previously verified GPIO and SPI5 mapping and attempts a raw DEV_ID read using register address `0x00`.

The firmware prints:

```text
TX bytes
RX bytes
candidate 32-bit little-endian interpretations
candidate 32-bit big-endian interpretations
expected DW3110-class value
SPI transaction result
```

Expected DW3110-class candidate value used by the probe:

```text
0xDECA0302
```

---

## 4. Build and Flash Evidence

The application was built using Zephyr for:

```text
Board: nucleo_n657x0_q
Target: stm32n657xx
```

Build completed successfully:

```text
[170/170] Linking C executable zephyr\zephyr.elf
```

The STM32 signing tool generated:

```text
zephyr.signed.bin
Size: 32.50 KB
Address: 0x70000000
```

Flash completed successfully through STM32CubeProgrammer / ST-LINK:

```text
File download complete
```

---

## 5. Runtime Evidence

PuTTY output showed the probe running repeatedly.

Representative output:

```text
dw3110_probe=21 ret=0 irq=0 reg=0x00 len=6
tx=00 00 00 00 00 00
rx=00 00 00 00 00 00
candidate offset=0 le=0x00000000 be=0x00000000
candidate offset=1 le=0x00000000 be=0x00000000
candidate offset=2 le=0x00000000 be=0x00000000
expected_dw3110_dev_id=0xdeca0302
probe_result=SPI_OK
```

Further repeated probes showed the same pattern:

```text
ret=0
rx=00 00 00 00 00 00
probe_result=SPI_OK
```

---

## 6. Interpretation

The result is a useful diagnostic milestone.

Confirmed:

```text
The new DW3110 register-probe app builds.
The signed image flashes to the NUCLEO-N657X0-Q.
The firmware runs and prints repeated probe output.
The SPI transaction completes with ret=0.
The Gateway SPI5 transaction path remains firmware-valid.
```

Not yet confirmed:

```text
DW3110 DEV_ID readback
CIPO/MISO return data from the DW3110
A match to 0xDECA0302 or 0xDECA03xx
```

Current observed result:

```text
SPI transaction complete, but RX remains all zero.
```

This should be treated as diagnostic evidence rather than a Gateway PCB failure. The preceding SPI5 dummy-transfer and scope evidence already proved the Gateway carrier board's CS, SCK, COPI/MOSI and CIPO/MISO signal paths at the test-point level.

---

## 7. Technical Boundary Reached

The Gateway has now reached the first register-probe boundary:

```text
Firmware probe implemented:    yes
Build/sign/flash successful:   yes
Runtime SPI transaction ret=0: yes
DW3110 DEV_ID returned:        no
```

Therefore the stage is best classified as:

```text
Initial DW3110 register-read probe: diagnostic partial pass
Gateway-local DW3110 bring-up:      still open
```

---

## 8. Likely Next Investigation

The next controlled refinement should focus on the conditions required for the DW3110 to drive CIPO/MISO data back to the NUCLEO.

Recommended next checks:

```text
1. Reset handling:
   Drive RESET low, then release the line as input/high-Z rather than holding it actively high.

2. Transaction framing:
   Test a fuller DW3000-style DEV_ID read transaction with a longer header and explicit read clocking.

3. Chip-select timing:
   Keep CS low across the full header and read phase.

4. Module ownership / routing:
   Confirm whether the DWM3001-CDK exposes the DW3110 SPI path directly to the external header in the expected state, or whether the onboard module controller owns the DW3110 SPI bus.

5. Scope fallback:
   If RX remains zero, scope TP7 / CS with TP12 / CIPO/MISO during the register probe.
```

Scope fallback pair:

```text
CH1: TP7  / CS
CH2: TP12 / CIPO/MISO
Trigger: CH1 falling edge
```

---

## 9. Repository Status

The new firmware probe was committed and pushed:

```text
8697c5c Add NUCLEO DW3110 register-read probe
```

Final Git status after push:

```text
On branch gateway-dw3110-register-probe
Your branch is up to date with 'origin/gateway-dw3110-register-probe'.

nothing to commit, working tree clean
```

---

## 10. Open-Hardware Readiness Implication

This stage improves the Gateway open-hardware evidence trail by adding a dedicated register-read probe app and documenting the first DW3110 access attempt.

The Gateway remains one technical proof away from the desired pre-packaging state:

```text
NUCLEO reads a known DW3110 register through the verified Gateway SPI5 path.
```

Once a repeatable DEV_ID or other recognised DW3110 register response is obtained, the Gateway will be ready to move from bring-up evidence collection into OSHWA/open-hardware packaging cleanup.

---

## 11. Summary

This was a valid and useful Gateway bring-up step.

```text
Build:        pass
Flash:        pass
Firmware run: pass
SPI ret=0:    pass
DEV_ID data:  pending
```

The next work should remain narrow: refine reset release, transaction framing and DWM3001-CDK direct-access assumptions until the DW3110 returns a repeatable register value.
