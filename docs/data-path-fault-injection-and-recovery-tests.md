# Thin-Pod Gateway rev 0.1: Data-Path Fault-Injection and Recovery Tests

**Document identifier:** `TP-GW-TEST-0001`<br>
**Document revision:** Draft A<br>
**Product:** Thin-Pod Gateway rev 0.1<br>
**Related node hardware:** Thin-Pod rev 0.1<br>
**Document status:** Verification protocol prepared before firmware/data-path implementation<br>
**Document date:** 27 May 2026<br>
**Related protocol:** `TPHIP-1`, `docs/dwm-to-nucleo-host-interface-protocol.md`<br>
**Certification boundary update (16 July 2026):** Thin-Pod Gateway rev 0.1 is OSHWA-certified under UID [`UK000092`](https://certification.oshwa.org/uk000092.html). This retained verification plan does not expand the certified hardware scope of either Gateway UID `UK000092` or node UID `UK000091`.

## 1. Purpose

This document defines controlled fault-injection and recovery testing for the Thin-Pod measurement-data path.

A measurement system should not merely work when conditions are ideal. It should make failure visible, preserve accepted evidence where possible and refuse to produce apparently valid analysis from corrupt or incomplete input.

The governing principle is:

> **Missing, corrupt, delayed or capacity-blocked vibration records must become explicit logged events rather than silent data loss or silent data substitution.**

## 2. Preconditions

No data-path fault testing should begin until the relevant physical bring-up stages have passed.

| Prerequisite | Evidence required before test execution |
|---|---|
| Gateway PCB visual inspection | Completed arrival/inspection record |
| Unpowered rail/no-short checks | Pass result recorded |
| NUCLEO-only powered rail check | Stable `5V_GATEWAY` / `3V3_GATEWAY` evidence |
| DWM module power/ground validation | Pass result recorded |
| Host-interface signal observability | `CS`, `IRQ`, `RESET` and initial SPI activity observed as intended |
| Golden synthetic record implemented | Known record CRC and byte pattern available |
| Counter/status reporting implemented | Test can read failure and capacity counters |
| Firmware build IDs recorded | Each test result can identify software used |

Where a test involves UWB, a prior wired-only DWM-to-NUCLEO synthetic record test must already have passed.

## 3. Test environment and evidence capture

### 3.1 Required instrumentation

| Tool | Purpose |
|---|---|
| NUCLEO firmware debug/log output | Record queue state, received records, CRC and counter events |
| Gateway DWM firmware debug/log output | Record reception, ready queue, credit and failure state |
| Logic analyser or oscilloscope where available | Capture SPI/IRQ/reset timing during representative tests |
| Deterministic record generator | Repeatable synthetic input and known CRC |
| UWB transmit endpoint for RF tests | Known-fragment and known-record injection |
| Version-controlled evidence folder | Retain logs, screenshots and firmware identifiers |

### 3.2 Evidence folder convention

```text
verification/
└── data-path-fault-tests/
    ├── TEST-REGISTER.md
    ├── F001-golden-transfer/
    ├── F002-withheld-ack/
    ├── F003-bad-record-crc/
    └── ...
```

Each executed test folder should retain:

- firmware build identifiers;
- hardware revision and fitted-module identity;
- test settings;
- input record or generation method;
- logs/counter snapshots before and after;
- trace/screenshots where used; and
- explicit pass/fail assessment.

## 4. Test-status vocabulary

| Status | Meaning |
|---|---|
| `NOT RUN` | Test defined but not executed |
| `BLOCKED` | Precondition not satisfied |
| `PASS` | Expected failure handling and evidence observed |
| `FAIL` | Behaviour diverged from requirement or became unexplained |
| `PASS WITH OBSERVATION` | Pass criterion met; non-blocking anomaly recorded for follow-up |
| `RETEST REQUIRED` | Test result invalidated by setup or firmware correction |

## 5. Counters required for verification

### 5.1 Gateway DWM counters

| Counter | Purpose in fault testing |
|---|---|
| `uwb_windows_granted` | Confirms admission policy |
| `uwb_windows_started` | Confirms reception/reassembly entry |
| `uwb_windows_completed_valid` | Confirms full valid reconstruction |
| `uwb_windows_crc_failed` | Confirms corruption visibility |
| `uwb_windows_timed_out` | Confirms incomplete-window visibility |
| `uwb_busy_responses` | Confirms capacity backpressure |
| `host_records_ready` | Confirms valid record made available to NUCLEO |
| `host_records_acked` | Confirms successful ownership transfer |
| `host_records_rejected` | Confirms host-side rejection path |
| `host_service_timeouts` | Confirms unavailable/slow-host visibility |
| `dwm_resets_detected` | Confirms recovery event history |

### 5.2 NUCLEO counters

| Counter | Purpose in fault testing |
|---|---|
| `records_read` | Counts host-interface record transfers |
| `records_crc_valid` | Confirms accepted valid records |
| `records_crc_failed` | Confirms host-side integrity rejection |
| `sequence_gaps` | Confirms omission visibility |
| `duplicate_sequences` | Confirms duplication visibility |
| `raw_queue_high_water_events` | Confirms buffer-pressure detection |
| `credits_withheld` | Confirms backpressure decision |
| `records_processed_dsp` | Ensures invalid records do not reach processing |
| `records_retained` | Confirms evidence preservation |
| `records_released` | Confirms intentional disposal only |
| `host_interface_recoveries` | Confirms reset/error recovery |
| `xiao_reporting_deferred` | Confirms ingress priority over reporting |

## 6. Safety and integrity rules

1. No powered fault injection may short supply rails or exceed rated module input conditions.
2. Hardware power faults discovered during bring-up take precedence over protocol testing.
3. A test that deliberately corrupts data must be visibly identified as a test record.
4. Corrupt, incomplete or unverified records must never enter the valid DSP input queue.
5. Firmware resets and forced timeouts must be performed in a controlled manner and recorded.
6. An unrepeatable unexplained outcome is a failure until accounted for.

## 7. Phase A: Baseline host-interface tests

These tests establish a valid reference before injecting faults.

### F001 — Golden synthetic record transfer

| Item | Definition |
|---|---|
| Objective | Establish byte-accurate DWM-to-NUCLEO transfer of the 4160-byte golden `SYNTHETIC_WINDOW` record |
| Injection / action | Gateway DWM firmware makes the golden record ready; NUCLEO reads descriptor/payload and sends `ACK_WINDOW` |
| Expected behaviour | Record CRC equals `0x0B1D48B2`; sequence `1` accepted; DWM frees slot only after ACK |
| Required evidence | NUCLEO record dump/hash/CRC result; counter snapshots; representative SPI/IRQ trace where available |
| Pass criterion | 100 consecutive transfers with zero CRC mismatch, unexplained sequence error or retained-slot overwrite |

### F002 — Baseline status/capability repeatability

| Item | Definition |
|---|---|
| Objective | Confirm host can identify protocol and queue state reliably |
| Action | Read `GET_CAPABILITIES` and `GET_STATUS` repeatedly at the commissioning SPI setting |
| Expected behaviour | Stable protocol version, maximum record size, slot count and no unexplained error flags |
| Pass criterion | 100 repeated capability/status cycles without malformed response or response CRC failure |

## 8. Phase B: Ownership, acknowledgement and backpressure tests

### F010 — Withheld acknowledgement

| Item | Definition |
|---|---|
| Objective | Demonstrate that DWM does not overwrite a completed record while host ownership is unresolved |
| Injection | NUCLEO reads or observes one ready golden record but deliberately withholds `ACK_WINDOW` |
| Expected behaviour | Record remains `READY_FOR_HOST` or explicitly transfer-pending; no slot release; no admission of replacement record beyond configured credit policy |
| Evidence | DWM status before/during/after timeout interval; unchanged sequence/CRC upon reread |
| Pass criterion | Original record remains recoverable and no silent replacement occurs |

### F011 — Explicit host rejection

| Item | Definition |
|---|---|
| Objective | Confirm rejected records do not enter valid processing |
| Injection | NUCLEO sends `REJECT_WINDOW` for a synthetically marked record |
| Expected behaviour | DWM increments rejection/event counter and releases only according to policy; NUCLEO does not increment `records_processed_dsp` |
| Pass criterion | Rejection is logged with record sequence and reason; no valid-processing entry exists |

### F012 — Credit zero / no admission

| Item | Definition |
|---|---|
| Objective | Demonstrate the most basic upstream flow-control rule |
| Injection | NUCLEO sends `SET_CREDITS(0)` while no record is in flight; node/test source attempts to offer a new record |
| Expected behaviour | Gateway DWM reports no capacity/busy or withholds grant; no record becomes valid/ready |
| Pass criterion | No new complete record is accepted; busy/withheld-admission counter increments predictably |

### F013 — NUCLEO high-water threshold

| Item | Definition |
|---|---|
| Objective | Confirm queue pressure withdraws admission before raw-record overwrite |
| Injection | Fill or simulate NUCLEO raw queue occupancy to the configured high-water threshold |
| Expected behaviour | `credits_withheld` increments; new admissions stop before the protected reserve is consumed |
| Pass criterion | No `VALID_RAW` record is overwritten; admission resumes only after capacity is deliberately released |

### F014 — DWM queue full

| Item | Definition |
|---|---|
| Objective | Confirm Gateway DWM local capacity is enforced |
| Injection | Occupy both initial DWM record slots through a controlled test condition before acknowledging/freeing them |
| Expected behaviour | Further record admission is refused/withheld; `uwb_busy_responses` or equivalent event increments |
| Pass criterion | Existing queued records remain unmodified and readable; no third record silently replaces one |

## 9. Phase C: Frame and integrity-error tests

### F020 — Corrupt host request CRC

| Item | Definition |
|---|---|
| Objective | Ensure state-changing commands are not accepted when host request integrity fails |
| Injection | Send a `SET_CREDITS` or other command with incorrect `request_crc32` |
| Expected behaviour | DWM returns `ERR_BAD_CRC`; no credit or queue state change occurs |
| Pass criterion | State before and after request is identical apart from diagnostic counter/event |

### F021 — Corrupt host response / transfer data

| Item | Definition |
|---|---|
| Objective | Confirm NUCLEO rejects a record failing host-side CRC |
| Injection | Development firmware deliberately alters one payload byte or CRC before NUCLEO validation |
| Expected behaviour | NUCLEO increments `records_crc_failed`, does not ACK as valid, does not submit to DSP |
| Pass criterion | Corrupt record is visible as invalid and no valid derived output is produced |

### F022 — Incorrect record length

| Item | Definition |
|---|---|
| Objective | Confirm malformed descriptor cannot trigger unsafe read or processing |
| Injection | Set `payload_bytes` or `record_total_bytes` inconsistently with the initial 4160-byte record contract |
| Expected behaviour | NUCLEO rejects descriptor before treating payload as valid; diagnostic reason logged |
| Pass criterion | No over-read, buffer write outside allocated slot or valid-DSP submission |

### F023 — Unsupported protocol version

| Item | Definition |
|---|---|
| Objective | Establish safe version mismatch behaviour |
| Injection | Send request or expose record using unsupported protocol/record version |
| Expected behaviour | Interface returns version fault; no measurement record is accepted |
| Pass criterion | Compatibility fault explicitly visible; system returns safely to compatible mode or requires deliberate recovery |

### F024 — Sequence duplication

| Item | Definition |
|---|---|
| Objective | Ensure duplicate records become visible |
| Injection | Present two records with identical `node_id` and `window_sequence` |
| Expected behaviour | First may be accepted; second is marked duplicate or accepted only with explicit duplicate status according to test policy |
| Pass criterion | `duplicate_sequences` or equivalent event records the condition; no silent double-counting in DSP log |

### F025 — Sequence gap

| Item | Definition |
|---|---|
| Objective | Ensure missing windows can be identified |
| Injection | Present sequence `n`, then sequence `n+2` |
| Expected behaviour | `sequence_gaps` increments and the gap is logged; valid record `n+2` may be retained with a missing-predecessor status |
| Pass criterion | Missing record is visible in provenance/error log |

## 10. Phase D: UWB fragmentation and reception tests

These tests begin only after wired host-interface tests pass.

### F030 — Known UWB record receipt

| Item | Definition |
|---|---|
| Objective | Establish a baseline UWB-to-host valid path |
| Injection / action | Node-side firmware transmits one known `UWB_TEST_WINDOW` record through the defined fragment path |
| Expected behaviour | Gateway DWM reconstructs, validates and presents record; NUCLEO accepts exact known record |
| Pass criterion | Matching expected CRC and sequence; logged RF-to-host latency |

### F031 — Missing fragment

| Item | Definition |
|---|---|
| Objective | Ensure an incomplete UWB record cannot become valid raw measurement |
| Injection | Omit one fragment from a known record |
| Expected behaviour | Reassembly timeout/incomplete event; no `READY_FOR_HOST` valid measurement; no DSP input |
| Pass criterion | `uwb_windows_timed_out` increments and missing record is explicit |

### F032 — Duplicate fragment

| Item | Definition |
|---|---|
| Objective | Confirm deterministic handling of duplicates |
| Injection | Send one fragment twice within a test window |
| Expected behaviour | Duplicate recognised/handled deterministically; completed record either remains valid if policy allows or is rejected explicitly |
| Pass criterion | Duplicate event counter/log updated; no unexplained payload change |

### F033 — Bad whole-window UWB CRC

| Item | Definition |
|---|---|
| Objective | Confirm record integrity is checked after reconstruction |
| Injection | Deliver all fragments but alter data or stored expected CRC |
| Expected behaviour | Gateway DWM rejects full record; does not expose valid record to NUCLEO |
| Pass criterion | UWB CRC failure recorded and valid-record count unchanged |

### F034 — Radio receipt attempted without credit

| Item | Definition |
|---|---|
| Objective | Confirm upstream policy violation is visible |
| Injection | Node test firmware transmits when the Gateway has advertised no capacity |
| Expected behaviour | Protocol violation/busy event recorded; record either rejected or quarantined according to defined policy, never silently treated as normal admitted measurement |
| Pass criterion | No unrecorded admission occurs |

### F035 — Retransmission policy test

| Item | Definition |
|---|---|
| Objective | Establish whether and how one failed fragment/window may be retried |
| Injection | Create one controlled recoverable loss under a selected retransmission rule |
| Expected behaviour | Retransmitted record retains identity plus retry metadata; successful final record marked as retransmitted |
| Pass criterion | Retry behaviour and added latency measurable and visible in record provenance |

## 11. Phase E: Reset, timeout and subsystem-availability tests

### F040 — DWM reset with no in-flight record

| Item | Definition |
|---|---|
| Objective | Confirm normal restart discovery path |
| Injection | Assert controlled DWM reset while no window is pending |
| Expected behaviour | NUCLEO observes reset/reconnect state, rereads capabilities/status and resumes only after explicit readiness |
| Pass criterion | No false valid record; reset event logged |

### F041 — DWM reset during host transfer

| Item | Definition |
|---|---|
| Objective | Confirm partial record cannot become accepted evidence |
| Injection | Assert DWM reset while NUCLEO is reading a synthetic record payload |
| Expected behaviour | NUCLEO rejects in-progress slot; records interface/reset fault; no DSP input created |
| Pass criterion | Partial record remains invalid and failure is visible |

### F042 — NUCLEO reset with DWM ready record retained

| Item | Definition |
|---|---|
| Objective | Determine safe recovery of DWM-owned complete record after host restart |
| Injection | Reset NUCLEO while DWM retains one unacknowledged ready synthetic record |
| Expected behaviour | Upon restart, host rereads status and either retrieves/ACKs the record or explicitly rejects/logs it; DWM does not silently discard it before policy timeout |
| Pass criterion | Outcome is deterministic and recorded |

### F043 — Host service timeout

| Item | Definition |
|---|---|
| Objective | Confirm communications subsystem fails safe when NUCLEO stops servicing ready data |
| Injection | Leave one DWM ready record pending beyond configured service deadline |
| Expected behaviour | DWM raises timeout/error status and stops granting further measurements; original record retained or released only under documented policy |
| Pass criterion | No continuing uncontrolled admission and timeout counter increments |

## 12. Phase F: DSP and optional backhaul isolation tests

### F050 — CRC-failed record blocked from DSP

| Item | Definition |
|---|---|
| Objective | Prove analytical results cannot be generated from knowingly invalid input |
| Injection | Present a corrupt record via the host test path |
| Expected behaviour | No RMS, peak, crest factor, kurtosis or later model result generated for that record as valid data |
| Pass criterion | DSP result log has no accepted entry linked to invalid record; rejection log exists |

### F051 — DSP consumer deliberately slowed

| Item | Definition |
|---|---|
| Objective | Confirm slow analysis invokes backpressure rather than raw-record loss |
| Injection | Add controlled processing delay until NUCLEO queue reaches high-water condition |
| Expected behaviour | Credits withheld; `raw_queue_high_water_events` increments; accepted raw records retained |
| Pass criterion | No silent overwrite; measured rate-limit transition recorded |

### F052 — XIAO reporting blocked or delayed

| Item | Definition |
|---|---|
| Objective | Confirm optional networking remains downstream of measurement ingress |
| Injection | Simulate long/blocking XIAO reporting activity while valid DWM records arrive |
| Expected behaviour | Ingress task pre-empts or defers reporting; `xiao_reporting_deferred` increments where implemented |
| Pass criterion | No increase in unexplained record loss attributable to reporting path |

### F053 — TinyML not permitted before provenance-complete valid record

| Item | Definition |
|---|---|
| Objective | Confirm later inference cannot detach from input validity/provenance |
| Injection | Supply missing-version or invalid-integrity test record to any experimental inference route |
| Expected behaviour | Model processing refused or explicitly labelled invalid/test-only |
| Pass criterion | No apparently operational classification generated from unqualified data |

## 13. Test register template

| Test ID | Test name | Firmware builds | Hardware build | Settings | Date | Result | Evidence location | Notes/action |
|---|---|---|---|---|---|---|---|---|
| F001 | Golden synthetic record transfer |  | Gateway rev 0.1 / ordered build `rev 0.1f` if used |  |  | NOT RUN |  |  |
| F002 | Baseline status/capability repeatability |  |  |  |  | NOT RUN |  |  |
| F010 | Withheld acknowledgement |  |  |  |  | NOT RUN |  |  |
| F011 | Explicit host rejection |  |  |  |  | NOT RUN |  |  |
| F012 | Credit zero / no admission |  |  |  |  | NOT RUN |  |  |
| F013 | NUCLEO high-water threshold |  |  |  |  | NOT RUN |  |  |
| F014 | DWM queue full |  |  |  |  | NOT RUN |  |  |
| F020 | Corrupt host request CRC |  |  |  |  | NOT RUN |  |  |
| F021 | Corrupt host response / record data |  |  |  |  | NOT RUN |  |  |
| F022 | Incorrect record length |  |  |  |  | NOT RUN |  |  |
| F023 | Unsupported protocol version |  |  |  |  | NOT RUN |  |  |
| F024 | Sequence duplication |  |  |  |  | NOT RUN |  |  |
| F025 | Sequence gap |  |  |  |  | NOT RUN |  |  |
| F030 | Known UWB record receipt |  |  |  |  | NOT RUN |  |  |
| F031 | Missing fragment |  |  |  |  | NOT RUN |  |  |
| F032 | Duplicate fragment |  |  |  |  | NOT RUN |  |  |
| F033 | Bad whole-window UWB CRC |  |  |  |  | NOT RUN |  |  |
| F034 | Receipt attempted without credit |  |  |  |  | NOT RUN |  |  |
| F035 | Retransmission policy |  |  |  |  | NOT RUN |  |  |
| F040 | DWM reset without in-flight record |  |  |  |  | NOT RUN |  |  |
| F041 | DWM reset during transfer |  |  |  |  | NOT RUN |  |  |
| F042 | NUCLEO reset with DWM record retained |  |  |  |  | NOT RUN |  |  |
| F043 | Host service timeout |  |  |  |  | NOT RUN |  |  |
| F050 | Invalid record blocked from DSP |  |  |  |  | NOT RUN |  |  |
| F051 | Deliberately slow DSP consumer |  |  |  |  | NOT RUN |  |  |
| F052 | XIAO reporting blocked/delayed |  |  |  |  | NOT RUN |  |  |
| F053 | TinyML provenance/integrity gate |  |  |  |  | NOT RUN |  |  |

## 14. Release evidence threshold

During pre-certification planning, the following threshold separated the open-hardware release evidence from later system-level analytics work. OSHWA certification UID `UK000092` was granted on 16 July 2026; the live certification scope and completed evidence record now govern the release.

The minimum evidence used for the Gateway rev 0.1 certification preparation was:

| Evidence class | Recommended minimum |
|---|---|
| Physical carrier bring-up | Power, ground and module-interface tests passed |
| Host-interface baseline | F001 and F002 passed |
| Ownership/overrun safety | F010, F012 and F013 passed or an equivalent evidence record exists |
| Source/BOM/fabrication reconciliation | Completed |
| Provenance/licence boundary | Completed |
| UWB/DSP/TinyML tests | May remain future milestones if not claimed in certified scope |

## 15. References

- `docs/dwm-to-nucleo-host-interface-protocol.md`.
- `docs/data-path-flow-control-and-buffering.md`.
- `docs/window-throughput-and-airtime-budget.md`.
- `docs/measurement-record-and-provenance-model.md`.
- `docs/gateway-bring-up-and-verification-protocol.md`.
- Qorvo, [DWM3001C product information](https://www.qorvo.com/products/p/DWM3001C).
- Qorvo, [DWM3001CDK development kit product information](https://www.qorvo.com/products/p/DWM3001CDK).
- STMicroelectronics, [STM32N657X0 product information](https://www.st.com/en/microcontrollers-microprocessors/stm32n657x0.html).

## 16. Engineering decision

The first failure tests are not optional polish. They are the proof that the system’s central claim of measurement integrity has substance: a buffer that is corrupt, unacknowledged, incomplete, duplicated, capacity-blocked or interrupted by reset must remain identifiable as such and must never silently become a valid analytic input.
