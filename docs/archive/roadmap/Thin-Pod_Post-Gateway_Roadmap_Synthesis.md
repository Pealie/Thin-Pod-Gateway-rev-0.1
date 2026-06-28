# Thin-Pod Post-Gateway Roadmap: Bounded Claims, Modular Architecture and EoL Strategy

**Date:** 18 June 2026  
**Status:** Working synthesis  
**Scope:** Thin-Pod node, Thin-Pod Gateway, future system repository, open hardware certification pathway, documentation strategy and evidence-led development.

## 1\. Purpose

This document synthesises the current Thin-Pod direction after completion of the certified node repository and ongoing Gateway work. It integrates the post-Gateway roadmap, the recent discussion around network effects and rent-like platform dynamics, the Gateway module-substitution / EoL strategy, and the need to keep all claims bounded, evidence-led and reviewable.

The central position is deliberately modest:

**Thin-Pod should become a documented, modular, inspectable open-hardware reference architecture for acquiring, transporting and exposing vibration-window data. It should not claim to be a completed predictive-maintenance product, validated industrial diagnostic system or proprietary platform.**

That distinction matters. The project becomes stronger by being more specific, more evidenced and easier to review.

## 2\. Bounded technical summary

Thin-Pod’s strongest current claim is not that it has solved predictive maintenance. It has not.

The bounded technical claim is that the project is building a traceable open hardware and firmware path from physical vibration sensing to Gateway-side data exposure, with enough architectural discipline to support later DSP, TinyML, validation and reporting layers.

A concise formulation would be:

> Thin-Pod provides open hardware and reference firmware needed to acquire, transport and expose vibration-window data. Downstream DSP, TinyML models and diagnostic interpretations remain deliberately separate user-developed layers above the documented interface.

This framing keeps the project serious, specific and reviewable. It also protects the OSHWA certification boundary: hardware can be open, documented and certified without implying that every downstream analytic or diagnostic claim has already been validated.

## 3\. Claim discipline

The project should avoid language suggesting that Thin-Pod is already any of the following:

* a completed industrial predictive-maintenance platform
* a validated diagnostic product
* a certified wireless maintenance system
* a proven TinyML fault-classification system
* a replacement for commercial condition-monitoring equipment
* an independently replicated research platform
* a deployed industrial safety-critical monitoring system

At the current stage, the stronger and more defensible language is:

* open hardware vibration-sensor node
* Gateway carrier / analytic-supervisor architecture
* UWB-linked condition-monitoring experiment
* documented hardware and firmware bring-up path
* modular reference architecture
* evidence chain for later validation
* reproducible bench demonstration target
* open interface for downstream DSP and TinyML work

The test is simple: a sceptical engineer should be able to ask, ‘What was actually done?’ and receive a concrete answer based on repository evidence.

## 4\. Current architectural direction

Thin-Pod should be treated as a layered system rather than a single monolithic product.

```text
Layer 1: Open hardware node
  - vibration sensor carrier PCB
  - power path
  - accelerometer signal path
  - ADC interface
  - build notes, BOM, fabrication package and certification boundary

Layer 2: Open hardware Gateway carrier
  - Gateway PCB
  - NUCLEO / UWB / backhaul integration
  - bring-up and verification firmware
  - pin mapping, test points, SPI/GPIO probe evidence
  - module-substitution and EoL strategy

Layer 3: System architecture
  - packet contracts
  - data formats
  - firmware roles
  - Gateway processing flow
  - test rigs
  - evidence chain
  - sample logs and validation protocols

Layer 4: Downstream analysis
  - DSP feature extraction
  - TinyML examples
  - anomaly scoring
  - maintenance-reporting caveats
  - research / workshop / publication outputs
```

This separation is one of the project’s most important strengths. It allows the hardware to be open and certifiable while keeping analytical claims staged, testable and appropriately caveated.

## 5\. Post-Gateway roadmap

The post-Gateway roadmap should proceed in six evidence-led phases.

### Phase 1: freeze the two open hardware artefacts

The node repository should remain a bounded certified hardware artefact. The Gateway repository should reach a similar standard once its scope is ready.

