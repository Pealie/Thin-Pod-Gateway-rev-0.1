# Thin-Pod Gateway Stage-2: NUCLEO SPI5/GPIO Probe App Commit Plan

## Purpose

This note defines the next modest code commit for the `stage2-uwb-rf-proof` branch of the Thin-Pod Gateway rev 0.1 repository.

The next commit should add:

```text
1. A Gateway-specific Zephyr devicetree overlay for the manufactured Gateway PCB.
2. A minimal NUCLEO-side SPI5/GPIO probe app.
```

It should **not** yet attempt a DW3110/DW3000 driver port, UWB ranging, SS-TWR, or vibration telemetry.

The purpose is to prove that Zephyr can build against the STM32 NUCLEO-N657X0-Q Gateway host interface and that the manufactured PCB mapping is represented cleanly in devicetree.

---

## Scope of this commit

### Include

```text
Gateway-specific overlay for nucleo_n657x0_q
Minimal probe firmware app
Buildable Zephyr project structure
RTT/UART console prints confirming device readiness
GPIO configuration for DWM_RESET and DWM_IRQ
SPI5 devicetree binding/probe
Documentation comments tying the overlay to the manufactured Gateway PCB
```

### Exclude

```text
Qorvo SDK source
Vendor FreeRTOS firmware
DW3000/DW3110 driver port
UCI/PCTT implementation
Actual RF TX/RX
TWR/ranging
Vibration-window payloads
```

This commit is a **host-interface foundation commit**, not a radio-functionality commit.

---

## Repository location

Suggested new app path:

```text
firmware/gateway/nucleo_spi5_gpio_probe/
```

Suggested files:

```text
firmware/gateway/nucleo_spi5_gpio_probe/CMakeLists.txt
firmware/gateway/nucleo_spi5_gpio_probe/prj.conf
firmware/gateway/nucleo_spi5_gpio_probe/boards/nucleo_n657x0_q.overlay
firmware/gateway/nucleo_spi5_gpio_probe/src/main.c
firmware/gateway/nucleo_spi5_gpio_probe/README.md
```

This keeps the probe separate from the later Stage-2 RF app. It can be deleted, archived, or retained as manufacturing/bring-up evidence once the proper RF path works.

---

## Grounded manufactured Gateway PCB mapping

The manufactured Gateway PCB maps the NUCLEO-N657X0-Q to the DWM3001-CDK as follows:

| Gateway net | NUCLEO pin | DWM3001-CDK connection | Intended Zephyr use |
|---|---:|---|---|
| `SPI5_SCK` | `PE15` | `J10.23 / SPI1_CLK` | SPI5 SCK |
| `SPI5_MISO` | `PG1` | `J10.21 / SPI1_MISO` | SPI5 MISO |
| `SPI5_MOSI` | `PG2` | `J10.19 / SPI1_MOSI` | SPI5 MOSI |
| `DWM_CS` | `PA3` | `J10.24 / CS_RPI` | GPIO-controlled SPI CS |
| `DWM_IRQ` | `PB9` | `J10.15 / GPIO_RPI` | GPIO input |
| `DWM_RESET` | `PD0` | `J10.12 / RESET` | GPIO output |

The NUCLEO Zephyr board support already maps SPI5 to:

```text
SPI5_NSS   PA3
SPI5_SCK   PE15
SPI5_MISO  PG1
SPI5_MOSI  PG2
```

For Gateway bring-up, `PA3` should be used as software-controlled chip select through `cs-gpios`, rather than relying on hardware NSS.

The default NUCLEO board support also enables `fdcan1` on `PD0`, so the overlay must disable `fdcan1` before `PD0` can be used as `DWM_RESET`.

---

## Overlay intent

The overlay should:

```text
Disable fdcan1 because it claims PD0.
Remove zephyr,canbus from chosen.
Keep SPI5 enabled.
Use SPI5 SCK/MISO/MOSI on PE15/PG1/PG2.
Use PA3 as GPIO chip select.
Expose PB9 as DWM_IRQ.
Expose PD0 as DWM_RESET.
Add a lightweight local node for the DWM3001-CDK host interface.
```

The local node should be named as a host-interface target, not as a direct DW3110 device, because the architecture still treats the DWM3001-CDK as a UWB subsystem.

