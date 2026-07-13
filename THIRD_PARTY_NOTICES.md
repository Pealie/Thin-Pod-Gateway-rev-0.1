# Third-party notices and dependency boundaries

Thin-Pod Gateway rev 0.1 is a creator-designed carrier PCB and software/documentation project that interfaces with commercial development modules and external software. Inclusion in a BOM, build manifest or interface description does not relicense third-party material.

## Commercial hardware

| Item | Role | Treatment |
|---|---|---|
| STMicroelectronics `NUCLEO-N657X0-Q` | Main Gateway host | Commercial development board. Internal design, firmware and documentation remain under STMicroelectronics terms. |
| Qorvo `DWM3001-CDK` | Gateway-side UWB subsystem | Commercial development board. Internal PCB, DW3110 device, supplied firmware and documentation remain under Qorvo terms. |
| Seeed Studio `XIAO ESP32-C6` | Optional onward-networking subsystem | Commercial module. Internal design, firmware and documentation remain under Seeed Studio and upstream terms. |
| Standard electronic components and assembly hardware | Carrier-board implementation | Commercial bought-in parts identified for reproducibility; no relicensing is asserted. |

## External software and toolchains

| Dependency | Use in this repository | Licence treatment |
|---|---|---|
| Qorvo `DW3_QM33_SDK_1.1.0` | External DW3000/DW3110 driver compiled by the Stage-2 applications | Not distributed in this repository. The installed SDK identifies its driver material as `LicenseRef-QORVO-2`; the authoritative terms remain with the SDK. |
| Zephyr Project | RTOS, drivers, devicetree and build system | Upstream Zephyr licences and file-level notices apply. |
| Nordic nRF Connect SDK | DWM3001-CDK firmware build environment | Upstream Nordic, Zephyr and component-specific terms apply. |
| STM32/Zephyr support packages | NUCLEO firmware build environment | Upstream STMicroelectronics, Zephyr and component-specific terms apply. |
| KiCad | Hardware design and fabrication-output generation | KiCad application and library licences apply to the tooling and any library-derived material. |

The files under `firmware/common/qorvo_dw3000/` are Thin-Pod-authored integration and platform-adapter material unless a file states otherwise. They locate and compile an external Qorvo SDK; they do not contain or relicense that SDK.

## CAD and footprint provenance

The certification release must identify every non-standard symbol, footprint and 3D model used by the editable KiCad project. Each dependency must be placed into one of these categories:

1. independently authored Thin-Pod source, licensed under `CERN-OHL-W-2.0`;
2. unmodified or adapted KiCad-library material, retaining the KiCad Libraries Licence and exception;
3. third-party design material, retaining its original terms and attribution;
4. vendor-controlled material referenced externally and excluded from the release package.

The current repository does not yet contain the editable KiCad project advertised by the README. Final footprint provenance therefore remains open until that exact project and its project-local libraries are committed and reviewed.

## Trade marks and certification marks

`Thin-Pod`, manufacturer names and product names may be trade marks of their respective owners. The OSHWA certification mark and a future Gateway UID must only be used after certification is granted and in accordance with OSHWA’s mark-usage terms. The existing Thin-Pod node UID `UK000091` applies only to the separate sensor-node carrier PCB.
