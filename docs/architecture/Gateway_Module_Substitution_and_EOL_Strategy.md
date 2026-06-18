# Gateway Module Substitution and EoL Strategy

## Purpose

This note defines a practical strategy for keeping the Thin-Pod Gateway design resilient if current development hardware becomes unavailable through end-of-life, supply disruption, vendor change, or a future architectural revision.

The immediate concern is the current Gateway development configuration, which uses a Qorvo DWM3001CDK-class UWB board and an STM Nucleo-class analytic supervisor. Both are valuable for bring-up, testing and firmware development, but neither should become a permanent architectural dependency.

The central design principle is:

> Make the interfaces more permanent than the parts.

Components vanish. Development boards vanish. Vendor ecosystems drift. A clean interface, documented adapter pattern, test regime and firmware abstraction can survive several generations of silicon.

## Summary

The Thin-Pod Gateway should not be designed around specific development-board footprints. It should be designed around replaceable roles:

- UWB coprocessor
- Analytic supervisor
- Backhaul or communications module
- Power and protection layer
- Debug and test layer
- Data and firmware interface layer

The current Qorvo CDK and STM Nucleo boards should be treated as reference implementations of those roles, not as the architecture itself.

A robust future Gateway design should therefore move toward:

```text
Gateway baseboard = stable Thin-Pod carrier
Replaceable module = temporary implementation of a role
Adapter PCB = footprint and pinout translator
Firmware abstraction = behavioural translator
```

This allows future modules with different footprints to be integrated through adapter boards rather than forcing a complete Gateway redesign.

## Core architectural rule

The Gateway should not be described as:

> A PCB for this Nucleo and this Qorvo CDK.

It should be described as:

> An open Gateway carrier with documented replaceable module interfaces, where the present Nucleo and Qorvo boards are reference implementations.

That sentence should guide the hardware, firmware, documentation and certification strategy.

## Why fixed development-board footprints are risky

Development boards are excellent during proof-of-concept work, but they make poor long-term anchors.

Risks include:

- Board end-of-life or sudden unavailability
- Header or pinout changes in later vendor revisions
- Dependency on large physical footprints
- Poor fit for enclosure design
- Awkward routing and mechanical constraints
- RF keep-out conflicts
- Fragile assumptions embedded in firmware
- Difficulty moving from prototype to product-like hardware
- OSHWA documentation ambiguity if third-party boards appear to be part of the open design scope

The existing Gateway rev 0.1 board is still valuable. It proves the functional concept and creates a concrete evidence chain. The strategic mistake would be treating the rev 0.1 physical footprint arrangement as the permanent design pattern.

## Preferred pattern: baseboard plus adapter cards

Future Gateway revisions should use a baseboard plus adapter-card architecture.

The baseboard should provide stable power, mounting, test points, connectors, common buses and defined internal interfaces. Specific vendor boards or modules should be connected through small adapter PCBs.

Example adapter types:

```text
UWB adapter cards
- DWM3001CDK adapter
- DWM3001C castellated-module adapter
- Future Qorvo UWB module adapter
- Future third-party UWB coprocessor adapter

Supervisor adapter cards
- STM NUCLEO-N657X0-Q adapter
- Future STM32N6 module adapter
- Future STM Nucleo or Discovery adapter
- Raspberry Pi Compute Module or Linux supervisor adapter
- Other MCU or AI-accelerator adapter

Backhaul adapter cards
- Seeed XIAO ESP32-C6 adapter
- Future Wi-Fi module adapter
- Ethernet adapter
- LTE, LoRaWAN or other communications adapter
```

This prevents the main Gateway PCB from becoming cluttered with multiple incompatible footprints.

## Avoid the universal-footprint trap

An apparent alternative would be to place several possible module footprints directly on the Gateway PCB. This should generally be avoided for large development boards, RF modules and mixed-signal subsystems.

Multiple overlapping footprints can create:

- Antenna keep-out conflicts
- Connector collisions
- Ambiguous silkscreen and assembly markings
- Compromised routing
- Excessive board area
- Unused copper and stubs
- Poor grounding around RF modules
- Harder inspection and debugging
- Higher chance of assembly mistakes

