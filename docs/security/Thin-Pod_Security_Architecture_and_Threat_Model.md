# Thin-Pod Security Architecture and Threat Model

**Document identifier:** `TP-SYS-SEC-0001`
**Revision:** Draft A
**Date:** 11 July 2026
**Status:** Security architecture and threat-model baseline
**Applies to:** Thin-Pod rev 0.1 node, Thin-Pod Gateway rev 0.1, associated firmware, host tooling and later backend integration
**Evidence baseline:** Node commit `289c93e14eff67d7815be4803c488e5ac80a4f6c`; Gateway commit `a2c8b846d1a67bcbf2f612d94cc7d17feea107dc`

## 1. Purpose

This document defines the security architecture, trust boundaries, assets, threats, control objectives and verification gates for the Thin-Pod experimental condition-monitoring system.

The baseline replaces an earlier security model that assumed an Ethernet-connected STM32 Gateway and an ESP32-based Thin-Pod node. The current system uses:

- a Thin-Pod rev 0.1 analogue vibration-sensing carrier;
- a node-side Qorvo DWM3001-CDK containing an nRF52833 and DW3110;
- a Gateway-side Qorvo DWM3001-CDK;
- an STM32 NUCLEO-N657X0-Q as Gateway host and prospective analysis supervisor;
- an optional Seeed Studio XIAO ESP32-C6 as the onward-networking subsystem;
- a later backend or client service outside the present hardware release.

This is a design and assurance document. It records required controls and current gaps. It does not claim that secure boot, authenticated UWB transport, secure host-interface messaging or operational OTA has been implemented.

## 2. Security principles

1. The measurement chain must preserve authenticity, integrity, provenance and availability state from acquisition to decision.
2. The optional ESP32-C6 is the network-edge trust zone. The NUCLEO should remain free from direct LAN exposure in the initial architecture.
3. Every MCU and communications subsystem is treated as an independent firmware security domain.
4. A CRC detects accidental corruption. A cryptographic MAC or authenticated-encryption tag establishes integrity under an active attacker.
5. Missing, rejected, stale and replayed records remain explicit evidence. Absence of data must never be converted into a healthy-machine conclusion.
6. Development access and deployment hardening are separate profiles with recorded transitions.
7. Open hardware and open firmware contain no deployment secrets. Device-specific keys and credentials remain private.
8. Security claims advance only after the corresponding adverse tests have passed.
9. Recovery is part of the security design. A device that verifies firmware but cannot recover safely remains operationally fragile.
10. Component versions, advisories, signing state and support commitments must be traceable to each release.

## 3. System security context

```text
Mechanical vibration
        |
        v
Thin-Pod rev 0.1 analogue carrier
        |
        | analogue measurement path
        v
Node DWM3001-CDK
  nRF52833 firmware + DW3110 UWB radio
        |
        | UWB trust boundary
        v
Gateway DWM3001-CDK
  nRF52833 firmware + DW3110 UWB radio
        |
        | SPI host-interface trust boundary
        v
STM32 NUCLEO-N657X0-Q
  record ownership, buffering, logging, later DSP/TinyML
        |
        | narrow onward-interface trust boundary
        v
XIAO ESP32-C6
  optional IP networking and remote update agent
        |
        | LAN / Internet / backend trust boundary
        v
Backend service, maintenance client or evidence archive
```

The present Gateway repository establishes firmware execution, debug logging, physical interface evidence and protocol scaffolding. Real DW3110 RF exchange, authenticated measurement transport, DWM-to-NUCLEO record transfer, operational OTA and backend integration remain future verification stages.

## 4. Assets and security objectives

| Asset | Required property | Consequence of compromise |
|---|---|---|
| Raw vibration window | Authenticity, integrity, provenance, ordering | False or misleading condition evidence |
| Acquisition metadata | Integrity, completeness, binding to raw window | Invalid feature interpretation |
| Node identity | Authenticity, uniqueness, revocability | Device impersonation or data substitution |
| Firmware image | Authenticity, version control, recoverability | Persistent code execution or unsafe operation |
| Signing keys | Confidentiality, controlled use, revocability | Fleet-wide firmware compromise |
| Transport keys | Confidentiality where required, uniqueness, rotation | Record forgery, replay or disclosure |
| Gateway host commands | Integrity, authorisation, bounded parsing | Peripheral-to-host compromise |
| Analytic result | Traceability to exact input and processing version | Unsupported maintenance conclusion |
| Security logs | Integrity, time context, retention | Loss of forensic evidence |
| Availability state | Accurate reporting | Communication failure mistaken for healthy operation |
| Debug interface | Controlled access | Firmware extraction, modification or key exposure |
| Backend credentials | Confidentiality, rotation | Unauthorised service access |

