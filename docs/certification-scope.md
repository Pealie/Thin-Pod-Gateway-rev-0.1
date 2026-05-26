# Thin-Pod Gateway rev 0.1: Draft OSHWA Certification Scope

**Project:** Thin-Pod Gateway  
**Hardware release:** rev 0.1  
**Release stage:** PCB ordered; bring-up and release freeze pending  
**Certification status:** Candidate for a later separate OSHWA self-certification application  
**Document date:** 27 May 2026  

## Purpose

This document defines the intended certification boundary for **Thin-Pod Gateway rev 0.1**, a separate hardware product from the `Thin-Pod rev 0.1` sensor-node carrier PCB already submitted to OSHWA.

It is a draft scope document for use during Gateway bring-up and repository preparation. It does not assert that the Gateway has passed verification or that an OSHWA application has been submitted.

## Certified-item candidate

The proposed certified item is:

> **Thin-Pod Gateway rev 0.1: an open-hardware carrier PCB for receiving and supervising Thin-Pod vibration-window data using commercially supplied processing and wireless development modules.**

The carrier PCB is intended to provide power distribution, local decoupling, test access and wired host-interface routing between an STM32 NUCLEO-N657X0-Q, a Qorvo DWM3001-CDK and an optional Seeed Studio XIAO ESP32-C6.

## Relationship to Thin-Pod rev 0.1

The Gateway is a **separate rev 0.1 product**, not an extension of the submitted sensor-node certification item.

| Product | Revision | OSHWA position |
|---|---:|---|
| Thin-Pod sensor-node carrier PCB | rev 0.1 | Application submitted on 26 May 2026; Gateway excluded from that scope |
| Thin-Pod Gateway carrier PCB | rev 0.1 | Separate candidate application after bring-up and release freeze |

The shared revision number indicates the first hardware release of each distinct product family. It does not merge the certification scopes.

## Ordered PCB identifier

The PCB currently in manufacture may contain the silkscreen/build identifier `rev 0.1f`. This is treated as a **pre-release fabrication-build identifier** within the Gateway rev 0.1 development path.

Before a Gateway OSHWA submission, the release record must either:

- freeze the tested `rev 0.1f` build as the documented physical implementation of Gateway rev 0.1; or
- incorporate any required pre-release corrections into a clean Gateway rev 0.1 release file set and document the relationship to the tested fabrication build.

No `rev 0.3` hardware is included in the present Gateway scope. Rev 0.3 is reserved for a later SMT/chip-down transition and is deferred.

## Included in the future Gateway certification scope

| Scope element | Description |
|---|---|
| Gateway carrier PCB | Creator-designed two-layer interconnect carrier board |
| NUCLEO mechanical/electrical interface | Carrier-board connections to the commercial NUCLEO-N657X0-Q |
| DWM3001-CDK mechanical/electrical interface | Carrier-board connections to the commercial Gateway-side UWB development board |
| XIAO ESP32-C6 mechanical/electrical interface | Carrier-board connections to optional commercial onward-networking module |
| Power distribution | `5V_GATEWAY`, `3V3_GATEWAY` auxiliary rail and common-ground routing |
| Local decoupling and pull-ups | Passive circuitry fitted on the carrier board |
| Host-interface wiring | Shared SPI5 route and separate chip-select/interrupt/reset lines |
| Test access | TP1–TP13 observability points |
| Mounting geometry | Carrier-board mechanical support and module placement |
| Editable hardware source | KiCad schematic, PCB and required project-local library sources |
| Fabrication outputs | Gerber and drill files derived from the release source |
| BOM and documentation | Public sourcing, bring-up, verification and provenance record |

## Explicitly outside the future Gateway certification scope

| Excluded item | Reason |
|---|---|
| STM32 NUCLEO-N657X0-Q as open hardware | Commercial STMicroelectronics development board; interfaced by the carrier PCB but not relicensed |
| Qorvo DWM3001-CDK as open hardware | Commercial UWB development board; interfaced by the carrier PCB but not relicensed |
| Seeed Studio XIAO ESP32-C6 as open hardware | Commercial optional networking module; interfaced by the carrier PCB but not relicensed |
| Thin-Pod sensor-node PCB | Separate hardware product with its own OSHWA application boundary |
| Completed UWB node-to-Gateway data transport | Functional system milestone, not an unverified carrier-board claim |
| DSP, TinyML or fault-detection performance | Later analysis functionality; not claimed by the initial carrier-board certification |
| Onward Wi-Fi/networking function | Optional later firmware/system function |
| PoE power integration | Not implemented merely by the presence of NUCLEO Ethernet hardware |
| Rev 0.3 SMT/chip-down Gateway | Later design direction, explicitly deferred |
| Radio, EMC, safety, CE/UKCA or industrial compliance | Not granted by OSHWA certification |

## Commercial module boundary

The Gateway BOM should include the NUCLEO, DWM3001-CDK and XIAO where required to reproduce the functional assembly. BOM inclusion identifies how the carrier board is used; it does not assert that commercial modules are open-source hardware.

The creator-controlled open-hardware contribution is the Gateway carrier-board design, the creator-authored source and documentation, and any demonstrably project-authored interface footprints. Any reused third-party CAD dependency must be separately identified under its applicable licence or replaced with a project-authored mating-interface representation before public release.

## Proposed licence selections

The following is the provisional OSHWA licence position, to be reviewed once Gateway source/footprint provenance and any firmware release scope have been fixed.

| Material category | Proposed treatment |
|---|---|
| Creator-designed Gateway hardware source | `CERN-OHL-W-2.0` |
| Creator-authored documentation and images | `CC-BY-4.0` |
| Firmware/software | Decide only once a firmware release is included; `No software` remains valid only if certifying the carrier hardware without supplied firmware |
| Third-party CAD dependencies | Retain under separately documented original terms or replace before submission |
| Commercial development modules | Listed and referenced; not relicensed |

## Pre-submission verification gates

| Gate | Evidence required |
|---|---|
| Physical inspection | Bare PCB and fitted/module orientation recorded photographically |
| Continuity/resistance | No unintended rail shorts; intended rails and critical signal routes verified |
| Power bring-up | `5V_GATEWAY` and `3V3_GATEWAY` validated under staged module population |
| Module seating and power | NUCLEO, DWM3001-CDK and XIAO powered only after preceding gates pass |
| Host-interface observability | SPI/control lines verified physically and, where possible, electrically exercised |
| Source/fabrication reconciliation | Published KiCad source matches final Gerber archive and BOM |
| Footprint provenance | Public licence boundary for all non-standard interface files recorded |
| Release freeze | Tagged public release created before Gateway OSHWA form submission |

## Conclusion

Thin-Pod Gateway rev 0.1 should be developed as a separate, rigorous open-hardware release candidate. Its first task is to validate the ordered carrier PCB and the module-interface path. OSHWA submission should follow physical evidence and source reconciliation, not precede them.