For Thin-Pod, adapter cards are cleaner than universal footprints.

## Proposed stable module interfaces

The Gateway should define stable internal interfaces for each role. The following are proposed starting points.

### UWB coprocessor interface v1

Purpose: connect a UWB subsystem to the Gateway supervisor without assuming a specific vendor footprint.

Minimum signals:

```text
UWB_INTERFACE_V1

Power:
- GND
- 3V3 logic rail
- 5V optional module/input rail

Primary host link:
- SPI_SCK
- SPI_MOSI
- SPI_MISO
- SPI_CS

Control and status:
- UWB_IRQ
- UWB_RESET

Optional:
- UWB_WAKE
- UWB_BOOT
- UWB_SYNC
- UART_TX
- UART_RX
- SWDIO
- SWCLK
- MODULE_ID_0
- MODULE_ID_1
```

Electrical assumptions:

```text
Logic level: 3.3 V preferred
Baseboard supplies: protected 5 V and 3.3 V
Adapter responsibility: local regulation or level shifting where required
Default state: reset and chip-select must have defined idle states
```

Mechanical assumptions:

```text
Adapter card should define its own RF keep-out
Antenna edge placement should be handled by the adapter
Mounting holes should be repeatable across adapter families where practical
Baseboard should avoid copper, metalwork or connectors near the adapter antenna zone
```

### Analytic supervisor interface v1

Purpose: connect the main Gateway processor or analytic supervisor without assuming a fixed Nucleo footprint.

Possible signals:

```text
SUPERVISOR_INTERFACE_V1

Power:
- GND
- 3V3
- 5V

UWB host path:
- SPI_SCK
- SPI_MOSI
- SPI_MISO
- SPI_CS_UWB
- UWB_IRQ
- UWB_RESET

Backhaul path:
- UART_TX
- UART_RX
- SPI_CS_BACKHAUL
- BACKHAUL_INT
- BACKHAUL_RESET

Debug:
- SWDIO
- SWCLK
- UART_CONSOLE_TX
- UART_CONSOLE_RX

General:
- STATUS_LED
- USER_BUTTON
- SPARE_GPIO_0
- SPARE_GPIO_1
- SPARE_GPIO_2
```

This allows an STM Nucleo board, a custom STM32N6 board, a Linux supervisor, or another MCU/AI platform to implement the same Gateway role.

### Backhaul interface v1

Purpose: connect Wi-Fi, Ethernet, cellular, LoRaWAN or other onward communications without binding the Gateway to one module family.

Possible signals:

```text
BACKHAUL_INTERFACE_V1

Power:
- GND
- 3V3
- 5V optional

Host link:
- UART_TX
- UART_RX
- SPI_SCK optional
- SPI_MOSI optional
- SPI_MISO optional
- SPI_CS optional

Control:
- BACKHAUL_RESET
- BACKHAUL_BOOT
- BACKHAUL_INT
- MODULE_PRESENT
```

The reference implementation may use a Seeed XIAO ESP32-C6-class module, but future designs should not depend on that footprint.

## Adapter-card responsibilities

Each adapter card should absorb the messiness of a specific module or development board.

Responsibilities include:

- Vendor footprint translation
- Header translation
- Voltage regulation where required
- Logic-level translation where required
- Boot-mode handling
- Reset polarity handling
- IRQ polarity handling
- Local decoupling
- RF keep-out and antenna placement
- Debug connector access
- Module identification
- Mechanical support
- Silkscreen labelling of module orientation and revision

The baseboard should only depend on the documented Thin-Pod interface.

## Module identification

Future adapter cards should include a simple identification mechanism. This reduces the risk of silently loading the wrong firmware configuration.

Simple options:

```text
Option A: resistor straps read by GPIO or ADC
Option B: solder links defining adapter ID
Option C: small I2C EEPROM containing adapter metadata
```

A minimal adapter identity record could include:

```text
adapter_role: UWB coprocessor
adapter_name: DWM3001C castellated adapter
adapter_revision: rev0.1
logic_voltage: 3.3V
max_spi_clock: defined by test
irq_polarity: active-high or active-low
reset_polarity: active-low or active-high
firmware_compatible: thinpod,uwb-interface-v1
notes: antenna edge keep-out required
```

