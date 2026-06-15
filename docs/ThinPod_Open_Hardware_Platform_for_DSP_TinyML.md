# Thin-Pod Gateway and Node Platform: Hardware Boundaries, Reference Firmware and the DSP / TinyML Interface

## Executive summary

The recent Thin-Pod Gateway and node work clarifies a central proposition: the project is not merely an open hardware vibration sensor, nor is it yet a complete predictive-maintenance analytics product. Its strongest and most defensible identity is as an **open hardware vibration-window platform**. It provides the sensing, carrier-board, Gateway, module-interconnect, firmware bring-up and data-exposure path required to let practitioners begin work at the DSP and TinyML boundary rather than at the PCB, RF, power-integrity or board-support layer.

That distinction matters. Many technically capable practitioners, especially those coming from IT, software, data science or machine-learning backgrounds, do not want first to become embedded-hardware developers. They do not want to design a sensor carrier, debug power rails, reflow headers, recover J-Link probes, decipher UWB development-module wiring, or establish from first principles whether an accelerometer signal path is credible. They want a documented stream of vibration windows, with enough integrity metadata and reproducibility evidence that they can begin modelling almost immediately.

Thin-Pod’s opportunity is therefore not to prescribe the final model. It is to provide the reliable, open path to model-ready vibration data.

The concise platform thesis is:

```text
Thin-Pod provides the open hardware and reference firmware needed to acquire,
transport and expose vibration-window data. Downstream DSP, TinyML models and
diagnostic interpretations are intentionally left as user-developed layers above
the documented interface.
```

This is not a narrowing of the project. It is a strengthening of it. It defines what the platform is for, where the OSHWA hardware boundary lies, why the firmware is included, and what remains properly outside the certification claim.

## From hardware object to data-access platform

The practical insight that emerged from discussion with an IT professional was simple but important: the user does not necessarily want ‘a hardware project’. They want the hardware to get out of the way.

For such a practitioner, the value of Thin-Pod lies in allowing the following workflow:

```text
Build or obtain the open node and Gateway hardware.
Flash the reference firmware.
Run the Gateway logger.
Receive timestamped vibration windows in a documented format.
Start DSP, feature extraction and TinyML experimentation.
```

That means the platform’s essential output is not a polished dashboard, not a finished industrial classifier, and not a proprietary predictive-maintenance service. Its essential output is a trustworthy, documented vibration-window stream.

A minimal useful output might be expressed as:

```text
timestamp\_ms,node\_id,sequence,sample\_rate\_hz,sample\_count,accel\_mg\[]
```

or as a structured Gateway packet log:

```text
TPGW\_PKT seq=<n> src=0x0001 dst=0x0000 uptime\_ms=<t>
samples=<n> rate=<hz> checksum=<value> rms\_mg=<value>
```

That is the hand-off point. Below it is hardware, firmware, transport and measurement integrity. Above it is signal processing, modelling, inference and interpretation.

The project’s value is therefore not only that it is open hardware. It is that the open hardware is organised to produce a clean computational boundary.

## The modular stack

The most useful way to understand the Thin-Pod Gateway and node platform is as a layered system:

```text
Open hardware node and Gateway platform
    ↓
reference firmware for bring-up and operation
    ↓
UWB or staged transport layer
    ↓
shared packet contract
    ↓
Gateway packet logging and parser/export tools
    ↓
documented vibration-window interface
    ↓
user-developed DSP and TinyML
    ↓
diagnostic interpretation, dashboard or service layer
```

The essential discipline is to avoid collapsing these layers into a single vague claim. Each layer should have a role, evidence standard and boundary.

The hardware layer provides the physical sensing, carrier-board, power, signal, module and test-point infrastructure. The firmware layer proves and operates the board sufficiently to produce or transport vibration windows. The packet layer defines what is being handed off. The logging and parser layer makes the hand-off usable by a practitioner. The DSP and TinyML layer sits above that hand-off, where different users can develop different methods. The diagnostic layer, if any, is still higher again and should not be confused with either OSHWA certification or basic platform capability.

## OSHWA boundary for Gateway rev 0.1

For Thin-Pod Gateway rev 0.1, the most defensible OSHWA framing is that the certified object would be the **creator-designed Gateway carrier PCB and its open hardware documentation**. That would include the KiCad design source, schematics, PCB layout, fabrication outputs, BOM, footprint provenance, release manifest, interface documentation, test-point information and bring-up evidence.

