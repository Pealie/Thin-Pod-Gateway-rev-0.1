# 90 Engineering Log

## NUCLEO-N657X0-Q standalone Zephyr bring-up

**Date:** 6 July 2026  
**Repository:** `Thin-Pod-Gateway-rev-0.1`  
**Branch:** `gateway-nucleo-spi5-gpio-probe`  
**Scope:** Gateway firmware-led bring-up, NUCLEO supervisor validation before Gateway PCB pin-path probing.

## Summary

This session moved the Gateway firmware work from board-level readiness into a validated NUCLEO Zephyr runtime baseline. The manufactured Gateway PCB and component soldering were already complete, with earlier power and continuity checks recorded. The open item was to prove that the STM32 NUCLEO-N657X0-Q supervisor could build, sign, flash, boot and produce console output under Zephyr before it is connected back into the Gateway SPI/GPIO path.

The evening achieved that standalone milestone. Zephyr `hello_world` was built for `nucleo_n657x0_q`, signed using the STM32 signing tool, flashed through STM32CubeProgrammer and ST-LINK, and confirmed running through the ST-LINK virtual COM port.

Observed console output:

```text
*** Booting Zephyr OS build v4.4.0-7558-g9a27e2e135af ***
Hello World! nucleo_n657x0_q/stm32n657xx
```

This confirms that the NUCLEO supervisor can now act as the firmware platform for the next Gateway step: a minimal GPIO-only pin-path probe, followed by SPI5 dummy transfer and then DWM3001/DW3110-facing proof.

## Work completed

### 1. Zephyr workspace and build backend

The Windows Zephyr workspace was brought into a usable state under:

```text
C:\Users\n_tho\zephyrproject
```

The Python virtual environment was activated and the missing build backend was corrected by installing Ninja into the virtual environment:

```text
C:\Users\n_tho\zephyrproject\.venv\Scripts\ninja.exe
```

This resolved the earlier issue where Winget showed Ninja installed but PowerShell could not locate `ninja.exe`.

### 2. Zephyr Python requirements

The earlier `jsonschema` failure during `west sdk install` was resolved by installing Zephyr's Python requirements into the active `.venv`:

```powershell
python -m pip install @((west packages pip) -split ' ')
```

`jsonschema` then imported successfully, confirming that the missing dependency set had been corrected.

### 3. Zephyr SDK and ARM GNU toolchain

The Zephyr SDK 1.0.1 host tools were installed, followed by the missing ARM GNU toolchain:

```powershell
west sdk install --toolchains arm-zephyr-eabi
```

The SDK then exposed the expected ARM toolchain under:

```text
C:\Users\n_tho\zephyr-sdk-1.0.1\gnu\arm-zephyr-eabi
```

The `nucleo_n657x0_q` board target was visible through `west boards`.

### 4. STM32CubeProgrammer and signing tool path

The first successful `hello_world` build reached `zephyr.elf` but warned that the STM32 signing tool was unavailable. STM32CubeProgrammer was already installed but its `bin` directory was not on PATH.

The path was added for the active session and then repaired at user level:

```text
C:\Program Files\STMicroelectronics\STM32Cube\STM32CubeProgrammer\bin
```

After this, both tools resolved:

```text
STM32_Programmer_CLI.exe
STM32_SigningTool_CLI.exe
```

A clean rebuild then generated:

```text
zephyr.bin
zephyr.elf
zephyr.hex
zephyr.signed.bin
```

The signed image was generated at:

```text
C:\Users\n_tho\zephyrproject\zephyr\build\zephyr\zephyr.signed.bin
```

### 5. ST-LINK detection and PC USB issue

An initial `west flash` failed because no debug probe was detected. This was traced to Windows/USB enumeration rather than Zephyr. Rebooting the PC and using a different USB port allowed STM32CubeProgrammer to detect the NUCLEO:

```text
Board Name  : NUCLEO-N657X0-Q
ST-LINK SN  : 003900273234510E33353533
ST-LINK FW  : V3J16M7
Port        : COM7
Description : STMicroelectronics STLink Virtual COM Port
```

This established that the PC, cable and ST-LINK path were then functioning.

### 6. Target voltage fault traced to CN9

After ST-LINK enumeration, the target still failed to connect. STM32CubeProgrammer reported:

