# Thin-Pod Gateway NUCLEO GPIO Probe

Minimal Zephyr GPIO-only probe for the Thin-Pod Gateway rev 0.1 NUCLEO-N657X0-Q supervisor.

## Scope

This app proves only:

- `PD0 -> DWM_RESET`
- `PA3 -> DWM_CS`
- `PB9 <- DWM_IRQ`

It deliberately avoids SPI, DW3110 register access and UWB RF behaviour.

## Expected serial output

```text
Thin-Pod Gateway rev0.1 NUCLEO GPIO probe
Board: nucleo_n657x0_q
Purpose: RESET / CS / IRQ pin-path confirmation only
No SPI. No DW3110 register access. No UWB RF.

DWM_RESET / PD0 ready: controller=... pin=0
DWM_CS / PA3 ready: controller=... pin=3
DWM_IRQ / PB9 ready: controller=... pin=9
GPIO configuration complete
probe=0 phase=LOW reset=0 cs=0 irq=...
probe=0 phase=HIGH reset=1 cs=1 irq=...
```

## Acceptance boundary

The firmware heartbeat alone is not Gateway PCB proof. The acceptance evidence is RESET and CS toggling at the DWM3001-CDK header, and IRQ being read consistently by firmware.

Measure:

| Signal | NUCLEO pin | DWM3001-CDK header | Expected |
|---|---:|---:|---|
| RESET | PD0 | J10.12 | toggles low/high |
| CS | PA3 | J10.24 | toggles low/high |
| IRQ | PB9 | J10.15 | readable input level |