The third-party development modules remain outside the hardware certification claim. The Gateway carrier board may host or interconnect a Qorvo DWM3001-CDK, an STM32 NUCLEO-N657X0-Q and a Seeed Studio XIAO ESP32-C6, but it does not claim those commercial modules as open hardware.

The firmware and software should be described as **open reference bring-up, verification and interface software**. It supports reproducibility and makes the intended architecture inspectable. It should not be described as the certified hardware object itself.

A suitable boundary statement is:

```text
The certified hardware scope is the Thin-Pod Gateway rev 0.1 carrier PCB,
including its editable design source, fabrication outputs, BOM, interface
documentation, footprint provenance and bring-up evidence.

Project-authored firmware and software in this repository are provided as open
reference, bring-up and verification material. This includes Zephyr RTT
alive-test firmware, staged UWB responder / initiator scaffolding, packet
logging, vibration-window ingestion and optional DSP / TinyML reference
examples.

The DSP / TinyML layer is included only to demonstrate possible downstream
processing of preserved vibration windows. It is not part of the certified
hardware object, does not constitute a production diagnostic claim and does not
certify industrial fault-detection performance.
```

This position is clear without becoming evasive. It says what is open, what is evidenced, and what is not being claimed.

## Firmware and software as reference infrastructure

The current Gateway repository now contains two important firmware layers.

The first is the Zephyr RTT alive-test baseline:

```
firmware/gateway/zephyr_rtt_alive/
firmware/node/zephyr_rtt_alive/
```

These applications prove the basic development loop: build, flash, execute and observe over SEGGER RTT.

The second is the staged UWB role harness:

```text
firmware/common/thinpod_protocol/
firmware/gateway/uwb_initiator/
firmware/node/uwb_responder/
tools/packet_logging/
```

This establishes firmware identities, a shared packet contract, synthetic vibration-window generation, Gateway packet logging and parser-ready ingestion. It does not yet claim real DW3110 RF exchange.

For rev 0.1, this is an ideal stage of maturity. It shows serious system architecture without overclaiming. The firmware is not decorative, but neither is it presented as a finished industrial analytics stack.

The proper phrase is:

```text
reference bring-up and verification firmware
```

or, more fully:

```text
project-authored reference firmware and software for bring-up, verification,
interface testing and reproducibility of the intended Gateway architecture
```

## DSP / TinyML reference layer

A completed DSP / TinyML reference layer can also fit inside this boundary, provided it is described carefully.

It should be framed as a **reference example layer above the vibration-window interface**, not as a certified diagnostic system.

Appropriate content would include:

```text
RMS, peak and peak-to-peak examples
crest factor
kurtosis
FFT band-energy examples
envelope-feature examples
synthetic or sample vibration-window processing
TinyML inference demonstration using clearly labelled test data
parser and feature-extraction scripts
repeatable example outputs
```

Riskier language should be avoided unless field validation has actually been carried out:

```text
detects bearing failure
validated predictive-maintenance AI
certified diagnostic model
production TinyML condition-monitoring engine
industrial fault-detection performance
```

The strongest phrasing is:

```text
DSP and TinyML reference examples are included to exercise the Gateway data path
after packet capture and vibration-window ingestion. They are provided for
reproducibility, experimentation and verification of software interfaces. They
are not required to fabricate the Gateway PCB, are not part of the certified
hardware design and do not constitute validated diagnostic or industrial
condition-monitoring claims.
```

This keeps the project useful to ML practitioners while avoiding an inflated certification claim.

## Minimum open platform for DSP and TinyML practitioners

The minimum useful platform is not the entire algorithmic stack. It is the reliable production of documented vibration windows.

For the node, the minimum useful open hardware and firmware offering is:

```text
documented sensor power and signal chain
documented accelerometer output route
documented sampling assumptions
documented UWB/module interface
test points and bring-up checks
reference firmware for producing or emulating a vibration window
```

For the Gateway, the minimum useful open hardware and firmware offering is:

```text
documented UWB/module interface
documented host/supervisor route
documented power and ground behaviour
reference firmware for receiving or emulating vibration windows
Gateway packet logging
parser/export tool
test vectors and sample logs
```

Together, these allow a practitioner to begin at the data boundary:

```text
node/Gateway platform
    ↓
vibration-window stream
    ↓
CSV / JSON / packet log
    ↓
notebook / Python / DSP / ML toolchain
```

