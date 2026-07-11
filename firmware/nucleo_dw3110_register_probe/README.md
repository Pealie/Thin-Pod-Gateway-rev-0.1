# NUCLEO DW3110 Register Probe

Purpose: perform the first Gateway-local DW3110 register-read probe over the verified SPI5 path.

Scope:

- no UWB RF
- no ranging
- no node-to-Gateway communication
- no production driver

Verified Gateway SPI5 path:

- TP7 / CS
- TP10 / SCK
- TP11 / COPI / MOSI
- TP12 / CIPO / MISO

The probe attempts a raw read of register 0 / DEV_ID and prints TX bytes, RX bytes and candidate 32-bit interpretations.

Expected DW3110-class value:

0xDECA0302

A zero or flat RX result is diagnostic evidence. It may indicate that additional DWM3001-CDK module routing, reset, wake or ownership steps are required before direct DW3110 access works from the NUCLEO.