Preferred node naming:

```text
dwm3001_host
```

Avoid naming it:

```text
dw3110
dw3000
uwb_radio
```

until the NUCLEO-side protocol and RF control boundary are proven.

---

## Draft overlay

File:

```text
firmware/gateway/nucleo_spi5_gpio_probe/boards/nucleo_n657x0_q.overlay
```

Draft content:

```dts
/*
 * Thin-Pod Gateway rev 0.1
 * NUCLEO-N657X0-Q to DWM3001-CDK host-interface probe.
 *
 * Manufactured Gateway PCB mapping:
 *   SPI5_SCK  PE15 -> DWM3001-CDK J10.23 / SPI1_CLK
 *   SPI5_MISO PG1  -> DWM3001-CDK J10.21 / SPI1_MISO
 *   SPI5_MOSI PG2  -> DWM3001-CDK J10.19 / SPI1_MOSI
 *   DWM_CS    PA3  -> DWM3001-CDK J10.24 / CS_RPI
 *   DWM_IRQ   PB9  -> DWM3001-CDK J10.15 / GPIO_RPI
 *   DWM_RESET PD0  -> DWM3001-CDK J10.12 / RESET
 */

/ {
    chosen {
        /delete-property/ zephyr,canbus;
    };
};

&fdcan1 {
    status = "disabled";
};

&spi5 {
    status = "okay";

    /*
     * The NUCLEO board support already assigns:
     *   spi5_sck_pe15
     *   spi5_miso_pg1
     *   spi5_mosi_pg2
     *
     * PA3 is treated as GPIO chip-select for bring-up rather than
     * relying on hardware NSS.
     */
    cs-gpios = <&gpioa 3 GPIO_ACTIVE_LOW>;

    dwm3001_host: dwm3001-host@0 {
        compatible = "thinpod,dwm3001-host";
        reg = <0>;
        spi-max-frequency = <1000000>;
        irq-gpios = <&gpiob 9 GPIO_ACTIVE_HIGH>;
        reset-gpios = <&gpiod 0 GPIO_ACTIVE_LOW>;
    };
};
```

### Note on pinctrl override

The first probe commit may leave the default `spi5_nss_pa3` pinctrl in place if the build accepts it and software CS still behaves as expected.

However, if PA3 cannot be controlled cleanly as `cs-gpios`, the overlay should override SPI5 pinctrl to remove `spi5_nss_pa3` from the default pin group.

That refinement should be made only after inspecting the generated DTS and/or observing actual GPIO behaviour.

---

## Minimal application behaviour

The probe app should do only this:

```text
Boot.
Print a banner.
Confirm SPI5 device is ready.
Confirm DWM_RESET GPIO device is ready.
Confirm DWM_IRQ GPIO device is ready.
Configure DWM_RESET as output.
Configure DWM_IRQ as input.
Pulse or set DWM_RESET in a conservative way.
Read DWM_IRQ level.
Print results.
Exit to a slow heartbeat loop.
```

Expected log style:

```text
Thin-Pod Gateway NUCLEO SPI5/GPIO probe
board=nucleo_n657x0_q
target=manufactured_gateway_rev0.1
backend=spi5_gpio_probe
spi5=ready
dwm_reset=pd0 ready=1 configured=1
dwm_irq=pb9 ready=1 configured=1 level=0
dwm_cs=pa3 via cs-gpios
status=ok
```

No SPI transfer to the DWM3001-CDK is required in this first version. The first code commit is only about buildability, devicetree correctness and GPIO/SPI device readiness.

---

## Suggested `prj.conf`

File:

```text
firmware/gateway/nucleo_spi5_gpio_probe/prj.conf
```

Suggested content:

```conf
CONFIG_GPIO=y
CONFIG_SPI=y
CONFIG_SERIAL=y
CONFIG_CONSOLE=y
CONFIG_UART_CONSOLE=y
CONFIG_PRINTK=y
CONFIG_LOG=y
CONFIG_LOG_DEFAULT_LEVEL=3
```

Keep this minimal. Do not enable networking, CAN, Bluetooth, UWB, shell, USB, or mcumgr unless specifically needed.

---

## Suggested `CMakeLists.txt`

