# 100 Engineering Log: DW3110 Physical Identity and Initialisation Proof

**Date:** 12 July 2026  
**Repository:** `Pealie/Thin-Pod-Gateway-rev-0.1`  
**Branch:** `gateway-dw3110-rf-exchange`  
**Final branch commit:** `6988263aef77a63e214045627d8691e1f9ca2777`  
**Scope:** External Qorvo DW3000/DW3110 driver integration, reproducible Gateway and Node builds, physical SPI identity proof, DW3110 initialisation, raw RTT evidence capture and repository evidence preservation

## Summary

This work established the first complete physical communication proof between the nRF52833 and the internal DW3110 transceiver on both Thin-Pod DWM3001-CDK boards.

The milestone progressed through a controlled sequence:

1. integrate the Qorvo DW3000/DW3110 driver as an external SDK dependency;
2. build the Gateway-side and Node-side Stage-2 applications;
3. implement a bounded identity and initialisation proof;
4. flash and test each DWM3001-CDK independently;
5. capture repeatable RTT evidence;
6. record firmware and log hashes;
7. preserve the original evidence bytes in Git;
8. leave the branch clean, pushed and ready for the next RF milestone.

Both boards successfully prepared their Zephyr SPI3 and GPIO interfaces, asserted and released the DW3110 reset line, completed `dwt\_probe()`, reached IDLE\_RC, completed `dwt\_initialise(DWT\_READ\_OTP\_ALL)`, and returned stable device identifiers at both 2 MHz and 8 MHz SPI rates.

The Gateway-side and Node-side boards also returned distinct OTP part identifiers, confirming that the validation covered two separate physical DW3110 transceivers.

The milestone demonstrates physical SPI communication and successful DW3110 initialisation. It does not yet demonstrate UWB transmission, reception, ranging or telemetry transport.

## Repository progression

The work was developed on:

```text
gateway-dw3110-rf-exchange
```

The main commits in the milestone sequence were:

```text
f16f693 Add external Qorvo DW3000 build integration
73e3668 Document Qorvo DW3000 build integration
368c570 Implement DW3110 identity and initialisation proof
d87dcfd Record DW3110 physical identity and initialisation proof
a1e14e6 Preserve raw DW3110 RTT evidence bytes
6988263 Store exact raw DW3110 RTT evidence
```

The branch finished at:

```text
6988263aef77a63e214045627d8691e1f9ca2777
```

Final Git state:

```text
On branch gateway-dw3110-rf-exchange
Your branch is up to date with 'origin/gateway-dw3110-rf-exchange'.

nothing to commit, working tree clean
```

## External Qorvo driver integration

The Qorvo DW3000/DW3110 source remained outside the Git repository and was selected through:

```text
QORVO\_DW3\_SDK\_ROOT
```

The validated local SDK location was:

```text
C:\\Qorvo\\DW3\_QM33\_SDK\_1.1.0
```

The integration compiled the required Qorvo sources together with the Thin-Pod Zephyr platform adapter while maintaining a clear boundary between Thin-Pod-owned source and Qorvo-owned SDK material.

The common integration included:

```text
firmware/common/qorvo\_dw3000/
```

with platform support for:

* SPI transactions;
* slow and fast SPI rates;
* DW3110 reset control;
* IRQ sampling;
* delays and critical sections;
* Qorvo probe-interface callbacks;
* physical identity and initialisation reporting.

Both Gateway-side and Node-side applications built successfully with the external driver.

## Build results

The Gateway identity and initialisation build completed successfully:

```text
FLASH: 51,808 bytes of 512 KB, 9.88%
RAM:    9,280 bytes of 128 KB, 7.08%
Result: PASS
```

The Node application also completed successfully.

The generated ELF and merged HEX artefacts were recorded with SHA-256 hashes:

```text
SHA256  872BC431F2DFEADC609F4A04C8B79BDB9E0CDEE7CB894D80CA70DA46689A33D4  gateway zephyr.elf
SHA256  9FAEA9B8698F03D92BA6E776B13F57D17E3CEC113D27B089387ED3E14A1FE7D1  gateway merged.hex
SHA256  C26C4C893B4666C77B1A323640252779D684D9C0F59BE595A42881D1F56CBBC1  node zephyr.elf
SHA256  7F419321B029CBCC661A48E9DEFA453A8BAF19EDDBF259237466774E7FEA5F3C  node merged.hex
```

## Physical proof sequence

The identity proof used a deliberately bounded sequence:

1. prepare Zephyr SPI3 and GPIO devices;
2. select the 2 MHz SPI rate;
3. assert DW3110 RESET low;
4. release RESET and allow the transceiver to settle;
5. sample the IRQ line;
6. call `dwt\_probe()`;
7. read and validate the device identifier;
8. wait for IDLE\_RC with a finite timeout;
9. call `dwt\_initialise(DWT\_READ\_OTP\_ALL)`;
10. read the post-initialisation device identifier;
11. capture part ID, lot ID, OTP revision and reference values;
12. select the 8 MHz SPI rate;
13. repeat the device-ID read;
14. verify device-ID stability;
15. emit structured RTT result lines.