Each artefact should have:

* clean certification scope
* appropriate hardware and documentation licences
* BOM
* fabrication package
* build notes
* third-party component treatment
* validation record
* known limitations
* revision history
* clear boundary between open project files and third-party modules

For the node, this has already been substantially achieved through the OSHWA-certified Thin-Pod rev 0.1 repository. For the Gateway, the equivalent task is still in progress.

### Phase 2: create the system repository

After the Gateway repo reaches certification-grade completeness, create a separate `Thin-Pod-System` repository.

This is where the architecture should live. It should not blur the hardware certification boundary of the node or Gateway. Instead, it should collect:

* system diagrams
* interface contracts
* firmware roles
* Gateway processing flow
* packet definitions
* vibration-window data format
* parser tools
* example logs
* validation protocols
* test-rig descriptions
* DSP/TinyML examples
* reporting caveats
* evidence-chain documentation

This repository becomes the bridge between certified hardware artefacts and future analytic work.

### Phase 3: produce the first end-to-end demo

The first end-to-end demo should be intentionally modest.

Not a polished commercial demo. Not a predictive-maintenance claim. Just a complete evidence path:

```text
motor
  -> mounted Thin-Pod node
  -> captured vibration window
  -> UWB transport
  -> Gateway reception
  -> basic processing
  -> saved log/report
```

The output could be a simple file, not a dashboard. The value is in proving that the chain exists and is inspectable.

A suitable first target:

> A motor, a mounted node, a Gateway, a captured vibration window, a processed result and a saved report.

The result should be described as a bench demonstration, not as industrial validation.

### Phase 4: publish the engineering story

Once the chain exists, publish the story in forms appropriate to the evidence level:

* Hackster article
* GitHub Pages site
* technical white paper
* OSHWA / Open Hardware Summit-style proposal
* later formal paper if the evidence base grows

The publication strategy should emphasise reproducibility, open hardware boundaries, modular architecture and practical lessons learned. It should avoid inflated claims around predictive maintenance until externally tested evidence exists.

### Phase 5: test with outsiders

Before rev 0.2, workshops or formal publication, ask one or two trusted technical users to inspect, build, follow or critique the repos.

Their confusion becomes documentation debt.

Their success becomes replication evidence.

This is a crucial transition: the work moves from ‘Neil can make it work’ to ‘someone else can understand, build or review it from the repository’.

The test should not be whether they praise it. The useful test is where they get stuck.

### Phase 6: decide the next move from evidence

After outsider feedback and a first end-to-end demo, decide whether the next move should be:

* rev 0.2 hardware
* Gateway rev 0.2
* system repo expansion
* workshop
* article
* paper
* small kit
* limited collaboration
* further test equipment
* controlled field-style experiment

The decision should be evidence-led. That means the next step follows from observed documentation gaps, build problems, technical risks, replication results and demo limitations.

## 6\. Proposed repository structure

The clean post-Gateway structure is:

```text
Thin-Pod-rev0.1
  certified open hardware node PCB

Thin-Pod-Gateway-rev0.1
  certified open hardware Gateway carrier PCB
  reference bring-up and verification firmware

Thin-Pod-System
  architecture
  packet contract
  sample data
  parser tools
  DSP/TinyML examples
  validation protocols
  evidence chain
```

This separation prevents the certified hardware repos from becoming overloaded with system-level claims. It also gives the system architecture a proper home.

## 7\. Network effects: risk and opportunity

The recent Monopoly / tollgate discussion maps neatly onto technical ecosystem design.

The danger is not network effects themselves. The danger is enclosed network effects: systems that become more useful as they grow, then use that growth to trap participants through lock-in, switching costs, opaque rules or proprietary choke points.

Thin-Pod should aim for the opposite pattern.

The desirable form is an open network effect:

* more documentation makes the next build easier
* more test logs make the evidence chain stronger
* more interface clarity makes substitution easier
* more sample data makes analysis more useful
* more replication attempts make the project more credible
* more modularity makes future revisions less fragile
* more OSHWA-grade boundary discipline makes the project easier to trust