The priority order for the experimental system is:

1. firmware authenticity;
2. measurement authenticity and integrity;
3. replay resistance and ordering;
4. evidence provenance;
5. safe failure and recoverability;
6. availability-state reporting;
7. confidentiality where machine information or credentials require it.

## 5. Trust zones and responsibility allocation

| Zone | Components | Security responsibility |
|---|---|---|
| Physical sensing zone | ADXL1005 interface and node carrier | Expose a measurable signal and preserve hardware identity; no cryptographic function is claimed by the carrier |
| Node firmware zone | Node DWM3001-CDK | Acquire or receive the measurement, assign identity/session/sequence, authenticate records and control the UWB radio |
| UWB communications zone | Node and Gateway DW3110 radios | Apply configured radio security, transport framing, anti-replay input and loss reporting |
| Gateway radio zone | Gateway DWM3001-CDK | Validate radio frames, reconstruct bounded records and expose a constrained host interface |
| Measurement and analysis zone | NUCLEO-N657X0-Q | Accept only validated bounded records, preserve raw evidence and generate traceable derived outputs |
| Network-edge zone | XIAO ESP32-C6 | Own IP networking, remote credentials, TLS, OTA and network-originated input filtering |
| Backend zone | Future service or archive | Authenticate devices, authorise access, retain records and manage credentials and lifecycle policy |

No trust zone may assume that an adjacent zone is uncompromised. Each boundary requires explicit validation and a defined failure response.

## 6. Threat actors and operating assumptions

### 6.1 Threat actors

- a nearby radio attacker able to transmit, receive, replay or jam UWB, BLE or Wi-Fi;
- a LAN attacker able to probe or compromise the XIAO ESP32-C6;
- an attacker with temporary physical access to a node or Gateway;
- a malicious or compromised firmware dependency;
- a contributor or build host that introduces unauthorised code or artefacts;
- an operator who installs the wrong firmware or key material;
- a compromised backend credential or service account;
- an accidental fault that resembles an attack, including bit corruption, brown-out, reset loops or partial update.

### 6.2 Assumptions

- the current development kits expose debug and recovery interfaces;
- laboratory firmware may retain J-Link, ST-LINK, UART and RTT access;
- production keys do not yet exist;
- the current packet CRCs provide accidental-error detection only;
- the current Telemetry Packet v1 is a laboratory proof format;
- the XIAO ESP32-C6 remains optional and outside the measurement-critical path;
- physical radio jamming cannot be prevented by cryptography;
- a future deployed profile will require a deliberate transition from development access to restricted debug and signed update.

## 7. Attack surfaces

| Surface | Examples |
|---|---|
| Node physical access | SWD/J-Link access, reset manipulation, flash extraction, sensor substitution |
| Gateway physical access | NUCLEO debug, DWM debug, SPI probing, XIAO programming access |
| UWB | spoofing, replay, malicious framing, jamming, forced resource use |
| BLE | unintended advertising, pairing, debug or DFU exposure |
| Wi-Fi/IP | service enumeration, credential attack, malformed network input, TLS misconfiguration |
| Host interface | oversized lengths, unknown opcodes, invalid state transitions, response confusion |
| Firmware update | unsigned image, downgrade, interrupted update, compromised signing key |
| Build and release | dependency substitution, unreviewed binary, missing SBOM, non-reproducible artefact |
| Backend | stolen credentials, excessive permissions, weak retention or audit controls |
| Maintenance workflow | false confidence, ignored stale-data state, unsupported diagnostic claim |

Unused services and radios should remain disabled by default.

## 8. Threat register

Status terms:

- **Open:** required control has not been implemented or proved.
- **Partial:** a supporting mechanism exists, while the security property remains unproved.
- **Planned:** architecture has been selected, implementation has not started.
- **Accepted for laboratory use:** risk is temporarily accepted within a controlled bench environment.