The practitioner should not have to solve the sensing platform before developing the model.

A practitioner-facing value statement could be:

```text
Thin-Pod Gateway and node hardware provide an open route from physical vibration
sensing to documented vibration-window data. The platform is intended to let
researchers, engineers and ML practitioners focus on DSP, TinyML and diagnostic
experimentation without first having to design the sensor carrier, Gateway
carrier, module interconnect, packet format or bring-up path.
```

That is a strong and accurate description of the platform’s utility.

## Rev 0.1 versus rev 0.3

For Gateway rev 0.1, the firmware is most safely described as reference bring-up and verification software. The hardware object is a carrier PCB around commercial modules. Real RF transport, host transfer, ADXL1005 vibration-window movement, DSP and TinyML remain staged or future verification layers unless completed and documented.

For a future rev 0.3, especially if the project moves toward castellated DWM3001C integration, SMT implementation, or a more product-like board, the firmware boundary changes. Firmware required for basic operation becomes more central to the release.

The shift is:

```text
rev 0.1:
open hardware carrier PCB + reference firmware scaffolding

rev 0.3:
integrated hardware platform + required open operating and verification firmware
```

For rev 0.3, the minimum firmware package may need to include board support, pin configuration, radio initialisation, UWB responder/initiator roles, packet framing, host-interface transfer, diagnostic logging and factory/bring-up tests.

The certification still need not claim the whole analytics stack. But the firmware needed to operate and reproduce the board’s basic behaviour becomes part of the platform evidence.

A future rev 0.3 boundary statement might read:

```text
Thin-Pod Gateway rev 0.3 includes project-authored open firmware and software
required for hardware bring-up, UWB role operation, packet validation,
host-interface verification and diagnostic logging. These files are included as
part of the reproducibility evidence for the open hardware release. The certified
hardware scope does not extend to third-party modules, vendor SDK internals,
cloud services, production analytics, or diagnostic claims beyond the documented
verification tests.
```

## What the platform should not claim

The project should continue to avoid claiming more than the evidence supports.

It should not claim that OSHWA certification validates predictive-maintenance accuracy. It should not imply that Qorvo, ST, Nordic, Espressif or Seeed hardware and SDKs are themselves covered by the project’s open hardware claim. It should not imply that TinyML examples are production diagnostics.

The platform can legitimately claim:

```text
open hardware carrier-board design
documented sensing and Gateway architecture
reference firmware and software for bring-up and verification
packet contract
Gateway logging path
vibration-window ingestion boundary
basis for downstream DSP / TinyML development
```

It should avoid claiming:

```text
validated industrial fault diagnosis
certified analytics performance
complete production UWB system, unless and until completed
certified third-party development modules
end-to-end predictive-maintenance product
```

This discipline is not mere caution. It is what makes the project more credible.

## The strategic identity of Thin-Pod

The strongest strategic identity for Thin-Pod is:

```text
an open hardware vibration-window platform for DSP and TinyML development
```

Or, more fully:

```text
Thin-Pod is an open hardware platform intended to move physical vibration
measurements into a documented, reproducible data interface, so that researchers
and practitioners can develop their own DSP, TinyML and diagnostic methods above
the hardware boundary.
```

This framing places the project in a useful middle position. It is more than a PCB. It is less than an overclaimed AI diagnostic appliance. It is an enabling layer.

The phrase ‘model-ready data’ is useful if handled carefully. It should not mean ‘validated model’. It should mean that the hardware and firmware expose data in a form suitable for modelling.

A safe phrase is:

```text
documented vibration-window interface for downstream DSP and TinyML development
```

Another is:

```text
open hardware route to model-ready vibration-window data
```

## Conclusion

The core insight is that Thin-Pod’s value lies in the boundary it creates. It lets one party do the hard work of open sensing hardware, carrier-board design, Gateway integration, firmware bring-up, packet framing and data exposure, so another party can begin at the DSP and TinyML layer.

That is not a weakness in the project. It is the project’s best product definition.

The Gateway and node platform should therefore be presented as a trustworthy open hardware path to vibration-window data. The reference firmware and software make that path reproducible. The DSP and TinyML layer remains open-ended, user-developed and deliberately above the certification boundary.

The most compact formulation is:

```text
The product is not the model.
The product is the reliable open path to model-ready vibration data.
```