The aim is not to create a tollgate. The aim is to create a reference point.

A closed platform says:

> Everyone must pass through this square.

A good open reference architecture says:

> Here is a square others can inspect, copy, improve, route around or build beside.

That is the kind of network effect Thin-Pod should cultivate.

## 8\. Practical measures to avoid harmful network effects

Thin-Pod should avoid becoming dependent on a single vendor, module, toolchain, cloud service, undocumented firmware blob, private dataset or personal memory.

The mitigation principles are:

### 8.1 Interoperability

Define interfaces clearly enough that future modules, firmware stacks and analysis tools can connect without reverse-engineering the original developer’s assumptions.

### 8.2 Portability

Use plain, exportable formats wherever possible: Markdown, CSV, JSON, KiCad project files, documented packet structures and local logs.

### 8.3 Multi-homing

Avoid architectural decisions that force the project into one vendor path. A future Gateway should be able to tolerate different UWB modules, supervisors and backhaul options.

### 8.4 Layer separation

Keep sensing, UWB transport, Gateway processing, DSP/TinyML and reporting as separable layers with stable documented boundaries.

### 8.5 Open reference implementations

Where possible, provide minimal working examples. A standard is stronger when there is a small reference implementation that proves the intended use.

### 8.6 Governance before scale

Before broader use, document licensing, certification boundaries, compatibility language, naming, contribution expectations and what is not being claimed.

## 9\. Beneficial network effects for Thin-Pod

Thin-Pod can deliberately cultivate several positive network effects.

### 9.1 Documentation network effect

Each clear bring-up note, test point, schematic explanation, failure record and correction reduces friction for the next builder.

### 9.2 Compatibility network effect

Stable interfaces allow alternate modules, different Gateways and future sensor boards to connect to the same system architecture.

### 9.3 Dataset network effect

Comparable vibration-window records become more useful as they accumulate, especially if metadata and test conditions are consistent.

### 9.4 Benchmark network effect

Shared bench tests allow different hardware and firmware revisions to be compared.

### 9.5 Credibility network effect

OSHWA certification, engineering logs, test evidence and clean repo structure compound into a public record that is harder to dismiss.

### 9.6 Teaching network effect

The project is well suited to articles, workshops and demonstrations because it intersects open hardware, vibration sensing, UWB, embedded firmware, DSP, TinyML and maintenance engineering.

## 10\. Gateway module-substitution and EoL strategy

The Gateway should not be designed around the assumption that the present Qorvo CDK or STM Nucleo will remain available indefinitely.

The architecture should instead treat those boards as current reference implementations of replaceable roles:

```text
UWB coprocessor role
  current: Qorvo DWM3001CDK / DWM3001C ecosystem
  future: alternate UWB modules or adapter cards

Analytic supervisor role
  current: STM NUCLEO-N657X0-Q
  future: custom STM32N6 module, alternate MCU board, Linux supervisor or AI module

Backhaul role
  current: XIAO ESP32-C6 / auxiliary comms path
  future: Ethernet, Wi-Fi, MQTT gateway, USB, serial or other backhaul module
```

The key design rule is:

**Make the interfaces more permanent than the parts.**

Components vanish. Dev boards go EoL. Vendor ecosystems drift. But a stable interface, documented adapter pattern and firmware abstraction can survive several generations of hardware.

## 11\. Baseboard plus adapter-card approach

The clean future architecture is:

```text
Gateway baseboard
  stable power, mounting, protection, debug and inter-module interfaces

UWB adapter card
  maps a specific UWB module or dev board to the Thin-Pod UWB interface

Supervisor adapter card
  maps a specific MCU / Nucleo / compute module to the Thin-Pod supervisor interface

Backhaul adapter card
  maps Wi-Fi, Ethernet, USB or other communications hardware to the baseboard
```

This is preferable to placing many incompatible footprints directly onto one PCB.

Overlapping universal footprints can create antenna conflicts, confusing assembly options, compromised routing and long-term maintenance problems. Adapter cards preserve modularity without turning the main board into a cluttered graveyard of possible futures.

## 12\. Thin-Pod Gateway Module Interface v1

