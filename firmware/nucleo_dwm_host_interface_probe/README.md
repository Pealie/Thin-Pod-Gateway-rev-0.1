# NUCLEO DWM Host Interface Probe

Runs on the STM32 NUCLEO-N657X0-Q.

Purpose:
- Act as SPI host/controller.
- Send GET_CAPABILITIES, GET_STATUS and GET_COUNTERS.
- Validate response status, sequence and CRC.
- Print host-interface proof lines over serial.

Expected first proof lines:
host_if_probe=GET_CAPABILITIES ret=0 status=OK protocol=1.0 counters=1 crc=1
host_if_probe=GET_STATUS ret=0 status=OK role=gateway_initiator backend=stub state=ready
host_if_probe=GET_COUNTERS ret=0 status=OK poll_count=N packet_count=M bad_command_count=0