An EEPROM is not essential for early revisions, but the documentation should anticipate module identification as a desirable future feature.

## Test points and isolation links

Every replaceable module interface should include test access to the key signals.

Recommended test points:

```text
- GND
- 3V3
- 5V where present
- SPI_SCK
- SPI_MOSI
- SPI_MISO
- SPI_CS
- IRQ
- RESET
- UART_TX
- UART_RX
- MODULE_ID signals
```

Recommended isolation or configuration features:

```text
- 0 ohm links on key SPI lines
- 0 ohm links on reset and IRQ lines
- Series resistors for signal integrity tuning
- Current-measurement link on module supply rail
- Pull-up or pull-down footprints for reset, boot and chip-select defaults
```

These features are cheap compared with a failed bring-up cycle. They also make EoL substitution much easier because signals can be isolated, rerouted or characterised without cutting tracks.

## RF-specific guidance

UWB modules should not be treated like ordinary digital modules.

The adapter design should handle:

- Antenna keep-out
- Ground clearance
- Module edge placement
- Orientation
- Shield and enclosure clearance
- Nearby copper exclusion
- Nearby connector exclusion
- External antenna options if applicable

The baseboard should avoid imposing RF compromises on the adapter. In general:

> The UWB adapter owns the antenna environment.

This allows future UWB modules to have different keep-out rules without forcing a full baseboard redesign.

## Power architecture

The baseboard should provide protected, testable rails. The adapter should be allowed to regulate locally.

Suggested rule:

> The baseboard provides protected 5 V and 3.3 V. The adapter decides what the module actually consumes. Logic returning to the baseboard should be 3.3 V unless explicitly level-shifted.

This supports future modules with different requirements, including:

- 3.3 V logic with 5 V input
- 3.3 V logic with local low-noise regulation
- 1.8 V module internals with adapter-level shifting
- Modules requiring special sequencing
- Higher-current backhaul modules

## Firmware abstraction

The hardware abstraction should be mirrored in firmware.

The Gateway firmware should avoid hard-coded assumptions such as:

```text
DWM IRQ is always PB9
DWM RESET is always PD0
The UWB device is always on SPI5
The supervisor is always the NUCLEO-N657X0-Q
```

Instead, the application should refer to logical devices:

```text
uwb0
gateway_spi
uwb_irq
uwb_reset
backhaul0
status_led0
```

Each hardware configuration should map those logical names to real pins through board descriptions, overlays or configuration files.

For Zephyr-based firmware, this points toward:

- Devicetree overlays for each adapter combination
- Stable compatible strings
- Board-specific pin mapping outside application logic
- Driver or transport abstraction for UWB communication
- A small hardware self-test at boot
- Runtime or build-time selection of Gateway configuration

Example structure:

```text
firmware/
  zephyr/
    boards/
    overlays/
      gateway_baseboard_dwm3001cdk_nucleo.overlay
      gateway_baseboard_dwm3001c_nucleo.overlay
      gateway_baseboard_dwm3001c_custom_stm32n6.overlay
    src/
      app_main.c
      uwb_transport.c
      gateway_dsp.c
      backhaul_transport.c
```

Changing modules should mostly require a new adapter board and overlay, not a rewrite of the application.

## Repository structure

A future Gateway repository could organise the work as follows:

```text
hardware/
  gateway_baseboard/
    kicad/
    gerbers/
    fabrication/
    assembly/
  adapters/
    uwb_dwm3001cdk_adapter/
    uwb_dwm3001c_castellated_adapter/
    supervisor_nucleo_n657x0_q_adapter/
    backhaul_xiao_esp32c6_adapter/

docs/
  architecture/
    Gateway_Module_Substitution_and_EOL_Strategy.md
  interfaces/
    ThinPod_Gateway_Module_Interface_v1.md
    UWB_Coprocessor_Interface_v1.md
    Supervisor_Interface_v1.md
    Backhaul_Interface_v1.md
  lifecycle/
    EOL_and_Module_Substitution_Policy.md
  bringup/
    Gateway_Baseboard_Bringup_Checklist.md
    Adapter_Bringup_Checklist.md

firmware/
  zephyr/
    overlays/
    src/
    tests/
```