A future interface document should define the stable contract for replaceable modules.

Example:

```text
UWB\_INTERFACE\_V1

Supply:
  3.3 V preferred
  5 V optional where adapter regulates locally

Logic:
  3.3 V unless explicitly level-shifted

Required host link:
  SPI or UART, depending on module class

Required signals:
  SCK
  MOSI
  MISO
  CS
  IRQ
  RESET
  GND
  3V3

Optional signals:
  WAKE
  BOOT
  SYNC
  UART\_TX
  UART\_RX
  SWDIO
  SWCLK

Mechanical:
  adapter-card outline
  mounting holes
  antenna keep-out
  connector orientation

Identification:
  resistor strap or I2C EEPROM for module ID

Firmware:
  compatible string
  reset polarity
  IRQ polarity
  maximum bus speed
  signal mapping
```

This should eventually become a repo document under something like:

```text
docs/interfaces/ThinPod\_Gateway\_Module\_Interface\_v1.md
docs/interfaces/UWB\_Coprocessor\_Interface\_v1.md
docs/interfaces/Supervisor\_Interface\_v1.md
docs/interfaces/Backhaul\_Interface\_v1.md
```

## 13\. Firmware abstraction

The firmware should not hard-code the present manufactured PCB as if it were the only possible future.

For Zephyr work, the application should refer to logical roles:

```text
uwb0
gateway\_spi
uwb\_irq
uwb\_reset
backhaul0
status\_led0
```

Board overlays and devicetree mappings should translate those roles into actual pins for each hardware configuration.

This keeps the application logic portable across Gateway revisions.

A future firmware tree might include:

```text
firmware/
  zephyr/
    boards/
    overlays/
      gateway\_baseboard\_dwm3001cdk\_nucleo.overlay
      gateway\_baseboard\_dwm3001c\_nucleo.overlay
      gateway\_baseboard\_dwm3001c\_custom\_stm32n6.overlay
```

## 14\. Evidence-led development path from the current branch

The current Gateway branch now has a useful documentation sequence:

```text
b1665fe  Add Stage-2 UWB RF proof notes
5db2d66  Document Zephyr DW3110 Stage-2 RF bring-up plan
3c346bd  Document NUCLEO SPI5 Gateway PCB pin mapping
24dfcb5  Document NUCLEO SPI5 GPIO probe app plan
82a2c6b  Document Gateway module substitution and EOL strategy
```

This is a coherent progression:

```text
RF proof notes
  -> bring-up plan
  -> manufactured PCB pin mapping
  -> minimal probe-app plan
  -> long-term module-substitution architecture
```

The next modest code commit should remain disciplined:

```text
Add Gateway-specific Zephyr overlay and minimal SPI5/GPIO probe app
```

This should not attempt a DW3110 driver port yet. Its purpose is narrower:

* prove Zephyr can build for the NUCLEO target
* map the manufactured Gateway PCB pins
* exercise SPI5 at a basic level
* control DWM CS
* observe DWM IRQ
* toggle or assert DWM RESET
* produce serial console evidence
* document the result

That turns the architecture documents into executable evidence without overreaching.

## 15\. Suggested future `Thin-Pod-System` contents

A first version of the future system repo could contain:

```text
README.md
docs/
  architecture/
    System\_Overview.md
    Layered\_Architecture.md
    Evidence\_Chain.md
    Certification\_Boundaries.md
  interfaces/
    ThinPod\_Data\_Record\_v1.md
    ThinPod\_Gateway\_Interface\_v1.md
    UWB\_Packet\_Contract\_v1.md
  validation/
    Bench\_Test\_Protocol\_v1.md
    Motor\_Demo\_Protocol\_v1.md
    Replication\_Test\_Notes.md
  reporting/
    Maintenance\_Report\_Caveats.md
    Example\_Report\_Format.md
sample\_data/
  synthetic/
  bench/
software/
  parsers/
  dsp\_examples/
  tinyml\_examples/
```

The system repo should be explicitly described as an integration and evidence repository, not as an OSHWA-certified hardware artefact.

## 16\. First end-to-end demo definition