| ID | Threat | Principal impact | Required controls | Verification evidence | Present status |
|---|---|---|---|---|---|
| T-01 | Unauthorised firmware executes on an MCU | Persistent compromise | Verified boot, protected trust anchor, signed images, failure on alteration | Corrupted-image rejection and valid-image boot log | Open |
| T-02 | Vulnerable firmware is restored | Reintroduction of known weakness | Security version, anti-rollback state, controlled recovery | Downgrade attempt rejected | Open |
| T-03 | Update interruption bricks a device | Loss of service | A/B or revertible update, power-failure test, wired recovery | Repeated interrupted-update recovery | Open |
| T-04 | Node identity is forged | False measurement source | Per-device identity, authenticated record, key revocation | Impersonation test rejected | Open |
| T-05 | Valid records are replayed | False healthy or fault history | Boot epoch, monotonic sequence, replay window | Duplicate and stale record rejection | Open |
| T-06 | Payload is altered in transit | Invalid evidence | MAC or AEAD tag over header, metadata and payload | Single-bit and field-substitution tests rejected | Open |
| T-07 | CRC is treated as authentication | Unsupported security claim | Explicit CRC-versus-MAC wording and separate status fields | Documentation review and negative test | Partial |
| T-08 | Malformed UWB frame exhausts resources | Availability loss | Length bounds, timeouts, rate limits, fixed allocation ceilings | Fuzz and burst tests | Open |
| T-09 | Compromised DWM attacks NUCLEO | Host compromise | Allowlisted opcodes, bounded parser, state machine, no arbitrary memory access | Host-interface fuzzing and fault injection | Planned |
| T-10 | Compromised XIAO crosses into NUCLEO | Measurement-domain compromise | Narrow protocol, outbound-only networking, authorisation and validation | Malicious XIAO test harness | Planned |
| T-11 | Debug port exposes firmware or keys | Extraction or modification | Profile-based debug policy, lock before deployment, controlled service recovery | Readout and attach attempts fail in deployment profile | Open |
| T-12 | Shared fleet secret is exposed | Multi-device compromise | Per-device credentials and component-specific signing keys | Provisioning audit | Planned |
| T-13 | Signing key is compromised | Fleet-wide firmware compromise | Offline signing, access control, rotation and revocation | Key ceremony and revocation exercise | Planned |
| T-14 | Radio link is jammed | Missing data | Gap detection, stale-state reporting, bounded local retention | Jam or outage test reports unavailable state | Open |
| T-15 | Brown-out causes inconsistent state | Corrupt state or update | Brown-out detection, atomic state update, recovery test | Power cycling at threshold | Partial |
| T-16 | Dependency vulnerability remains unnoticed | Known exploitability | Pinned versions, SBOM, advisory monitoring, applicability record | Release assurance register | Partial |
| T-17 | Backend credentials are stolen | Unauthorised access | Per-device mTLS, least privilege, rotation, revocation | Revoked credential denied | Planned |
| T-18 | Security event cannot be reconstructed | Weak incident response | Boot/update/auth/replay counters, timestamps or boot epochs, retained logs | Event reconstruction exercise | Planned |
| T-19 | Missing data is interpreted as healthy | Unsafe decision | Explicit unavailable/stale state and claims control | Report-generation negative test | Planned |
| T-20 | Open repository exposes secrets | Credential compromise | Secret scanning, repository review, no private keys in source | Automated and manual release check | Planned |

## 9. Security controls by subsystem

### 9.1 Node DWM3001-CDK

Required deployed-profile controls:

- verified boot using a supported Zephyr-compatible boot chain;
- component-specific firmware signing key;
- anti-rollback or explicit security-version policy;
- device identity provisioned outside the public source tree;
- authenticated measurement records;
- boot epoch and monotonic sequence management;
- debug access restricted after commissioning;
- bounded local buffering and explicit gap reporting;
- BLE disabled unless a defined operational requirement exists;
- recovery procedure that does not bypass image verification.

Current position:

- nRF Connect SDK and Zephyr firmware execution has been demonstrated;
- J-Link and RTT remain enabled for development;
- secure boot, protected keys and authenticated record transport remain unproved.

### 9.2 Gateway DWM3001-CDK

Required controls:

- verified boot and signed firmware;
- strict validation of UWB frames;
- replay, identity and authentication status carried to the NUCLEO;
- fixed record-size ceilings;
- bounded reconstruction and timeout behaviour;
- no arbitrary host memory addressing;
- explicit counters for malformed, unauthenticated, replayed and dropped frames;
- controlled reset and recovery.

The Gateway DWM is treated as an intelligent communications subsystem and as an untrusted peripheral from the NUCLEO perspective.

### 9.3 STM32 NUCLEO-N657X0-Q

Required controls:

- a documented root-of-trust and verified-boot design;
- image version and rollback policy;
- signed update and recoverable image replacement;
- bounded host-interface parser;
- immutable raw record after acceptance into Gateway ownership;
- separation of transport status, authentication status and measurement validity;
- restricted debug in any external deployment;
- protected configuration and key material;
- security-event and boot-state logging.

A signed Zephyr artefact produced by the STM32 build/signing tools is useful build evidence. It does not, on its own, prove that the target enforces secure boot.

### 9.4 XIAO ESP32-C6

Required controls before network connection:

- Secure Boot v2;
- flash encryption in release mode;
- signed A/B OTA with rollback and first-boot confirmation;
- anti-rollback security version;
- outbound mTLS with per-device credentials;
- no unnecessary inbound service;
- restricted UART download and JTAG access in deployment profile;
- narrow, allowlisted interface to the NUCLEO;
- network-input length, type and rate validation;
- credential rotation and revocation;
- security logging that excludes secret material.

The XIAO should own IP networking. It should never function as a transparent bridge into NUCLEO memory or the DWM host interface.

### 9.5 Backend or evidence archive

Future requirements:

- device authentication and authorisation;
- least-privilege service accounts;
- key and certificate rotation;
- immutable or tamper-evident evidence retention where required;
- explicit data ownership and retention terms;
- audit records for firmware release and device enrolment;
- safe treatment of stale, missing or unauthenticated data.

## 10. Protocol security requirements

The detailed normative requirements are defined in:

```text
docs/security/Thin-Pod_Protocol_Security_Requirements.md
```

At minimum, any deployment-capable UWB record or secure host-interface envelope must bind:

- protocol version;
- message type;
- sender or node identity;
- boot epoch or session identifier;
- sequence number;
- key identifier;
- payload length;
- acquisition metadata or a digest of it;
- payload;
- authentication tag.

The receiver must authenticate the record before accepting it into measurement ownership. Replay checks occur after cryptographic authentication and before semantic processing.

## 11. Key hierarchy and provisioning

A future key hierarchy should separate:

| Key class | Scope |
|---|---|
| Offline firmware-signing root | Authorises component release-signing keys |
| Node DWM signing key | Signs node DWM firmware only |
| Gateway DWM signing key | Signs Gateway DWM firmware only |
| NUCLEO signing key | Signs NUCLEO firmware only |
| ESP32-C6 signing key | Signs XIAO firmware only |
| Per-device identity key | Authenticates one physical device |
| UWB session or group key | Protects a defined radio relationship and rotation domain |
| Backend client credential | Authenticates one Gateway or installation |

Rules:

1. Development and deployment keys remain separate.
2. Private signing keys never enter the Git repository, firmware image or routine build log.
3. A shared universal fleet secret is prohibited.
4. Every key has an identifier, owner, creation date, permitted use, rotation rule and revocation record.
5. Recovery and manufacturing processes must not create a bypass around verified boot.
6. Key compromise triggers documented revocation and replacement procedures.

The exact cryptographic implementation remains a future design decision and must use maintained platform libraries and reviewed algorithms. Thin-Pod will not implement a custom cryptographic primitive.

## 12. Firmware update architecture

Each component requires an independently verified update path.

| Component | Development update | Required deployed update |
|---|---|---|
| Node DWM3001-CDK | J-Link wired flash | Signed image, rollback control, recoverable boot path |
| Gateway DWM3001-CDK | J-Link wired flash | Signed image, rollback control, recoverable boot path |
| NUCLEO-N657X0-Q | ST-LINK / STM32CubeProgrammer | Verified boot, signed replacement, rollback and recovery |
| XIAO ESP32-C6 | Wired development flash | Signed A/B OTA, first-boot confirmation, rollback and anti-rollback |

A secure update gate passes only after:

- a valid image boots;
- an altered image is rejected;
- an image signed by an unknown key is rejected;
- a prohibited downgrade is rejected;
- interrupted update testing returns to a known-good image;
- the release identity is visible in logs;
- wired recovery remains possible under a controlled procedure.

## 13. Debug and physical-access profiles

| Profile | Debug state | Permitted use |
|---|---|---|
| Laboratory development | J-Link, ST-LINK, UART and RTT enabled | Bench bring-up and fault localisation |
| Controlled pilot | Debug access physically restricted and logged; secrets protected | Supervised field evaluation |
| Deployment candidate | Debug locked or authenticated; service process defined | External installation |
| Returned-device analysis | Controlled unlock or dedicated analysis image | Authorised forensic investigation |

Debug locking must occur only after recovery and signing procedures have been proved. Premature locking creates avoidable device-loss risk.

## 14. Security logging and evidence

Each firmware domain should expose, without leaking secrets:

- component and build identity;
- boot result and reset reason;
- image verification result;
- update attempt, success, failure and rollback;
- authentication failures;
- replay and stale-epoch rejections;
- malformed-frame and length violations;
- rate-limit events;
- key identifier in use;
- sequence gaps and dropped records;
- debug-policy state;
- clock or time-quality status where available.

Security logs are secondary records linked to the exact firmware build and session. They do not replace raw measurement evidence.

## 15. Vulnerability and dependency management

The controlling register is:

```text
docs/security/Thin-Pod_Firmware_Assurance_Register.md
```

Every release candidate must record:

- SDK, RTOS, compiler and tool versions;
- repository commit and board target;
- generated SBOM location;
- known advisories and applicability decisions;
- signing and debug state;
- update and recovery evidence;
- supported-update period;
- unresolved security risks.

Repository-level reporting instructions are defined in `SECURITY.md`.

## 16. Security verification gates

| Gate | Required evidence |
|---|---|
| SEC-0 Architecture baseline | Threat model, trust boundaries, asset list and protocol-security requirements committed |
| SEC-1 Dependency baseline | Pinned versions, release manifest and SBOM generated |
| SEC-2 Verified boot | Valid image accepted; corrupted and unknown-key images rejected |
| SEC-3 Recovery and rollback | Interrupted update recovers; downgrade policy enforced |
| SEC-4 Authenticated records | Modification, impersonation and replay tests rejected |
| SEC-5 Host-interface resilience | Bounds, opcode, state and fuzz tests pass without memory corruption |
| SEC-6 Network isolation | Compromised or malicious XIAO cannot access unauthorised NUCLEO functions |
| SEC-7 Debug transition | Deployment debug policy verified and recovery retained |
| SEC-8 Vulnerability response | Reporting route, triage process, support period and release procedure exercised |

No deployable-security claim should be made before the relevant gate is closed.

## 17. Current security claim boundary

The present project may claim:

- explicit modular trust boundaries have been defined;
- CRC and cryptographic authentication have been distinguished;
- security requirements and assurance gates have been documented;
- firmware versions and development debug paths can be recorded.

The present project must not yet claim:

- secure boot enforcement on any Thin-Pod MCU;
- authenticated or encrypted UWB measurement transport;
- replay-resistant host-interface records;
- production key provisioning;
- secure operational OTA;
- hardened physical deployment;
- formal penetration testing or regulatory conformity.

## 18. References

Project references:

- `docs/dwm-to-nucleo-host-interface-protocol.md`
- `docs/protocol/ThinPod_Gateway_Telemetry_Packet_v1.md`
- `docs/measurement-record-and-provenance-model.md`
- `docs/system-interface-control-document.md`
- `docs/architecture/Gateway_UWB_Module_Access_Model.md`
- `docs/observability/Thin-Pod_Memfault_nRFCloud_Evaluation.md`

General firmware references consulted:

- Subrata Banik and Vincent Zimmer, *System Firmware: An Essential Guide to Open Source and Embedded Solutions*, Apress, 2022, corrected 2023.
- Subrata Banik and Vincent Zimmer, *Firmware Development: A Guide to Specialized Systemic Knowledge*, Apress, 2022, corrected 2023.

## 19. Revision history

| Date | Revision | Change |
|---|---|---|
| 2026-07-11 | Draft A | Replaced the obsolete Ethernet/ESP32-node model with the current DWM, NUCLEO, XIAO and backend trust-boundary architecture |
