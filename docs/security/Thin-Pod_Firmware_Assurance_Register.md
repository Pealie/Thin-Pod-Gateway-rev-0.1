# Thin-Pod Firmware Assurance Register

**Document identifier:** `TP-SYS-SEC-0003`
**Revision:** Draft A
**Date:** 11 July 2026
**Status:** Living firmware inventory and assurance register
**Applies to:** Thin-Pod node DWM, Gateway DWM, STM32 NUCLEO, optional XIAO ESP32-C6 and host-side firmware tooling
**Evidence baseline:** Gateway commit `a2c8b846d1a67bcbf2f612d94cc7d17feea107dc`

## 1. Purpose

This register records the firmware and toolchain identity, SBOM status, signing policy, debug state, update and recovery position, advisory monitoring and supported-update commitment for every Thin-Pod firmware domain.

It is intended to answer:

```text
What firmware is running?
Which dependencies produced it?
Was the image signed and was that signature enforced?
How can the component recover?
Which debug interfaces remain open?
Which advisories apply?
How long will this release receive security updates?
```

The register must be updated whenever a component, SDK, board target, security profile or release commitment changes.

## 2. Status terms

| Status | Meaning |
|---|---|
| Demonstrated | Evidence exists in the repository or bench logs |
| Partial | A supporting mechanism exists, while the security property remains incomplete |
| Planned | Requirement and intended direction are recorded |
| Open | Decision or implementation remains unresolved |
| Development-only | Suitable for controlled laboratory use; no deployment claim |
| Release gate | Must be closed before external pilot or supported release |

## 3. Current component inventory

| Domain | Hardware / MCU | Current firmware basis | Board target or build identity | Update path | Boot/signing assurance | Debug state | SBOM | Support status |
|---|---|---|---|---|---|---|---|---|
| Node UWB subsystem | Qorvo DWM3001-CDK, nRF52833 + DW3110 | nRF Connect SDK v3.3.1 / Zephyr | `decawave_dwm3001cdk`; Stage-1 node responder and alive-test applications | J-Link wired flash | Secure boot and protected trust anchor unproved | J-Link, RTT and development access enabled | Not generated | Development-only |
| Gateway UWB subsystem | Qorvo DWM3001-CDK, nRF52833 + DW3110 | nRF Connect SDK v3.3.1 / Zephyr | `decawave_dwm3001cdk`; Stage-1 Gateway initiator, host-interface stub and diagnostic applications | J-Link wired flash | Secure boot and protected trust anchor unproved | J-Link, RTT and development access enabled | Not generated | Development-only |
| Gateway host / analysis supervisor | STM32 NUCLEO-N657X0-Q, STM32N657 | Upstream Zephyr development tree used for current bring-up | Runtime evidence: `v4.4.0-7558-g9a27e2e135af`; board `nucleo_n657x0_q` | ST-LINK and STM32CubeProgrammer | Build signing tool used; target-enforced secure boot unproved | ST-LINK, serial and development access enabled | Not generated | Development-only |
| Optional onward networking | Seeed Studio XIAO ESP32-C6 | No Thin-Pod networking firmware committed in examined snapshot | To be assigned | Wired development flash; signed A/B OTA planned | Secure Boot v2 and flash encryption planned | Development default; deployment policy open | Not generated | Not implemented |
| Host packet tooling | Python parsers, loggers and validation scripts | Repository scripts | Repository commit and script hash | Source-controlled update | Release hash and signed release process open | Host OS controls | Python dependency SBOM not generated | Development tooling |

## 4. Recorded toolchain evidence

| Tool or dependency | Recorded version or identity | Evidence source | Assurance note |
|---|---|---|---|
| nRF Connect SDK | `v3.3.1` | Engineering log 85 and firmware baseline documents | Pin exact manifest revisions in future release metadata |
| nRF Connect SDK runtime identity | `v3.3.1-1d7a0b0e49b8` | Gateway DWM runtime log | Runtime identity should be retained in release evidence |
| west | `v1.5.0` | Engineering log 85 | Record with each reproducible build environment |
| Zephyr SDK for DWM builds | `0.17.0` | Engineering log 85 and RTT baseline | Toolchain SBOM and compiler identity still required |
| GCC for DWM builds | Zephyr SDK GCC `12.2.0` | RTT baseline | Record build flags and binary hashes |
| Ninja for DWM build environment | `1.13.2` | Engineering log 85 | Build tool only; pin in environment manifest |
| CMake for DWM build environment | `3.25.0` | Engineering log 85 | Build tool only; pin in environment manifest |
| J-Link tools | `9.50` recorded for Stage-1 | Engineering log 85 | Later logs may use a different installed version; record per build/flash session |
| NUCLEO Zephyr runtime | `v4.4.0-7558-g9a27e2e135af` | Engineering log 90 and TP6 RTT evidence | Development snapshot; formal release tag remains open |
| Zephyr SDK for NUCLEO work | `1.0.1` host/toolchain installation recorded | Engineering log 90 | Record exact compiler version in release manifest |
| STM32CubeProgrammer | Installed and used successfully; exact version not recorded in examined evidence | Engineering log 90 | Exact version is a release-gate item |
| ESP-IDF | Not selected | No committed XIAO firmware | Select and pin before networking work |

