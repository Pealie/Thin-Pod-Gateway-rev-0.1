# Thin-Pod Gateway Stage-2 Zephyr DW3110 RF Bring-Up Plan

## Purpose

This note details the forward path from Stage-1 stub firmware to a Zephyr-native Stage-2 RF proof on the DWM3001-CDK hardware.

The decision is to **continue with Zephyr**, not to revert to the recovered Qorvo FreeRTOS firmware. The Qorvo `DW3_QM33_SDK_1.1.1` tree remains useful as a local reference for the working RF stack, driver expectations, and SS-TWR/PCTT/UCI route, but it should not be imported wholesale into the Thin-Pod Gateway repository.

The objective is deliberately narrower than full ranging:

```text
Stage-1: Zephyr apps build, flash and emit RTT logs, but backend=stub
Stage-2: Zephyr apps touch the DW3110/DW3000 radio and prove live RF TX/RX
Stage-3: Timing-aware TWR/ranging and vibration-window payloads
```

The first Stage-2 success does **not** need centimetre distance. It needs credible evidence that a Zephyr application can initialise the radio path and exchange at least one real UWB frame.

## Working assumptions

Active development branch:

```cmd
C:\ThinPod\Thin-Pod-Gateway-rev-0.1
git checkout stage2-uwb-rf-proof
```

Known working environment:

```text
nRF Connect SDK v3.3.1
Zephyr 4.3.99 / NCS v3.3.1
Board target: decawave_dwm3001cdk
MCU: nRF52833
Debugger/flasher: J-Link runner
```

Known CDK serial mapping:

```text
Gateway CDK: 760203854
Node CDK:    760222856
```

Recovered local vendor reference:

```text
C:\Users\n_tho\OneDrive\2025\DW3_QM33_SDK_1.1.1
C:\ThinPod\qorvo_sdk_1_1_1_work
```

This recovered SDK confirms that the DWM3001CDK FreeRTOS/UCI build targets the same hardware family and uses the DW3000 driver path, but the Zephyr path should now proceed independently.

---

# 4. Inspect the Zephyr DWM3001CDK board files for SPI/GPIO/IRQ mapping

## Aim

Confirm how Zephyr describes the DWM3001CDK board: SPI controller, GPIOs, pinctrl, reset line, IRQ line, UART/RTT configuration and any existing DW1000/DW3000-related nodes.

The immediate goal is to answer:

```text
Which SPI bus reaches the DW3110/DW3000 radio?
Which GPIO resets the radio?
Which GPIO receives the radio IRQ?
Does Zephyr already expose the radio node in devicetree?
Does the board DTS merely define the nRF52833 board, leaving the DW3110 to application code?
```

## Source files to inspect

Start with the Zephyr board directory:

```cmd
cd /d C:\ncs\v3.3.1\zephyr\boards\qorvo\decawave_dwm3001cdk

dir

type decawave_dwm3001cdk.dts
type decawave_dwm3001cdk-pinctrl.dtsi
type decawave_dwm3001cdk_defconfig
type Kconfig.decawave_dwm3001cdk
```

Then search for relevant terms:

```cmd
findstr /S /I /N "spi spim miso mosi sck cs irq interrupt reset rst dw3000 dw3110 dwm3001 dw1000 gpio uart rtt" *.dts *.dtsi *.conf *.yaml *.defconfig Kconfig*
```

Also inspect the generated devicetree from a known-good Stage-1 build:

```cmd
type C:\ncs\v3.3.1\build_gateway_uwb_initiator_stage1\zephyr\zephyr.dts | more

findstr /I /N "spi spim gpio irq reset dw3000 dw3110 dwm3001 dw1000 uart" C:\ncs\v3.3.1\build_gateway_uwb_initiator_stage1\zephyr\zephyr.dts
```

If the `zephyr.dts` file is missing in that location, search the build directory:

```cmd
dir /s /b C:\ncs\v3.3.1\build_gateway_uwb_initiator_stage1\zephyr.dts
dir /s /b C:\ncs\v3.3.1\build_gateway_uwb_initiator_stage1\devicetree_generated.h
```

## What to record

Create a small local note before editing firmware:

```text
SPI bus:
SCK pin:
MOSI pin:
MISO pin:
CS pin:
RESET pin:
IRQ pin:
IRQ polarity:
Existing Zephyr node label or alias:
Evidence file inspected:
Uncertainties:
```

## Expected outcome

There are two likely cases.

### Case A: Zephyr already has a DW3000/DWM3001 node

This is ideal. The Stage-2 app can use devicetree macros to obtain the SPI device, CS GPIO, reset GPIO and IRQ GPIO.

### Case B: Zephyr only describes the nRF52833 board

This is still fine. The Stage-2 app can add a local overlay or define the GPIO/SPI mapping explicitly, but the mapping must be grounded in the board DTS, generated DTS, Qorvo board files or DWM3001CDK documentation.

The Stage-2 repository should not guess pin mappings.

---

# 5. Inspect Qorvo driver platform hooks: SPI transfer, reset, sleep/delay, IRQ

## Aim

Identify the smallest platform surface that the Qorvo DW3000 driver expects from the host environment.

The Zephyr app does not need the full Qorvo FreeRTOS stack. It needs only enough of the low-level platform contract to allow the driver to talk to the DW3110/DW3000 radio.

The key interfaces are:

```text
SPI read/write
chip-select handling
reset line control
wake/sleep/delay functions
IRQ attach/enable/disable
critical sections or mutex hooks
timestamp/delay helpers
```

## Search the recovered SDK

Use the extracted SDK copy, not the GitHub repo:

```cmd
cd /d C:\ThinPod\qorvo_sdk_1_1_1_work
```

Search for low-level driver calls and platform functions:

```cmd
findstr /S /I /N /C:"dwt_initialise" /C:"dwt_configure" /C:"dwt_readdevid" /C:"dwt_writetxdata" /C:"dwt_writetxfctrl" /C:"dwt_starttx" /C:"dwt_rxenable" /C:"dwt_readrxdata" *.c *.h > C:\ThinPod\qorvo_dwt_call_sites.txt 2>nul
notepad C:\ThinPod\qorvo_dwt_call_sites.txt
```

Search for SPI and platform hooks:

```cmd
findstr /S /I /N /C:"readfromspi" /C:"writetospi" /C:"spi_write" /C:"spi_read" /C:"spi_transceive" /C:"deca_sleep" /C:"deca_usleep" /C:"reset_DWIC" /C:"port_set_dw_ic_spi" /C:"decamutexon" /C:"decamutexoff" *.c *.h > C:\ThinPod\qorvo_platform_hooks.txt 2>nul
notepad C:\ThinPod\qorvo_platform_hooks.txt
```

Search the board/HAL side:

```cmd
findstr /S /I /N /C:"DWM3001CDK" /C:"DW3000" /C:"DWIC" /C:"IRQ" /C:"RESET" /C:"SPI" Src\Boards\*.c Src\Boards\*.h Src\HAL\*.c Src\HAL\*.h > C:\ThinPod\qorvo_board_hal_hooks.txt 2>nul
notepad C:\ThinPod\qorvo_board_hal_hooks.txt
```

## What to extract conceptually

Do not copy the Qorvo stack into the Thin-Pod repo. Extract the platform contract:

```text
Function name:
Purpose:
Called by:
Expected return value:
Blocking or non-blocking:
Uses interrupt or polling:
Zephyr equivalent:
```

Example mapping:

```text
Qorvo-style SPI read/write  -> Zephyr spi_transceive()
Qorvo reset function        -> Zephyr gpio_pin_set_dt()
Qorvo delay                 -> k_msleep() / k_busy_wait()
Qorvo IRQ enable            -> gpio_pin_interrupt_configure_dt()
Qorvo critical section      -> k_mutex or irq_lock/irq_unlock, depending on context
```