The first complete demo should be defined narrowly enough to finish.

Minimum viable demo:

```text
Input:
  mounted Thin-Pod node on a small motor or vibration source

Transport:
  vibration window or representative packet delivered to Gateway path

Gateway:
  received or simulated buffer processed by basic firmware or host script

Output:
  saved CSV/JSON log
  simple feature summary
  timestamped report file
```

Minimum feature set:

```text
RMS
peak
crest factor
basic FFT band energy
sample count
sample rate
sensor orientation note
test condition metadata
```

Required caveat:

> This demonstration shows an end-to-end data and processing chain. It does not validate industrial fault detection or diagnostic accuracy.

## 17\. Publication stance

The engineering story should be published only at the level justified by evidence.

Suitable early title styles:

* `Building an OSHWA-Certified Open Hardware Vibration Node`
* `A Modular Gateway Architecture for Open Vibration Monitoring Experiments`
* `From Sensor Node to Gateway: An Open Hardware Evidence Chain`
* `Thin-Pod: Open Hardware Boundaries for Vibration-Window Acquisition`

Avoid title styles such as:

* `An AI Predictive Maintenance Platform`
* `A Complete Industrial Condition Monitoring System`
* `Low-Cost Replacement for Commercial Vibration Monitoring`
* `TinyML Diagnosis of Rotating Machinery Faults`

The former titles are defensible. The latter titles overclaim.

## 18\. What the project should charge for, if commercialised

If Thin-Pod ever becomes commercially useful, the open hardware ethos need not prevent income.

The clean formulation is:

**Keep the map open; charge for navigation, assurance, integration, training and judgement.**

Potential paid layers:

* workshops
* assembled educational kits
* documentation review
* integration support
* custom test rigs
* validation assistance
* maintenance-report templates
* deployment consultancy
* advanced analytics above the open interface
* hosted dashboard or reporting layer, if kept portable

This preserves the distinction between an open reference architecture and a private tollgate.

## 19\. Review criteria before the next major step

Before moving to Gateway certification, system repo launch or a public article, run this checklist:

```text
Can the node repo be understood without private context?
Can the Gateway repo be understood without private context?
Are third-party modules clearly distinguished from open project files?
Are certification boundaries explicit?
Is there a clear data format?
Is there at least one reproducible bench test?
Is there a minimal firmware or software path from capture to saved result?
Are limitations stated plainly?
Can another technical reader identify what is real, what is planned and what remains unvalidated?
```

If the answer to any of these is weak, that weakness becomes documentation debt rather than failure.

## 20\. Recommended immediate next actions

1. Add the Gateway-specific Zephyr overlay and minimal SPI5/GPIO probe app.
2. Capture serial-console evidence from the probe app.
3. Save a short bring-up log documenting build target, pins, test method and result.
4. Create `docs/interfaces/` in the Gateway repo.
5. Draft `ThinPod\_Gateway\_Module\_Interface\_v1.md`.
6. Draft `ThinPod\_Data\_Record\_v1.md` for eventual migration into `Thin-Pod-System`.
7. Keep the next work narrow enough to become evidence, not aspiration.

## 21\. Final synthesis

Thin-Pod is now best understood as a staged open hardware and systems-engineering project.

The node establishes the first certified open hardware artefact. The Gateway extends the architecture toward UWB-linked capture, supervisory processing and eventual system-level integration. The future system repo should hold the interface contracts, data records, evidence chain and downstream DSP/TinyML examples. The project’s credibility depends on not confusing these layers.

The most important strategic move is to make the interfaces more durable than the parts. Qorvo CDKs, STM Nucleos, ESP32 modules and specific development boards may change or disappear. A well-documented modular interface can survive them.

The most important communication move is to keep the claims bounded. Its strength lies in the paper trail: open hardware files, OSHWA certification, bring-up evidence, failure records, pin mappings, module-substitution planning, and an evidence-led route toward end-to-end demonstration.

The project should proceed as an open, modular, reviewable evidence chain: not a tollgate, not a finished industrial platform, but a serious reference architecture that becomes more useful every time its interfaces, logs, tests and limitations become clearer.