The proof intentionally excluded RF configuration, transmission and reception.

## Gateway-side physical result

The Gateway-side DWM3001-CDK reported:

```text
TPDW\_RESULT role=gateway role\_id=0x0000 result=PASS failure\_stage=none final\_rc=0 platform\_rc=0 reset\_rc=0 probe\_rc=0 idlerc=1 init\_rc=0 devid\_match=1 post\_match=1 fast\_match=1 stable=1
```

Device evidence:

```text
expected=0xdeca0312
mask=0xffffff0f
devid\_slow=0xdeca0302
devid\_after\_init=0xdeca0302
devid\_fast=0xdeca0302
```

OTP evidence:

```text
part\_id=0x0f34872a
lot\_hi=0x00005056
lot\_lo=0x36583637
otp\_revision=1
ref\_voltage=119
ref\_temperature=124
```

IRQ observation:

```text
before=1
after=1
```

Result:

```text
PASS
```

## Node-side physical result

The Node-side DWM3001-CDK reported:

```text
TPDW\_RESULT role=node role\_id=0x0001 result=PASS failure\_stage=none final\_rc=0 platform\_rc=0 reset\_rc=0 probe\_rc=0 idlerc=1 init\_rc=0 devid\_match=1 post\_match=1 fast\_match=1 stable=1
```

Device evidence:

```text
expected=0xdeca0312
mask=0xffffff0f
devid\_slow=0xdeca0302
devid\_after\_init=0xdeca0302
devid\_fast=0xdeca0302
```

OTP evidence:

```text
part\_id=0x8ec49c9a
lot\_hi=0x00005056
lot\_lo=0x34583230
otp\_revision=1
ref\_voltage=118
ref\_temperature=126
```

IRQ observation:

```text
before=1
after=1
```

Result:

```text
PASS
```

## Device distinction

The two boards returned different OTP part IDs:

```text
Gateway: 0x0f34872a
Node:    0x8ec49c9a
```

This provides evidence that two distinct physical DW3110 devices were independently flashed, probed and initialised.

## Evidence package

The durable physical-validation record was added at:

```text
docs/validation/DW3110\_Physical\_Identity\_Initialisation\_Validation.md
```

The external integration document was updated at:

```text
docs/firmware/Qorvo\_DW3000\_External\_SDK\_Integration.md
```

The raw validation evidence was stored under:

```text
logs/validation/dw3110-identity-init/
```

The evidence set contains:

```text
build-artifact-sha256.txt
gateway-dwm3001cdk-rtt.log
node-dwm3001cdk-rtt.log
```

Recorded RTT SHA-256 hashes:

```text
SHA256  DA14DCC850B29FFE88ED0CE282CE651F9DB178BF7CC7AFFE446EA073F634BF9E  gateway-dwm3001cdk-rtt.log
SHA256  9AC40F65C265C37CE6FC36067F6D0D118E1ACB244C8DF193C7EC39CF8EF3CB05  node-dwm3001cdk-rtt.log
```

The RTT Viewer files contain legitimate blank records represented as `00> ` and use CRLF line endings. Repository attributes were therefore added to preserve them as raw evidence:

```text
logs/validation/dw3110-identity-init/\*.log -text -diff -whitespace
```

The originally captured CRLF bytes were then written directly into the Git index and committed. This aligned the committed files with the recorded SHA-256 hashes.

## Validated claim

The evidence supports the following claim:

```text
The Thin-Pod Gateway-side and Node-side DWM3001-CDK boards independently established physical SPI communication with their internal DW3110 transceivers, returned Qorvo-driver-matching device identifiers, reached IDLE\_RC and completed DW3110 initialisation successfully.
```

## Remaining boundary

This milestone does not demonstrate:

* UWB RF transmission;
* UWB RF reception;
* a frame exchanged between boards;
* ranging;
* vibration-window transport;
* telemetry transport;
* IRQ-driven receive handling;
* RF performance;
* regulatory compliance.

Both boards reported IRQ high before and after initialisation. IRQ polarity, idle behaviour and event clearing remain subjects for the receive-path milestone.

## Next concrete milestone

The next justified firmware gate is:

```text
one-way UWB frame transmission and reception
```

The intended progression is:

1. configure both DW3110 transceivers with the same bounded PHY settings;
2. place one board in receive mode;
3. transmit a small, deterministic frame from the other board;
4. verify frame length and payload;
5. capture transmit and receive status;
6. record repeatability over multiple exchanges;
7. preserve raw RTT evidence and firmware hashes;
8. defer ranging and vibration-window transport until the basic RF exchange is reliable.

This keeps the Gateway work aligned with the agreed milestone sequence: concrete bring-up evidence first, followed by the smallest defensible RF proof.

## 