## Expected outcome

At the end of this step, there should be a short shim design, not a large port.

The design target is:

```text
dw3110_zephyr_hal.c/.h
    spi read/write
    reset
    delay
    irq attach / flag
    basic logging
```

---

# 6. Build a minimal Zephyr DW3110 bring-up shim

## Aim

Add the smallest Zephyr-native abstraction layer needed by the Stage-2 gateway and node apps.

This shim should initially do only three things:

```text
1. Prove the Zephyr app can control reset and SPI.
2. Read a known device identity or equivalent status from the radio.
3. Provide clean RTT log events for evidence.
```

It should **not** attempt full SS-TWR or vibration payloads yet.

## Suggested repository structure

Prefer a shared local module used by both Stage-2 apps:

```text
firmware/common/dw3110_zephyr/
    dw3110_zephyr_hal.h
    dw3110_zephyr_hal.c
    dw3110_minimal.h
    dw3110_minimal.c
```

Gateway app:

```text
firmware/gateway/uwb_initiator_stage2_rf/
    CMakeLists.txt
    prj.conf
    src/main.c
```

Node app:

```text
firmware/node/uwb_responder_stage2_rf/
    CMakeLists.txt
    prj.conf
    src/main.c
```

If adding shared CMake support becomes awkward, duplicate a tiny local `dw3110_zephyr_hal.c` in each Stage-2 app only temporarily. However, the preferred design is a shared module, because Gateway and node must use the same radio abstraction.

## Devicetree/overlay strategy

If the board DTS exposes a useful SPI node and GPIOs, use those via devicetree.

If not, add app-level overlays:

```text
firmware/gateway/uwb_initiator_stage2_rf/boards/decawave_dwm3001cdk.overlay
firmware/node/uwb_responder_stage2_rf/boards/decawave_dwm3001cdk.overlay
```

The overlay should define only what is needed for the DW3110 radio:

```text
SPI bus
CS GPIO
reset GPIO
IRQ GPIO
```

Do not create fictitious aliases or guessed pins. Ground them in inspected board/vendor material.

## Minimal shim API

Proposed first API:

```c
int dw3110_hal_init(void);
int dw3110_hw_reset(void);
int dw3110_spi_read(uint16_t reg, uint8_t *buf, size_t len);
int dw3110_spi_write(uint16_t reg, const uint8_t *buf, size_t len);
int dw3110_read_devid(uint32_t *devid);
bool dw3110_irq_seen(void);
```

The exact register read format may change once the Qorvo driver access pattern is inspected. The purpose of this skeleton is to make the Zephyr boundary clean.

## Stage-2 banner

Both apps should emit an unambiguous banner:

Gateway:

```text
Thin-Pod Gateway UWB initiator stage-2
gateway_id=0x0000 role=initiator backend=dw3110_zephyr
```

Node:

```text
Thin-Pod Node UWB responder stage-2
node_id=0x0001 role=responder backend=dw3110_zephyr
```

---

# 7. First target: read/confirm DW3000/DW3110 identity or initialise status

## Aim

Before transmitting anything, prove that Zephyr can talk to the radio over SPI.

This is the first hard boundary between Stage-1 and Stage-2:

```text
Stage-1: Zephyr app emits synthetic/logical UWB records
Stage-2: Zephyr app obtains a real response from the DW3110/DW3000 radio
```

## Preferred proof

The cleanest proof is a device ID or equivalent known register:

```text
TPGW_RF_INIT backend=dw3110_zephyr spi=ok reset=ok devid=0x........ status=ok
TPNODE_RF_INIT backend=dw3110_zephyr spi=ok reset=ok devid=0x........ status=ok
```

If the driver exposes a higher-level initialise call, acceptable proof may be:

```text
TPGW_RF_INIT backend=dw3110_zephyr dwt_initialise=ok status=ok
TPNODE_RF_INIT backend=dw3110_zephyr dwt_initialise=ok status=ok
```

