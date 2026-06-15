# Thin-Pod Protocol Baseline

Shared packet contract for the staged node responder and Gateway initiator applications.

The current packet format is intentionally small and deterministic. It provides a stable contract for firmware identity, Gateway packet logging and vibration-window ingestion before the real DW3110 UWB backend and ADXL1005 sampling path are restored.

```text
Gateway ID: 0x0000
Node ID:    0x0001
Samples:    32
Units:      int16_t milligravity
Rate:       1600 Hz
```