Versions above describe the examined evidence. They do not establish current vendor support or absence of vulnerabilities.

## 5. SBOM policy and target locations

Every tagged firmware release candidate MUST generate an SBOM for source and binary dependencies.

Preferred formats:

- SPDX JSON;
- CycloneDX JSON where required by a downstream tool;
- a human-readable dependency summary.

Target repository locations:

```text
artifacts/sbom/node-dwm/<release>/node-dwm.spdx.json
artifacts/sbom/gateway-dwm/<release>/gateway-dwm.spdx.json
artifacts/sbom/nucleo/<release>/nucleo.spdx.json
artifacts/sbom/xiao-esp32c6/<release>/xiao-esp32c6.spdx.json
artifacts/sbom/host-tools/<release>/host-tools.spdx.json
```

Each SBOM entry set MUST identify:

- component name and version;
- source repository or package origin;
- licence;
- direct or transitive relationship;
- repository commit or package digest where available;
- generated firmware artefact hash;
- SBOM tool and version;
- generation date;
- unresolved proprietary or vendor binary component.

Current status: **Open release gate. No firmware SBOM is present in the examined snapshot.**

## 6. Firmware signing policy

### 6.1 General rules

1. Each firmware domain uses a separate signing key or delegated signing authority.
2. Development keys remain distinct from pilot and production keys.
3. Private signing keys never enter the source repository, CI logs, firmware image or routine developer workstation backup.
4. Public verification keys or digests may be stored in the device trust anchor and repository release metadata.
5. Every signed image records component, version, security version, build identity and artefact hash.
6. Revocation and key rotation must be tested before external deployment.
7. Signature generation and target enforcement are recorded as separate evidence.

### 6.2 Current component status

| Domain | Current signing position | Required next proof |
|---|---|---|
| Node DWM | No enforced signed-boot proof | Valid signed image boots; corrupted and unknown-key images fail |
| Gateway DWM | No enforced signed-boot proof | Valid signed image boots; corrupted and unknown-key images fail |
| NUCLEO | STM32 signing tool used during Zephyr build | Demonstrate target root of trust and rejection of altered/unknown-key image |
| XIAO ESP32-C6 | No firmware baseline | Select Secure Boot v2 signing profile and prove A/B OTA |
| Host tools | Git commit history only | Release hashes and optional signed release manifest |

Current status: **Development-only. No production signing key exists.**

## 7. Debug policy

| Profile | Node DWM | Gateway DWM | NUCLEO | XIAO ESP32-C6 |
|---|---|---|---|---|
| Laboratory | J-Link/RTT enabled | J-Link/RTT enabled | ST-LINK/serial enabled | USB/JTAG/UART as required |
| Controlled pilot | Physical access restricted; debug events logged | Physical access restricted; debug events logged | Physical access restricted; secrets isolated | Secure download settings planned |
| Deployment candidate | Locked or authenticated debug; recovery procedure proved | Locked or authenticated debug; recovery procedure proved | Locked or authenticated debug; recovery procedure proved | JTAG/UART download restrictions applied |
| Returned-device analysis | Authorised service process | Authorised service process | Authorised service process | Authorised service process |

Current status: **Laboratory profile.**

Debug restrictions must follow successful verified-boot, recovery and key-provisioning tests.

## 8. Update and recovery register

| Domain | Current method | Required deployed method | Recovery evidence | Anti-rollback |
|---|---|---|---|---|
| Node DWM | J-Link full flash | Signed image with revertible or recoverable boot path | Open | Open |
| Gateway DWM | J-Link full flash | Signed image with revertible or recoverable boot path | Open | Open |
| NUCLEO | ST-LINK / STM32CubeProgrammer | Verified signed replacement with rollback and wired recovery | Basic wired flash demonstrated; interrupted-update recovery open | Open |
| XIAO ESP32-C6 | None in repo | Signed A/B OTA with first-boot confirmation and wired rescue | Open | Planned |
| Host tools | Git pull or release download | Hashed, versioned release package | Git history available | Version selection by operator |