## Build commands

Use the already working J-Link runner pattern.

Gateway build:

```cmd
cd /d C:\ncs\v3.3.1

west build -b decawave_dwm3001cdk -p always ^
  -d C:\ncs\v3.3.1\build_gateway_uwb_initiator_stage2_rf ^
  C:\ThinPod\Thin-Pod-Gateway-rev-0.1\firmware\gateway\uwb_initiator_stage2_rf
```

Gateway flash:

```cmd
west flash -d C:\ncs\v3.3.1\build_gateway_uwb_initiator_stage2_rf --runner jlink --dev-id 760203854
```

Node build:

```cmd
cd /d C:\ncs\v3.3.1

west build -b decawave_dwm3001cdk -p always ^
  -d C:\ncs\v3.3.1\build_node_uwb_responder_stage2_rf ^
  C:\ThinPod\Thin-Pod-Gateway-rev-0.1\firmware\node\uwb_responder_stage2_rf
```

Node flash:

```cmd
west flash -d C:\ncs\v3.3.1\build_node_uwb_responder_stage2_rf --runner jlink --dev-id 760222856
```

## Acceptance criteria

Minimum acceptance:

```text
Both boards boot Zephyr Stage-2 firmware.
Both boards identify backend=dw3110_zephyr.
Both boards report reset=ok.
Both boards report spi=ok.
Both boards read a plausible non-zero, non-0xffffffff identity/status value.
```

Failure patterns to interpret:

```text
0x00000000        likely bus, reset or power issue
0xffffffff        likely MISO floating, CS wrong or SPI not talking
timeout           likely IRQ/polling/driver wait path issue
build failure     likely devicetree/CMake/include issue
```

---

# 8. Second target: TX-only frame

## Aim

Once SPI and init are proven, make one board transmit a simple UWB frame. Do not attempt ranging yet.

The aim is to verify:

```text
radio configured
TX buffer written
TX started
TX done event or status observed
```

## Gateway first

The Gateway is the natural first TX-side board:

```text
TPGW_RF_CFG channel=9 prf=... preamble=... sfd=... sts=... status=ok
TPGW_RF_POLL_TX seq=1 src=0x0000 dst=0x0001 len=... status=started
TPGW_RF_TX_DONE seq=1 status=ok
```

At this stage, the node does not have to receive. TX-only is still useful because it proves the Zephyr app can command the radio beyond identity reads.

## PHY settings

Use the previous working RF settings as reference, but do not overfit the first TX-only proof:

```text
Channel 9
DS_TWR_DEFERRED as later ranging reference
SP3
SFD 2
Preamble 10
Static STS IV: 01:02:03:04:05:06
```

For a simple raw TX proof, the decisive requirement is that both sides later use the same settings. If the Qorvo driver requires a standard configuration structure, derive that configuration carefully from the recovered SDK or old notes.

## Acceptance criteria

Minimum acceptance:

```text
Gateway reports RF configuration success.
Gateway writes TX payload successfully.
Gateway starts TX.
Gateway reports TX done or equivalent status.
No hard fault.
No endless driver wait loop.
```

Log example:

```text
TPGW_RF_POLL_TX seq=1 src=0x0000 dst=0x0001 payload=thinpod_stage2_poll status=tx_started
TPGW_RF_TX_DONE seq=1 status=ok
```

---

# 9. Third target: RX frame

## Aim

After TX-only works, make the second board receive a simple frame and log it over RTT.

This is the first true Zephyr-native RF proof.

## Node RX first

Start the node in RX mode:

```text
TPNODE_RF_RX_ARM seq_expected=1 status=ok
```

Then start Gateway TX:

```text
TPGW_RF_POLL_TX seq=1 src=0x0000 dst=0x0001 status=tx_started
TPGW_RF_TX_DONE seq=1 status=ok
```

