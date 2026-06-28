# Thin-Pod Memfault / nRF Cloud Evaluation

## Document status

**Project:** Thin-Pod  
**Document type:** Observability / device-management evaluation  
**Proposed repository path:** `docs/observability/Thin-Pod_Memfault_nRFCloud_Evaluation.md`  
**Proposed branch:** `memfault-nrfcloud-evaluation`  
**Status:** Draft for review and possible repository inclusion  

---

## Executive conclusion

Memfault / nRF Cloud is suitable as an optional Nordic / Zephyr observability and OTA layer for Thin-Pod nodes and Gateway-side Nordic modules. It should be used for firmware health, reboot diagnostics, crash capture, update history and fleet-level operational metrics. It should not replace the project’s open telemetry, DSP feature extraction, vibration-data logging or predictive-maintenance analysis pipeline.

In practical terms, Memfault is a good fit for making Thin-Pod more reliable, inspectable and maintainable in the field. It is not the correct place to host or define the core vibration-analysis workflow.

---

## Why this is relevant to Thin-Pod

Thin-Pod already has a natural point of contact with the Nordic ecosystem through the Qorvo DWM3001C / DWM3001CDK hardware path. The DWM3001C combines UWB capability with a Nordic nRF52833-class microcontroller, which makes Nordic Connect SDK, Zephyr and associated device-management tooling relevant to the project.

The current Thin-Pod architecture is deliberately modular:

- the sensor node captures vibration data;
- DSP / TinyML logic extracts or classifies condition-monitoring features;
- UWB provides local wireless transport;
- the Gateway aggregates, logs or forwards telemetry;
- the repository preserves the open hardware, interface and validation evidence.

Memfault / nRF Cloud should sit beside this architecture as an optional device-observability layer. It should not become a dependency of the core open-source hardware demonstration.

---

## Suitable role for Memfault / nRF Cloud

Memfault is best suited to answering questions such as:

- Did the node reboot unexpectedly?
- Was the reboot caused by watchdog, brownout, assertion failure or firmware update?
- Which firmware version is running in the field?
- Are particular builds producing more crashes or faults?
- Are UWB communication failures increasing?
- Are sample windows being dropped?
- Are SPI, ADC, timing or buffer errors occurring?
- Is battery voltage or supply stability degrading?
- Is the Gateway seeing stale or intermittent node traffic?
- Can a firmware update be deployed and tracked safely?

This is highly relevant to Thin-Pod because predictive-maintenance hardware must itself be maintainable. A condition-monitoring device that cannot report its own health is harder to trust in field trials.

---

## Unsuitable role for Memfault / nRF Cloud

Memfault should not be treated as the main Thin-Pod predictive-maintenance backend.

It should not replace:

- raw vibration acquisition logs;
- extracted DSP feature records;
- FFT-band or envelope-analysis output;
- labelled bearing, imbalance or misalignment datasets;
- local CSV / JSON export;
- reproducible bench logs;
- Gateway telemetry records;
- model-training data;
- open validation artefacts;
- the project’s own data schema for condition-monitoring evidence.

The Thin-Pod analytics path should remain open, inspectable and reproducible without requiring a proprietary cloud service.

---

## Recommended first integration scope

The first Thin-Pod use of Memfault / nRF Cloud should be deliberately small.

Recommended initial scope:

1. Create a separate evaluation branch.
2. Add a short observability document to the repository.
3. Build a minimal Zephyr / Nordic Connect SDK firmware target for the DWM3001CDK or equivalent Nordic-based module.
4. Enable only basic diagnostic features:
   - reboot reason;
   - firmware version;
   - uptime;
   - trace events;
   - fault / assert capture if available;
   - a small custom metric set.
5. Avoid OTA until the core UWB telemetry path is stable.
6. Keep all credentials, project keys and cloud identifiers outside the repository.
7. Preserve a fully local, non-cloud reproducibility path.

The key principle is that Memfault should be introduced as an observability experiment, not as a platform migration.

---

## Suggested Thin-Pod metric dictionary

A Thin-Pod-specific Memfault metric set could include:

| Metric | Purpose |
|---|---|
| `thinpod_uptime_s` | Device uptime since last boot |
| `thinpod_reboot_reason` | Encoded reboot cause |
| `thinpod_watchdog_count` | Watchdog reset count |
| `thinpod_battery_mv` | Battery or local supply voltage |
| `thinpod_sensor_power_cycle_count` | Number of controlled sensor power cycles |
| `thinpod_adc_overrun_count` | Missed or overrun ADC sample windows |
| `thinpod_spi_error_count` | SPI transaction failures |
| `thinpod_uwb_tx_ok_count` | Successful UWB transmissions |
| `thinpod_uwb_rx_timeout_count` | UWB receive timeouts |
| `thinpod_packet_drop_count` | Dropped telemetry packets |
| `thinpod_feature_window_ms` | Time required to process one feature window |
| `thinpod_feature_queue_depth` | Pending feature or telemetry queue depth |
| `thinpod_gateway_seen_age_s` | Age of last successful Gateway contact |
| `thinpod_fw_mode` | Current firmware operating mode |
| `thinpod_build_id` | Firmware build identifier |

These metrics should be kept small, bounded and operational. They are not a replacement for the main vibration dataset.

---

## Suggested event traces

Useful trace events could include:

- sensor power enabled;
- sensor power disabled;
- ADC acquisition started;
- ADC acquisition window completed;
- ADC overrun detected;
- DSP feature window completed;
- UWB ranging / transfer started;
- UWB transfer completed;
- UWB transfer failed;
- Gateway acknowledgement received;
- Gateway acknowledgement timeout;
- configuration update applied;
- firmware fault detected;
- safe-mode entered.

These events would help reconstruct device behaviour without uploading high-volume vibration data.

---

## OTA update position

OTA firmware update capability is attractive, but it should be treated as a later-stage feature.

For Thin-Pod, OTA should come after:

1. stable local firmware builds;
2. repeatable flashing and debug workflow;
3. stable UWB node-to-Gateway communication;
4. documented telemetry schema;
5. reliable local logging;
6. basic observability metrics;
7. known-good rollback or recovery behaviour.

Premature OTA work could distract from the more important Stage-2 evidence: a working node-to-Gateway telemetry path with logged and validated results.

---

## Open-source hardware boundary

Memfault / nRF Cloud is a proprietary cloud/device-management service. That does not prevent Thin-Pod from remaining open-source hardware, but it does mean the integration should be framed carefully.

The repository should make clear that:

- Thin-Pod hardware can be built, inspected and tested without Memfault;
- local serial, RTT, CSV, JSON or Gateway logs remain the open reproducibility path;
- Memfault / nRF Cloud is optional;
- credentials and project keys must never be committed;
- cloud-specific configuration should be documented separately from core hardware files;
- the core predictive-maintenance evidence should remain portable.

Recommended wording:

> Thin-Pod can be evaluated without Memfault / nRF Cloud. Memfault / nRF Cloud is an optional fleet-observability and OTA layer for Nordic / Zephyr firmware builds. It is used to inspect firmware and fleet health, not to define the core vibration-analysis pipeline.

---

## Gateway implications

The current Thin-Pod Gateway concept should not be redesigned around Memfault.

The Gateway should remain responsible for:

- receiving node telemetry;
- preserving the UWB transport boundary;
- logging feature records;
- exporting data for analysis;
- supporting field validation;
- remaining independent of any single commercial observability service.

A later Gateway revision could optionally act as a relay for Memfault diagnostic data, but that should not be a prerequisite for the rev 0.1 Gateway proof.

---

## Recommended repository addition

Add this document at:

```text
docs/observability/Thin-Pod_Memfault_nRFCloud_Evaluation.md
```

Recommended branch name:

```text
memfault-nrfcloud-evaluation
```

Recommended commit message:

```text
Evaluate Memfault nRF Cloud observability fit
```

---

## Decision

Memfault / nRF Cloud is a suitable optional addition to the Thin-Pod project when used for device reliability and field observability.

It is especially useful for:

- Nordic / Zephyr firmware health;
- reboot diagnostics;
- crash capture;
- update history;
- small operational metrics;
- pilot fleet monitoring;
- evidence of field maintainability.

It is not suitable as the primary home for:

- vibration waveform data;
- DSP analysis;
- TinyML model evidence;
- rotating-equipment diagnostic records;
- open validation datasets;
- core predictive-maintenance telemetry.

The best Thin-Pod position is therefore:

> Use Memfault / nRF Cloud to observe the device. Use Thin-Pod’s own open telemetry and analysis stack to observe the machine.

---

## Next practical step

Create the evaluation branch, add this document, commit it, and push the branch for review.

Do not modify the working Stage-2 firmware path until the document has been reviewed and the role of Memfault / nRF Cloud has been accepted as optional and bounded.
