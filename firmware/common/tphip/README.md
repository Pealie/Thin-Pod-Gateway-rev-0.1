# Thin-Pod Host Interface Protocol shared module

`tphip.c` and `tphip.h` provide the byte-level contract shared by the
Gateway DWM3001-CDK SPIS endpoint and the NUCLEO-N657X0-Q SPI5 host probe.

The first frozen transaction is:

- request transaction: 16 bytes;
- response transaction: 32 bytes;
- command: `GET_CAPABILITIES` (`0x01`);
- response: `0x81`;
- CRC: CRC-16/CCITT-FALSE;
- request and response sequence values must match.

All multibyte fields are encoded explicitly in little-endian form. The module
uses no dynamic allocation and does not cast received byte arrays to packed
structures.