File:

```text
firmware/gateway/nucleo_spi5_gpio_probe/CMakeLists.txt
```

Suggested content:

```cmake
cmake_minimum_required(VERSION 3.20.0)

find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(thinpod_gateway_nucleo_spi5_gpio_probe)

target_sources(app PRIVATE src/main.c)
```

---

## Suggested `main.c`

File:

```text
firmware/gateway/nucleo_spi5_gpio_probe/src/main.c
```

Draft structure:

```c
#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/spi.h>
#include <zephyr/sys/printk.h>

#define DWM_NODE DT_NODELABEL(dwm3001_host)

#if !DT_NODE_EXISTS(DWM_NODE)
#error "dwm3001_host node is not defined in the board overlay"
#endif

static const struct spi_dt_spec dwm_spi =
    SPI_DT_SPEC_GET(DWM_NODE, SPI_WORD_SET(8) | SPI_TRANSFER_MSB, 0);

static const struct gpio_dt_spec dwm_irq =
    GPIO_DT_SPEC_GET(DWM_NODE, irq_gpios);

static const struct gpio_dt_spec dwm_reset =
    GPIO_DT_SPEC_GET(DWM_NODE, reset_gpios);

int main(void)
{
    int ret;
    int irq_level;

    printk("Thin-Pod Gateway NUCLEO SPI5/GPIO probe\n");
    printk("board=nucleo_n657x0_q\n");
    printk("target=manufactured_gateway_rev0.1\n");
    printk("backend=spi5_gpio_probe\n");

    if (!spi_is_ready_dt(&dwm_spi)) {
        printk("spi5=not_ready status=fail\n");
        return 0;
    }

    printk("spi5=ready\n");

    if (!gpio_is_ready_dt(&dwm_reset)) {
        printk("dwm_reset=not_ready status=fail\n");
        return 0;
    }

    if (!gpio_is_ready_dt(&dwm_irq)) {
        printk("dwm_irq=not_ready status=fail\n");
        return 0;
    }

    ret = gpio_pin_configure_dt(&dwm_reset, GPIO_OUTPUT_INACTIVE);
    if (ret != 0) {
        printk("dwm_reset=config_failed ret=%d status=fail\n", ret);
        return 0;
    }

    ret = gpio_pin_configure_dt(&dwm_irq, GPIO_INPUT);
    if (ret != 0) {
        printk("dwm_irq=config_failed ret=%d status=fail\n", ret);
        return 0;
    }

    /*
     * Conservative reset exercise.
     * Polarity may need confirmation against the DWM3001-CDK RESET input.
     * For now this proves GPIO ownership and toggling, not radio reset timing.
     */
    gpio_pin_set_dt(&dwm_reset, 1);
    k_msleep(10);
    gpio_pin_set_dt(&dwm_reset, 0);
    k_msleep(10);

    irq_level = gpio_pin_get_dt(&dwm_irq);

    printk("dwm_reset=pd0 ready=1 configured=1\n");
    printk("dwm_irq=pb9 ready=1 configured=1 level=%d\n", irq_level);
    printk("dwm_cs=pa3 via=cs-gpios\n");
    printk("status=ok\n");

    while (1) {
        printk("probe_heartbeat=1\n");
        k_sleep(K_SECONDS(5));
    }

    return 0;
}
```

This code intentionally avoids SPI transactions. The next increment can attempt a harmless SPI transaction or protocol-level host command once the DWM3001-CDK-side expectations are understood.

---

## Suggested README

File:

```text
firmware/gateway/nucleo_spi5_gpio_probe/README.md
```

Suggested content:

```markdown
# NUCLEO SPI5/GPIO Probe

This minimal Zephyr application validates the Thin-Pod Gateway rev 0.1 manufactured PCB host-interface mapping between the STM32 NUCLEO-N657X0-Q and the DWM3001-CDK.

It verifies that Zephyr can see:

- SPI5 on the NUCLEO Gateway route
- PA3 as DWM chip select through `cs-gpios`
- PB9 as DWM IRQ/status input
- PD0 as DWM reset output after disabling default FDCAN1 use

This app does not initialise the DW3110/DW3000 radio and does not perform UWB RF TX/RX.
```

---

## Build command