An update path is not deployment-ready until power interruption, malformed image, unknown key and downgrade tests have passed.

## 9. Protocol assurance register

| Protocol | Present protection | Security classification | Required next revision |
|---|---|---|---|
| Telemetry Packet v1 | CRC16 | Laboratory parser and transport test only | New authenticated version with identity, epoch, sequence, key ID and tag |
| DWM-to-NUCLEO Draft A | Request sequence and CRC16 | Laboratory host-interface proof | Secure record envelope, preserved authentication status and replay semantics |
| Stage-1 UWB role harness | Stub transport | Firmware identity and logging evidence only | Real RF backend plus application authentication and replay tests |
| NUCLEO-to-XIAO | Not defined | No security claim | Narrow allowlisted protocol with authorised control and preserved provenance |
| Backend transport | Not defined | No security claim | Outbound mTLS, per-device credential and lifecycle policy |

Normative requirements are in `docs/security/Thin-Pod_Protocol_Security_Requirements.md`.

## 10. Vulnerability advisory monitoring

The following sources must be reviewed for each release candidate and at a defined maintenance interval:

| Component | Advisory sources |
|---|---|
| Zephyr | Zephyr security advisories, CVE records and project PSIRT notices |
| nRF Connect SDK / nRF52833 | Nordic security advisories, release notes and dependency manifest |
| Qorvo DW3110 / DWM3001-CDK | Qorvo product notices, release notes and support advisories |
| STM32N6 / STM32CubeProgrammer | ST security advisories and product notices |
| ESP-IDF / ESP32-C6 | Espressif security advisories and ESP-IDF release notes |
| Python host tools | Python Security Response Team, package advisories and lockfile audit |
| GitHub Actions | Action release and security advisories; actions pinned to immutable revisions for release workflows |

Each advisory decision must record:

```text
advisory identifier
publication date
affected component and version
Thin-Pod applicability
exposure path
severity and rationale
mitigation
target release
verification evidence
decision owner
review date
```

Current status: **Manual and incomplete. A formal advisory log is a release gate.**

Suggested location:

```text
docs/security/Thin-Pod_Vulnerability_Advisory_Log.md
```

## 11. Vulnerability-reporting contact

Preferred reporting route:

```text
GitHub private vulnerability reporting / private security advisory
Repository: Pealie/Thin-Pod-Gateway-rev-0.1
Owner: @Pealie
```

Fallback:

```text
Open a minimal public issue titled "Security contact request"
Do not include exploit details in the public issue
```

The public `SECURITY.md` controls reporter guidance and response targets.

## 12. Supported-update period

### 12.1 Current development baseline

```text
Production security-support period: none declared
Status: development-only
```

This is an explicit limitation. Current branches and test packets carry no production maintenance commitment.

### 12.2 External pilot release gate

Before any external pilot, the release record MUST declare:

- support start date;
- minimum security-update end date;
- supported hardware revisions;
- supported firmware branches;
- update delivery method;
- end-of-support notification method;
- handling of critical vulnerabilities after end of support.

The period remains **TBD** until pilot scope, update mechanism and maintainer capacity are known. A release without a declared period must remain labelled development-only.

## 13. Release assurance checklist

A firmware release candidate passes only when:

- [ ] source commit and clean-tree state are recorded;
- [ ] SDK, toolchain, compiler and board target are pinned;
- [ ] SBOM is generated and archived;
- [ ] release artefact hashes are recorded;
- [ ] signing key identifier and signature state are recorded;
- [ ] target-enforced image verification is demonstrated;
- [ ] rollback and interrupted-update behaviour are tested;
- [ ] debug state is recorded;
- [ ] protocol version and security profile are recorded;
- [ ] valid and adverse protocol vectors pass;
- [ ] applicable advisories are reviewed;
- [ ] unresolved risks are accepted by name;
- [ ] supported-update period is declared;
- [ ] release notes include recovery instructions;
- [ ] no secret material is present in source or artefacts.

## 14. Immediate assurance work

1. Generate a reproducible dependency manifest and SBOM for the current DWM and NUCLEO builds.
2. Select and prove the verified-boot path independently for nRF52833, STM32N657 and ESP32-C6.
3. Define the secure record cryptographic profile and deterministic adverse test vectors.
4. Add a vulnerability-advisory log and release-security checklist to CI.
5. Preserve development debug access until signing and recovery proofs are complete.

## 15. Revision history

| Date | Revision | Change |
|---|---|---|
| 2026-07-11 | Draft A | Created firmware inventory, SBOM, signing, debug, update, advisory and support-period baseline |