The exact folders can change, but the separation should remain clear:

```text
Baseboard design
Adapter designs
Interface documentation
Lifecycle policy
Firmware mappings
Bring-up and validation evidence
```

## Module substitution matrix

The project should maintain a substitution matrix so that EoL risk is visible and controlled.

Example:

```text
Role: UWB coprocessor
Current implementation: DWM3001CDK
Preferred productised implementation: DWM3001C castellated module adapter
Substitution requirement: TWR-capable UWB subsystem, host interface, reset, IRQ, documented timing behaviour
Firmware impact: UWB transport or driver layer only
Hardware impact: adapter card only
Certification impact: Gateway revision note required
```

```text
Role: Analytic supervisor
Current implementation: STM NUCLEO-N657X0-Q
Preferred productised implementation: custom STM32N6-class module or other documented supervisor adapter
Substitution requirement: sufficient DSP/TinyML capability, UWB host link, backhaul link, logging/export support
Firmware impact: board overlay and hardware abstraction layer
Hardware impact: supervisor adapter only
Certification impact: Gateway revision note required
```

```text
Role: Backhaul module
Current implementation: XIAO ESP32-C6-class module
Preferred productised implementation: replaceable communications adapter
Substitution requirement: documented host interface, reset, boot, status/interrupt, power budget
Firmware impact: backhaul transport layer only
Hardware impact: backhaul adapter only
Certification impact: normally adapter-level documentation update
```

## OSHWA and open-hardware boundary notes

The modular strategy supports clearer OSHWA documentation.

The open hardware claim should apply to the Thin-Pod Gateway baseboard and any open adapter PCBs designed by the project. Third-party modules and development boards should be documented as external dependencies or reference modules, not as open hardware created by the project.

Recommended wording:

> The Thin-Pod Gateway baseboard and project adapter PCBs are open hardware. Third-party development boards and radio modules are integrated through documented replaceable interfaces and remain under their respective vendor terms.

This avoids overstating the open scope while still documenting how third-party components are integrated.

For certification, each hardware release should clearly state:

- What is included in the certified/open hardware scope
- What is a third-party module
- What is a reference implementation only
- What interfaces allow substitution
- What validation evidence exists for each supported configuration

## Suggested revision path

### Gateway rev 0.1

Purpose:

- Prove the current Nucleo + Qorvo CDK + XIAO concept
- Validate power, grounding, SPI, reset, IRQ and physical bring-up
- Establish engineering evidence

Status:

- Current manufactured PCB remains valuable as proof-of-concept hardware
- Existing pin mapping and bring-up logs should be preserved

### Gateway rev 0.2

Purpose:

- Preserve the same functional architecture
- Add formal interface documentation
- Improve test-point coverage
- Add 0 ohm isolation links
- Document module substitution assumptions
- Consider first DWM3001C adapter route

Expected outcome:

- Less dependence on specific dev-board assumptions
- Better OSHWA and repo clarity
- Easier firmware abstraction

### Gateway rev 0.3

Purpose:

- Move toward baseboard plus adapter-card architecture
- Stop treating the Nucleo and Qorvo CDK footprints as permanent
- Support at least one alternative UWB adapter or supervisor adapter

Expected outcome:

- EoL resilience becomes a demonstrated property, not only a design intention

### Gateway rev 1.0

Purpose:

- OSHWA-certifiable open Gateway carrier
- Documented adapter ecosystem
- At least two working hardware configurations if practical
- Firmware overlays proving substitution works
- Clear boundary between open baseboard, third-party modules and optional higher-level software

Expected outcome:

- The Gateway becomes an open, modular measurement platform rather than a one-off carrier for specific dev boards

## Design doctrine

The Gateway should follow the same modular design ethos as the wider Thin-Pod project:

```text
Sensing layer
UWB transport layer
Gateway supervision layer
DSP and TinyML layer
Backhaul and reporting layer
Validation and evidence layer
```

Each layer should have documented boundaries and replaceable implementations.

The practical doctrine is:

> Footprints are temporary. Interfaces are strategic.