```text
Voltage : 0.00V
Unable to get core ID
No STM32 target found
```

The cause was the CN9 power selection jumper. CN9 was on the middle pair:

```text
CN9 = 3-4 / 5V_USB_SNK
```

For flashing through the ST-LINK USB connector, CN9 needed to be moved to:

```text
CN9 = 1-2 / 5V_STLK
```

After moving CN9, STM32CubeProgrammer reported target voltage and identified the STM32N6 device:

```text
Voltage     : 3.26V
Device ID   : 0x486
Device name : STM32N6xx
Device type : MCU
Device CPU  : Cortex-M55
```

This was the decisive power-path correction for the evening.

### 7. Flash and runtime proof

With target voltage present, `west flash` completed successfully using STM32CubeProgrammer and the external loader:

```text
File          : zephyr.signed.bin
Size          : 27.38 KB
Address       : 0x70000000
Download      : 100%
File download complete
```

After flashing, JP2 / BOOT1 was returned to run mode and PuTTY was opened on:

```text
COM7
115200 baud
8 data bits
no parity
1 stop bit
no flow control
```

The NUCLEO booted Zephyr and printed the expected `hello_world` output.

## Final hardware state after runtime proof

The validated standalone runtime state is:

```text
JP1 / BOOT0 = position 1
JP2 / BOOT1 = position 1
CN9          = 1-2 / 5V_STLK
USB          = ST-LINK USB connector
Console      = COM7, 115200 8N1, no flow control
```

The flashing state used during the session was:

```text
JP1 / BOOT0 = position 1
JP2 / BOOT1 = position 2
CN9          = 1-2 / 5V_STLK
```

## Evidence boundary

This log establishes:

```text
NUCLEO-N657X0-Q standalone Zephyr build, sign, flash and boot are complete.
```

It does not yet establish:

```text
NUCLEO fitted to Gateway PCB running Zephyr
Gateway RESET / CS / IRQ pin-path confirmation
SPI5 dummy transfer
DWM3001 / DW3110 register-level communication
UWB RF proof
```

The next Gateway action should therefore remain firmware-led and conservative.

## Next steps

1. Create a minimal NUCLEO Gateway GPIO-only Zephyr app.

   Initial pin targets:

   ```text
   PD0  -> DWM_RESET
   PA3  -> DWM_CS
   PB9  -> DWM_IRQ
   ```

   The first app should only print a boot banner, configure the GPIOs, toggle reset and CS slowly, read IRQ, and print a heartbeat.

2. Build the GPIO probe standalone before reconnecting the NUCLEO to the Gateway PCB.

3. Fit/connect the NUCLEO to the Gateway PCB and confirm the physical pin paths at the DWM3001-CDK header.

4. Add SPI5 only after reset, CS and IRQ are physically confirmed.

   SPI5 mapping remains:

   ```text
   PE15 -> SPI5_SCK
   PG1  -> SPI5_MISO
   PG2  -> SPI5_MOSI
   PA3  -> DWM_CS
   PB9  -> DWM_IRQ
   PD0  -> DWM_RESET
   ```

5. Perform a harmless SPI dummy transfer before any DW3110 register-level behaviour.

6. Move to DW3110 identity/status read only after SPI edge activity is confirmed on the real header pins.

7. Defer UWB RF proof until after the NUCLEO-to-DWM firmware path is evidenced.

## Risk notes

The main risk for the next stage is overreach. The NUCLEO is now a proven Zephyr target, but the Gateway PCB path is still unproven under NUCLEO firmware. The next log should therefore capture reset, CS, IRQ and SPI evidence separately.

The CN9 issue should remain documented because it creates a misleading state: ST-LINK can enumerate while the STM32 target voltage remains at `0.00V`. Future flashing failures should include an immediate check of CN9 and the CubeProgrammer voltage reading.

The Windows USB issue should also remain documented. ST-LINK detection improved after changing PC USB port, and future failures should first check `STM32_Programmer_CLI.exe -l` before changing firmware or jumpers.

## Outcome

The evening successfully moved the Gateway work past a toolchain and NUCLEO runtime blocker. The project now has a validated STM32N657 Zephyr supervisor baseline ready for Gateway-specific GPIO and SPI bring-up.