Expected node evidence:

```text
TPNODE_RF_POLL_RX seq=1 src=0x0000 dst=0x0001 len=... rssi_or_diag=... status=ok
```

Only then add a node response TX and Gateway RX:

```text
TPNODE_RF_RESP_TX seq=1 src=0x0001 dst=0x0000 status=ok
TPGW_RF_RESP_RX seq=1 src=0x0001 dst=0x0000 status=ok
```

## Acceptance criteria

Minimum Stage-2 Zephyr RF proof:

```text
Gateway Zephyr app transmits a real UWB frame.
Node Zephyr app receives that frame.
Node Zephyr app logs the received sequence/source/destination.
Optional: node transmits a response.
Optional: gateway receives the response.
```

Preferred full Stage-2 proof:

```text
Gateway:
TPGW_RF_INIT backend=dw3110_zephyr status=ok
TPGW_RF_POLL_TX seq=1 src=0x0000 dst=0x0001 status=ok
TPGW_RF_RESP_RX seq=1 src=0x0001 dst=0x0000 status=ok

Node:
TPNODE_RF_INIT backend=dw3110_zephyr status=ok
TPNODE_RF_POLL_RX seq=1 src=0x0000 dst=0x0001 status=ok
TPNODE_RF_RESP_TX seq=1 src=0x0001 dst=0x0000 status=ok
```

## Important distinction

This is not yet a ranging result.

The first valid claim should be:

```text
Zephyr-native DW3110 RF TX/RX proof achieved between Gateway and node DWM3001-CDKs.
```

Not:

```text
Thin-Pod ranging validated.
Distance measurement validated.
Predictive-maintenance telemetry over UWB validated.
```

Those come later.

---

# Evidence capture

For each milestone, capture logs into files:

```text
docs/firmware/evidence/stage2_zephyr_gateway_init_rtt.log
docs/firmware/evidence/stage2_zephyr_node_init_rtt.log
docs/firmware/evidence/stage2_zephyr_gateway_tx_rtt.log
docs/firmware/evidence/stage2_zephyr_node_rx_rtt.log
```

Recommended evidence README:

```text
docs/firmware/evidence/Stage2_Zephyr_RF_Evidence_README.md
```

The README should state:

```text
The logs show Zephyr-native firmware running on DWM3001-CDK hardware.
The Qorvo FreeRTOS/UCI firmware was not used for this proof.
The recovered Qorvo SDK 1.1.1 was used only as local reference material.
The RF proof demonstrates TX/RX events, not calibrated distance/ranging.
```

---

# Git discipline

Before editing:

```cmd
cd /d C:\ThinPod\Thin-Pod-Gateway-rev-0.1
git checkout stage2-uwb-rf-proof
git status
```

Suggested commit sequence:

```text
Commit 1:
Document Zephyr DW3110 Stage-2 bring-up plan

Commit 2:
Add shared DW3110 Zephyr HAL skeleton

Commit 3:
Add Stage-2 radio identity read for gateway and node

Commit 4:
Add Stage-2 gateway TX-only proof

Commit 5:
Add Stage-2 node RX proof and evidence logs
```

Do not commit:

```text
Qorvo SDK source dump
Vendor binary blobs
Temporary extracted SDK folders
Build directories
```

If needed, add local ignores:

```gitignore
qorvo_sdk_1_1_1_work/
*.map
*.elf
*.bin
build_*/
```

Only add `.gitignore` entries that make sense for the actual repository layout.

---

# Summary

The correct forward path is:

```text
Inspect Zephyr board mapping
Inspect Qorvo platform contract
Create a tiny Zephyr DW3110 shim
Read device identity or initialise status
Transmit one simple UWB frame
Receive one simple UWB frame
Then add responder reply
Only later attempt TWR/ranging
```

The strongest design principle is to keep the Stage-2 proof narrow and honest. A small verified Zephyr-native RF exchange is more valuable than a large half-ported vendor stack.
