\# UWB RF Proof Stage 2

> **Current gated milestone, 12 July 2026:** the first physical RF proof is a
> bounded one-way frame exchange from the Gateway DWM3001-CDK to the Node
> DWM3001-CDK. The return path, ranging and vibration-window transport remain
> later milestones. The implementation procedure is recorded in
> `DW3110_One_Way_RF_Exchange_Procedure.md`.



This document tracks the Stage-2 Thin-Pod UWB RF proof for the Gateway and node DWM3001-CDK firmware.



Stage 1 proved that the Gateway and node firmware roles build under nRF Connect SDK v3.3.1, flash via the J-Link runner, boot under Zephyr and emit structured SEGGER RTT logs using a stub backend.



Stage 2 aims to prove live DW3110 over-the-air exchange between the Gateway and node DWM3001-CDKs.



\## Goal



Prove the minimum RF path:



```text

Gateway firmware

&#x20;   -> DW3110 radio

&#x20;   -> UWB antenna

&#x20;   -> air

&#x20;   -> UWB antenna

&#x20;   -> DW3110 radio

&#x20;   -> node firmware

```



and the return path:



```text

Node firmware

&#x20;   -> DW3110 radio

&#x20;   -> UWB antenna

&#x20;   -> air

&#x20;   -> UWB antenna

&#x20;   -> DW3110 radio

&#x20;   -> Gateway firmware

```



\## Intended role mapping



```text

Gateway CDK 760203854 -> gateway\_uwb\_initiator\_stage2\_rf

Node CDK    760222856 -> node\_uwb\_responder\_stage2\_rf

```



\## Intended RF evidence



The Gateway RTT log should eventually report events such as:



```text

TPGW\_RF\_INIT

TPGW\_RF\_POLL\_TX

TPGW\_RF\_RESP\_RX

```



The node RTT log should eventually report events such as:



```text

TPNODE\_RF\_INIT

TPNODE\_RF\_POLL\_RX

TPNODE\_RF\_RESP\_TX

```



The RTT records are evidence logs only. The RF payload itself should travel over the DW3110 UWB radio path.



\## Boundary



This stage is intended to prove live UWB RF packet exchange.



It does not yet claim full vibration-window transport, DSP, TinyML, industrial diagnostic performance or production condition-monitoring behaviour.



\## Relationship to Stage 1



Stage 1 verified:



```text

Zephyr build -> J-Link flash -> board boot -> RTT role log -> packet-format evidence

```



Stage 2 should verify:



```text

Gateway firmware -> DW3110 RF transmit -> over-the-air exchange -> DW3110 RF receive -> node/Gateway RTT evidence

```



The Stage-1 stub backend should remain preserved as a known-good firmware/logging baseline.









