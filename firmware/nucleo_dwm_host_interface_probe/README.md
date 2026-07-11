# NUCLEO DWM Host Interface Probe

Runs on the STM32 NUCLEO-N657X0-Q and proves the first real host-interface
exchange with the Gateway DWM3001-CDK.

The application:

- drives SPI5 at 1 MHz, mode 0, MSB first;
- uses PA3 as manual active-low CS;
- reads PB9 as DWM READY;
- optionally resets the DWM through PD0;
- sends an exactly 16-byte `GET_CAPABILITIES` request;
- clocks an exactly 32-byte response;
- checks magic, version, flags, lengths, CRC, status, opcode and sequence;
- runs ten valid exchanges;
- checks unknown version, reserved flags, oversized payload, bad CRC and
  unknown opcode handling;
- runs a local valid-CRC sequence-mismatch parser self-test;
- uses fixed static buffers with guard words and no dynamic allocation.

One cold boot runs one complete suite. The hardware acceptance target is three
cold power cycles, each with ten valid exchanges and zero CRC, sequence, guard
or READY failures.