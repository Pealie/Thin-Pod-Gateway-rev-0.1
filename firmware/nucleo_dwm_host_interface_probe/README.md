# NUCLEO DWM Host Interface Probe

Runs on the STM32 NUCLEO-N657X0-Q and proves the first real host-interface exchange with the Gateway DWM3001-CDK.

The application:

- drives SPI5 at 1 MHz, mode 0, MSB first;
- uses PA3 as manual active-low CS;
- reads PB9 as DWM READY;
- resets the DWM through PD0 and releases the line as a high-impedance input;
- sends an exactly 16-byte `GET_CAPABILITIES` request;
- clocks an exactly 32-byte response;
- checks magic, version, flags, lengths, CRC, status, opcode and sequence;
- runs ten valid exchanges;
- checks unknown version, reserved flags, oversized payload, bad CRC and unknown opcode handling;
- runs a local valid-CRC sequence-mismatch parser self-test;
- uses fixed static buffers with guard words and no dynamic allocation.

## Supported complete-response diagnostic

The probe prints the complete 32-byte physical response immediately after it has been clocked:

```text
response_after_clock=54 50 47 57 ...
```

The hexadecimal dump permits direct comparison with the frozen TPHIP vectors and preserves the received bytes when a validation failure occurs.

This application is explicitly a host-interface validation probe, so the complete response dump is retained as supported output. It was present in the firmware used for the successful physical validation and keeps the committed source aligned with the tested build.

## Hardware acceptance result

One cold boot runs one complete suite. The acceptance target was three PoE power cycles, each producing ten valid exchanges and zero CRC, sequence, guard or READY failures.

That target was met on 12 July 2026. Each complete captured suite concluded:

```text
suite=GET_CAPABILITIES passes=16 failures=0 guard=OK result=PASS
```

The next Gateway milestone is a real DWM-to-DWM UWB RF exchange.