Use the board target identified in the installed Zephyr tree:

```cmd
cd /d C:\ncs\v3.3.1

west build -b nucleo_n657x0_q -p always ^
  -d C:\ncs\v3.3.1\build_gateway_nucleo_spi5_gpio_probe ^
  C:\ThinPod\Thin-Pod-Gateway-rev-0.1\firmware\gateway\nucleo_spi5_gpio_probe
```

If `west` is not on PATH in a normal `cmd.exe` window, use the same nRF Connect SDK command prompt or environment that successfully built Stage-1.

Do not spend time debugging `west` from an uninitialised shell unless necessary.

---

## Generated DTS inspection

After a successful build, inspect the generated DTS before flashing:

```cmd
dir /s /b C:\ncs\v3.3.1\build_gateway_nucleo_spi5_gpio_probe\zephyr.dts
dir /s /b C:\ncs\v3.3.1\build_gateway_nucleo_spi5_gpio_probe\devicetree_generated.h
```

Then search the build output:

```cmd
findstr /S /I /N "dwm3001_host spi5 gpioa gpiob gpiod fdcan1 canbus pa3 pb9 pd0" C:\ncs\v3.3.1\build_gateway_nucleo_spi5_gpio_probe\*.dts C:\ncs\v3.3.1\build_gateway_nucleo_spi5_gpio_probe\*.h 2>nul
```

Acceptance criteria:

```text
dwm3001_host exists.
spi5 is okay.
fdcan1 is disabled.
zephyr,canbus is absent or no longer selects fdcan1.
PA3 appears as cs-gpios.
PB9 appears as irq-gpios.
PD0 appears as reset-gpios.
```

---

## Flashing

Only flash after generated DTS inspection is sane.

The exact flashing command may depend on the STM32/NCS runner configuration for `nucleo_n657x0_q`.

Start with:

```cmd
west flash -d C:\ncs\v3.3.1\build_gateway_nucleo_spi5_gpio_probe
```

If runner/debug-probe issues appear, resolve them as a separate tooling task. Do not confuse flashing issues with devicetree validity.

---

## Commit sequence

From the repository root:

```cmd
cd /d C:\ThinPod\Thin-Pod-Gateway-rev-0.1

git checkout stage2-uwb-rf-proof
git status
```

Create files:

```text
firmware/gateway/nucleo_spi5_gpio_probe/CMakeLists.txt
firmware/gateway/nucleo_spi5_gpio_probe/prj.conf
firmware/gateway/nucleo_spi5_gpio_probe/boards/nucleo_n657x0_q.overlay
firmware/gateway/nucleo_spi5_gpio_probe/src/main.c
firmware/gateway/nucleo_spi5_gpio_probe/README.md
```

Then:

```cmd
git add firmware\gateway\nucleo_spi5_gpio_probe

git diff --cached --check

git commit -m "Add NUCLEO SPI5 GPIO probe app"

git push

git status
```

---

## Valid claim after this commit

If the app builds and generated DTS is correct, the valid claim is:

```text
A Zephyr NUCLEO-N657X0-Q probe app and Gateway-specific overlay now represent the manufactured Gateway PCB SPI5/GPIO host-interface mapping for the DWM3001-CDK connection.
```

If the app also flashes and prints the expected log, the valid claim becomes:

```text
The NUCLEO-side Zephyr firmware can claim and configure the manufactured Gateway PCB host-interface pins for SPI5, DWM_CS, DWM_IRQ and DWM_RESET.
```

Do not yet claim:

```text
The DW3110/DW3000 radio has been initialised.
The DWM3001-CDK host protocol has been verified.
UWB RF has been transmitted or received.
Ranging has been demonstrated.
```

---

## Next step after this commit

The next increment should be either:

```text
A harmless SPI transaction probe, if the DWM3001-CDK host-side protocol is known.
```

or:

```text
A DWM3001-CDK-side Zephyr responder/host-protocol stub, if the NUCLEO is expected to talk to firmware running on the CDK nRF52833 rather than directly to the DW3110/DW3000 radio.
```

The correct choice depends on whether the Gateway architecture intends the NUCLEO to command the DWM3001-CDK subsystem as a host peripheral or directly control the DW3110/DW3000 radio path.
