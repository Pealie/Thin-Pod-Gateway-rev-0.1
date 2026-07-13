# Thin-Pod Gateway rev 0.1: symbol and footprint provenance

**Project:** Thin-Pod Gateway  
**Hardware release:** rev 0.1  
**EDA environment:** KiCad 10  
**Status:** Release provenance record

## Purpose

This document records the origin and licence treatment of the symbols and footprints used by the published Thin-Pod Gateway rev 0.1 KiCad source. It follows the same release-boundary approach used by the certified Thin-Pod node repository: creator-authored material is identified separately from standard KiCad library dependencies, third-party CAD material and vendor-controlled material excluded from the open-hardware claim.

## Provenance inventory

| CAD item | Use | Provenance | Release treatment |
|---|---|---|---|
| Standard resistor, capacitor, test-point and mounting-hole symbols/footprints | Ordinary carrier-board components and PCB features | Standard KiCad libraries | Remain standard KiCad library dependencies; KiCad library licence and exception apply |
| `NUCLEO-N657X0-Q` symbol and mating footprint | U1 host-board interface | Project-local representation produced from STMicroelectronics UM3417 connector, pinout and mechanical information | Published as part of the Thin-Pod Gateway source under CERN-OHL-W-2.0; ST documentation and NUCLEO product remain under ST terms |
| `DWM3001-CDK` symbol and mating footprint | U2 UWB development-board interface | SnapEDA/SnapMagic-origin CAD data identified by the embedded source link, used as the carrier mating representation | Third-party Design File; original SnapEDA/SnapMagic terms and attribution retained; the commercial CDK remains outside the Gateway hardware licence |
| `XIAO ESP32-C6` symbol and carrier footprint | U3 optional onward-networking interface | Project representation based on Seeed Studio public pinout and mechanical information | Published within the Gateway source under CERN-OHL-W-2.0 to the extent of Thin-Pod authorship; the commercial XIAO module and Seeed documentation retain their original terms |
| Board-specific wiring, copper, zones, test access, module placement and mechanical arrangement | Creator-controlled Gateway carrier design | Thin-Pod / Neil Thomson | CERN-OHL-W-2.0 |

## Licence boundary

The Gateway open-hardware contribution comprises the carrier-board schematic, PCB layout, board-specific interface implementation, project-authored CAD material and derived fabrication outputs. Commercial development boards remain third-party products whose internal designs are outside the certification scope.

Standard KiCad library material remains subject to the KiCad Libraries Licence and exception. Third-party CAD files retain their own licence and attribution. Vendor documentation is referenced for dimensional, pinout and interface information and is not redistributed or relicensed by this project.

## Publication statement

The published KiCad project is self-contained at the schematic and PCB level and includes the symbols and footprint geometry required by the released design. No claim is made that the NUCLEO-N657X0-Q, DWM3001-CDK or XIAO ESP32-C6 modules themselves are open hardware under the Gateway licence.

## Release conclusion

**PASS:** symbol and footprint provenance is recorded at a level consistent with the certified Thin-Pod node release. The remaining release steps concern repository checks, public visibility, immutable tagging and OSHWA submission.