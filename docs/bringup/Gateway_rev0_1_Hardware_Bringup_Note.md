\# Thin-Pod Gateway rev 0.1 Hardware Bring-Up Note



\## Purpose



This note records stable hardware bring-up evidence for the Thin-Pod Gateway rev 0.1 PCB.



This document directly supports Gateway completion checklist item 1: stable hardware bring-up note.



No new Gateway architecture scope is introduced here.



\## Board under test



| Item | Detail |

|---|---|

| Project | Thin-Pod Gateway |

| Hardware revision | rev 0.1 |

| PCB | Thin-Pod Gateway rev 0.1 Gateway PCB |

| Main controller | STM32 NUCLEO-N657X0-Q |

| UWB module | DWM3001-CDK |

| Wi-Fi / auxiliary module | Seeed XIAO ESP32-C6 |

| Power source | NUCLEO powered from PoE splitter |

| Test state | Bench bring-up |



\## Bring-up objective



The objective of this bring-up stage is to confirm that the manufactured Gateway PCB powers the attached development modules correctly and no longer requires a temporary jumper workaround for the DWM3001-CDK ground return.



Required evidence:



\- NUCLEO powers normally from the PoE splitter.

\- Gateway PCB distributes power to the DWM3001-CDK.

\- Gateway PCB distributes 3.3 V power to the XIAO ESP32-C6.

\- Gateway ground continuity is verified.

\- DWM3001-CDK ground return is stable.

\- No jumper wire is required between the CDK J1 negative pin and the ground post after reflow/repair.

\- No unexpected heating, brownout, reset loop, smoke, or unstable power behaviour is observed.



\## Known issue discovered during bring-up



During initial bring-up, the DWM3001-CDK did not power reliably from the Gateway PCB unless an additional jumper was fitted between the CDK J1 negative pin and the ground post.



Further probing indicated that the issue was associated with the CDK ground return, specifically around CDK pin 25 / ground continuity.



The issue was resolved by reflowing the relevant CDK ground connection. After reflow, the DWM3001-CDK powered from the Gateway PCB without the temporary jumper ground.



\## Power checks



| Test point / location | Expected | Observed | Result |

|---|---:|---:|---|

| TP1 / TP2, DWM ground to DWM 5 V | \~5 V | TBD | TBD |

| TP3 / TP4, C6 ground to C6 3.3 V | \~3.3 V | TBD | TBD |

| DWM3001-CDK J10.2 to Gateway ground | \~5 V | TBD | TBD |

| XIAO ESP32-C6 3V3 to GND | \~3.3 V | TBD | TBD |

| Gateway ground to CDK ground pins | Continuity | TBD | TBD |

| Gateway ground to XIAO ground | Continuity | TBD | TBD |



\## Functional observations



| Observation | Expected | Observed | Result |

|---|---|---|---|

| NUCLEO powers from PoE splitter | Board powers normally | TBD | TBD |

| DWM3001-CDK powers from Gateway PCB | CDK powers without jumper workaround | TBD | TBD |

| XIAO ESP32-C6 powers from Gateway PCB | C6 powers normally | TBD | TBD |

| CDK red LED behaviour | Stable expected module behaviour | TBD | TBD |

| C6 amber LED behaviour | Stable expected module behaviour | TBD | TBD |

| Thermal behaviour | No unusual heating | TBD | TBD |

| Reset / brownout behaviour | No repeated reset loop | TBD | TBD |



\## Stable bring-up conclusion



TBD.



\## Completion checklist link



This document satisfies:



\- Item 1: stable hardware bring-up note



It also provides supporting evidence for:



\- Item 7: validation log

\- Item 10: OSHWA certification preparation



\## Revision history



| Date | Change |

|---|---|

| 2026-06-27 | Initial bring-up note created |

