# Security Policy

## Project security status

Thin-Pod is an experimental open-hardware and firmware programme. The repositories currently contain hardware designs, firmware bring-up applications, protocol scaffolding, verification records and research specifications.

No current Thin-Pod firmware release is represented as a production-secure or safety-certified deployment.

The following capabilities remain development objectives:

- enforced verified boot on each MCU;
- authenticated and replay-resistant UWB measurement records;
- hardened DWM-to-NUCLEO host-interface transfer;
- secure operational OTA;
- production key provisioning;
- deployment debug restrictions;
- a declared production security-support period.

## Supported versions

| Component or release | Security-support status |
|---|---|
| Thin-Pod rev 0.1 open-hardware carrier | Hardware documentation baseline; firmware and radio security remain outside the certified hardware scope |
| Thin-Pod Gateway rev 0.1 development branches | Active development; no production security-support commitment |
| Stage-1 UWB stub and Telemetry Packet v1 | Laboratory evidence only; unauthenticated and unsuitable for external deployment |
| Future tagged deployment firmware | Support period will be declared in the release and firmware-assurance register |

Security fixes may be applied to active development branches on a best-effort basis. A defined minimum update period must be approved before any external pilot or commercial firmware release.

## Reporting a vulnerability

Preferred route:

1. Use GitHub's **Report a vulnerability** or private security-advisory function for this repository.
2. Include the affected component, repository commit or release, reproduction conditions, likely impact and any proposed mitigation.
3. Avoid publishing exploit details, keys, credentials, private device data or unpatched proof-of-concept code in a public issue.

Where private vulnerability reporting is unavailable, open a minimal public issue titled `Security contact request` without technical details. The repository owner will establish a private channel.

Do not use security reports for general support, feature requests or routine build failures.

## Expected response

This is a solo, independently maintained project. The target response process is:

| Stage | Target |
|---|---:|
| Acknowledge receipt | 7 calendar days |
| Initial triage and applicability decision | 14 calendar days |
| Remediation plan or status update | 30 calendar days |
| Coordinated disclosure | Agreed according to severity, exploitability and update readiness |

These are project targets rather than a service-level guarantee.

## Report content

A useful report contains:

- affected repository and path;
- exact commit, tag or firmware build identity;
- affected MCU, board or host tool;
- attack prerequisites;
- steps to reproduce;
- expected and observed behaviour;
- impact on confidentiality, integrity, authenticity, availability or recovery;
- whether physical access is required;
- suggested remediation;
- disclosure constraints.

## Scope

Security-relevant areas include:

- boot and update verification;
- signing and key handling;
- downgrade and rollback;
- UWB, BLE, Wi-Fi and host-interface protocols;
- record authenticity and replay handling;
- parser bounds and memory safety;
- debug and recovery interfaces;
- build dependencies and release artefacts;
- backend credentials and onward networking;
- security logging and provenance;
- secrets accidentally committed to the repository.

The following generally fall outside security-reporting scope unless they create a concrete vulnerability:

- normal radio interference or range limitations;
- expected development debug access documented as laboratory-only;
- incomplete future features;
- unsupported hardware combinations;
- denial of service requiring permanent possession of an unprotected development board.

## Disclosure and credit

The project favours coordinated disclosure. Reporter credit will be given where requested and legally permissible after a fix or mitigation is available.

No bounty programme is currently offered.

## Security design documents

- `docs/security/Thin-Pod_Security_Architecture_and_Threat_Model.md`
- `docs/security/Thin-Pod_Protocol_Security_Requirements.md`
- `docs/security/Thin-Pod_Firmware_Assurance_Register.md`
- `docs/measurement-record-and-provenance-model.md`

## Current limitation statement

CRC values in the current protocols detect accidental corruption. They do not authenticate a sender or resist deliberate alteration. Current packet and host-interface formats must not be described as secure until the authentication and replay gates in the protocol-security requirements have passed.
