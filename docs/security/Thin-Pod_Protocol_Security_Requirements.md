# Thin-Pod Protocol Security Requirements

**Document identifier:** `TP-SYS-SEC-0002`
**Revision:** Draft A
**Date:** 11 July 2026
**Status:** Normative security requirements before protocol freeze
**Applies to:** Node-to-Gateway UWB transport, Gateway DWM-to-NUCLEO host interface and NUCLEO-to-XIAO onward interface
**Related documents:** `docs/dwm-to-nucleo-host-interface-protocol.md`, `docs/protocol/ThinPod_Gateway_Telemetry_Packet_v1.md`

## 1. Purpose

This document defines the security properties and mandatory semantic fields required before any Thin-Pod transport format is designated suitable for external deployment.

The current protocols support laboratory bring-up:

- Telemetry Packet v1 is a fixed 24-byte test packet with a node identifier, sequence and CRC16;
- the DWM-to-NUCLEO Draft A protocol uses a bounded header, request sequence and CRC16;
- the Stage-1 UWB implementation uses a stub backend.

These formats remain useful engineering evidence. They do not establish sender authenticity, malicious-modification resistance or replay protection.

## 2. Normative language

The terms **MUST**, **MUST NOT**, **SHOULD**, **SHOULD NOT** and **MAY** express requirement strength.

A deployment-capable protocol version MUST satisfy every applicable MUST requirement in this document and retain test evidence for the associated acceptance criteria.

## 3. Security properties

A deployment-capable Thin-Pod record protocol MUST provide:

1. **Source authenticity:** the receiver can verify the authorised sending device or security context.
2. **Integrity:** alteration of protected fields or payload is detected.
3. **Replay resistance:** duplicate, stale and invalid-session records are rejected.
4. **Context binding:** payload is bound to node identity, acquisition context and protocol version.
5. **Ordering evidence:** gaps, duplicates and reordering remain observable.
6. **Bounded parsing:** every length, count and state transition has a fixed limit.
7. **Cryptographic agility:** the algorithm profile and key identifier can evolve without ambiguous interpretation.
8. **Failure visibility:** authentication, replay, length and version failures are recorded separately.
9. **Safe defaults:** unknown versions, algorithms, keys and opcodes are rejected.
10. **Optional confidentiality:** encryption MAY be applied where operating data or credentials require it; integrity and authenticity remain mandatory.

## 4. Required semantic fields

The exact byte layout remains a protocol-version decision. Every authenticated measurement record MUST bind the following semantics.

| Field | Purpose | Requirement |
|---|---|---|
| `protocol_version` | Selects an unambiguous parser and security profile | MUST be authenticated |
| `message_type` | Distinguishes telemetry, raw window, control, acknowledgement and error | MUST be authenticated |
| `sender_id` or `node_id` | Identifies the originating device | MUST be authenticated and provisioned |
| `receiver_id` or destination context | Prevents cross-context substitution where needed | SHOULD be authenticated |
| `boot_epoch` or `session_id` | Separates sequences across reboots or rekeying | MUST be authenticated |
| `sequence` | Supports ordering, duplicate detection and replay defence | MUST be authenticated and monotonic within the epoch |
| `key_id` | Selects the verification key or key version | MUST be authenticated or uniquely implied by the security envelope |
| `payload_len` | Bounds parsing and prevents truncation/extension ambiguity | MUST be authenticated |
| `acquisition_config_id` or metadata digest | Binds the sample to rate, range, channel and configuration | MUST be authenticated for measurement records |
| `window_id` | Links fragments and provenance records | MUST be authenticated for window transfer |
| `payload` | Carries measurement or control content | MUST be authenticated |
| `auth_tag` | Verifies authenticity and integrity | MUST be present in a secure profile |
| `nonce` or nonce derivation inputs | Supports AEAD uniqueness where encryption is used | MUST be unambiguous and never repeat under the same key |

A record MAY retain a CRC for early corruption detection and diagnostic counters. The CRC is outside or inside the authenticated envelope according to the selected profile, but it MUST never be reported as proof of authenticity.

## 5. CRC, hash, MAC and AEAD meanings

| Mechanism | Thin-Pod use | Security meaning |
|---|---|---|
| CRC16/CRC32 | Link diagnostics and accidental corruption detection | No sender identity and no resistance to deliberate modification |
| SHA-256 or later approved hash | Identifying an archived artefact or release | No sender identity unless signed or authenticated |
| MAC | Authenticity and integrity under a shared secret | Supports malicious-modification detection; no confidentiality |
| AEAD | Authenticity, integrity and confidentiality | Preferred where encryption is required |
| Digital signature | Firmware release or independently verifiable artefact | Supports public verification and controlled release authority |

Documentation, logs and field names MUST use these meanings consistently.

The following claim is prohibited:

```text
CRC valid, therefore the sender and data are authentic.
```

The following wording is acceptable:

```text
CRC valid: no accidental corruption was detected by the configured CRC.
Authentication valid: the record was verified under key identifier K.
Replay accepted: the epoch and sequence passed the configured replay policy.
```

## 6. Cryptographic profile

Thin-Pod MUST use a maintained cryptographic implementation supplied by the selected platform or a widely reviewed library.

Thin-Pod MUST NOT implement a custom cipher, MAC, signature scheme, random-number generator or key-derivation function.

Before implementation freeze, a cryptographic profile document MUST specify:

- algorithm and mode;
- key length;
- tag length;
- nonce length and construction;
- associated-data fields;
- key lifetime and rotation;
- failure behaviour;
- test vectors;
- platform library and version;
- any hardware acceleration used;
- side-channel and fault assumptions relevant to the target.

Algorithm selection remains open in this revision. The protocol layout MUST reserve sufficient space or an extension mechanism for the selected profile.

## 7. Boot epoch and sequence requirements

### 7.1 Boot epoch

`boot_epoch` or `session_id` MUST change whenever:

- the sender reboots;
- the communication key changes;
- sequence state is lost;
- a security session is re-established.

The epoch MAY be:

- a persistent monotonic boot counter;
- a cryptographically random session identifier with collision risk bounded by the selected profile;
- a value assigned through an authenticated session-establishment procedure.

A constant boot epoch is prohibited for deployment.

### 7.2 Sequence

The sender MUST maintain a monotonic sequence within one epoch and security context.

The receiver MUST:

- reject an exact duplicate;
- reject a sequence older than the accepted replay window;
- record forward gaps;
- define whether limited reordering is accepted;
- reset replay state only after authenticated acceptance of a new epoch or key context.

Sequence rollover MUST be defined. Rekeying or a new epoch SHOULD occur before rollover.

## 8. Replay-window behaviour

A receiver MAY use a highest-seen sequence and a bounded bitmap window.

Required behaviour:

1. Authenticate the record.
2. Identify the sender, key and epoch.
3. Reject an unknown or revoked key.
4. Reject a stale or unauthorised epoch.
5. Reject a sequence below the replay window.
6. Reject a duplicate already marked in the window.
7. Accept an authenticated unseen sequence within policy.
8. Advance the window for a new highest sequence.
9. record gaps and reordering as transport evidence.

Unauthenticated input MUST NOT advance replay state.

The replay-window size is a deployment parameter and MUST be fixed in the protocol profile. It should reflect expected radio reordering and memory limits.

## 9. Authentication coverage

For a measurement record, the authentication tag MUST cover:

```text
protocol version
message type
sender or node identity
receiver or installation context where applicable
boot epoch or session identifier
sequence
key identifier or security profile identifier
payload length
window identifier
acquisition configuration identity or metadata digest
fragment position and total where fragmentation is used
payload
```

Fields that influence parsing, routing, replay state, acquisition interpretation or ownership MUST NOT remain unauthenticated.

## 10. Fragmentation and reassembly

Where a vibration window is fragmented:

- every fragment MUST carry authenticated window identity, fragment index and fragment count or total length;
- fragment count, total record length and per-fragment length MUST have fixed maxima;
- fragments from different senders, epochs, keys or windows MUST NOT be combined;
- duplicate fragments MUST be detected;
- incomplete records MUST expire after a bounded timeout;
- the NUCLEO MUST receive a complete authenticated record or an explicit rejected/incomplete status;
- partial payload MUST NOT enter DSP or TinyML processing.

An end-to-end authentication tag over the complete logical record is preferred. Per-fragment protection MAY be added to reject malicious fragments early.

## 11. UWB transport requirements

The UWB layer MUST:

- use an explicitly selected IEEE 802.15.4z/DW3110 security configuration where supported and appropriate;
- retain application-record authentication independently of radio-layer security;
- bind node identity and session context to the measurement record;
- detect replay and duplicate records;
- apply fixed frame and reassembly limits;
- expose radio loss, authentication failure and replay counters separately;
- treat jamming or sustained loss as an unavailable-data condition;
- avoid a healthy-state conclusion when current authenticated data is absent.

Radio-layer ranging or STS capability does not, by itself, authenticate the Thin-Pod application record.

## 12. DWM-to-NUCLEO host-interface requirements

The host interface MUST preserve the security state of every record crossing from the Gateway DWM to NUCLEO ownership.

### 12.1 Parser controls

The NUCLEO and DWM implementations MUST:

- enforce the declared header length;
- enforce per-opcode payload maxima;
- reject reserved flag bits;
- reject unknown protocol versions and opcodes;
- use fixed-width integer types;
- check arithmetic for overflow before allocation or copy;
- use bounded buffers;
- enforce request/response sequence matching;
- apply transaction timeouts;
- recover to a defined state after malformed input;
- expose counters for each rejection class.

### 12.2 Measurement-record transfer

A record descriptor transferred to the NUCLEO MUST include or reference:

- originating node identity;
- boot epoch;
- UWB/application sequence;
- key identifier;
- authentication result;
- replay result;
- complete-record length;
- acquisition configuration identity;
- window identity;
- transport loss or reassembly status.

The preferred deployed design carries the authenticated node record, or a verifiable authenticated envelope, through the Gateway DWM so that the NUCLEO can verify or independently retain the verification evidence.

### 12.3 Control commands

The minimal `GET_CAPABILITIES`, `GET_STATUS` and `GET_COUNTERS` commands remain valid for laboratory proof.

Before deployment freeze:

- control commands that modify state MUST require an authorised security context;
- arbitrary memory read/write commands are prohibited;
- reset, key, update and configuration commands require separate authorisation;
- the interface MUST distinguish accidental CRC failure from cryptographic authentication failure;
- any local-session authentication design requires its own threat analysis and test vectors.

## 13. NUCLEO-to-XIAO onward-interface requirements

The onward interface MUST:

- expose a narrow set of message types;
- prevent arbitrary memory access or transparent bridging;
- authenticate security-sensitive commands;
- validate all network-originated lengths and types before forwarding;
- preserve record identity, provenance and authentication status;
- rate-limit repeated malformed or unauthorised requests;
- default to outbound telemetry and update checks;
- keep credentials and TLS private keys within the network-edge security domain where feasible;
- record XIAO, NUCLEO and protocol build identities.

Compromise of the XIAO must not automatically grant firmware-update or arbitrary-control authority over the NUCLEO or DWM modules.

## 14. Required status and error semantics

A secure protocol version MUST distinguish at least:

```text
OK
BUSY
BAD_MAGIC
BAD_VERSION
BAD_LENGTH
BAD_CRC
UNKNOWN_OPCODE
NOT_READY
INTERNAL_ERROR
AUTH_FAILED
UNKNOWN_KEY
REVOKED_KEY
REPLAY_DUPLICATE
REPLAY_TOO_OLD
STALE_EPOCH
NONCE_ERROR
RATE_LIMITED
INCOMPLETE_RECORD
```

The external response MAY use a less detailed error to avoid creating an oracle. Internal counters and evidence logs SHOULD preserve the precise rejection reason.

## 15. Validation order

A receiver SHOULD process a frame in this order:

1. receive into a bounded buffer;
2. validate minimum framing and declared lengths;
3. reject unknown version, profile and impossible counts;
4. perform optional CRC diagnostics;
5. select the key or security context;
6. verify the MAC or AEAD tag;
7. apply epoch and replay-window checks;
8. validate semantic state and opcode authorisation;
9. commit a complete accepted record to ownership;
10. update counters and provenance;
11. expose the record to higher-level processing.

Failure at any stage MUST prevent measurement ownership and downstream analytics.

## 16. Telemetry Packet v1 classification

`docs/protocol/ThinPod_Gateway_Telemetry_Packet_v1.md` remains:

- a laboratory transport and parser test vector;
- fixed at 24 bytes;
- protected by CRC16 for accidental-error detection;
- unauthenticated;
- unsuitable for external deployment or security claims.

A secure successor MUST use a new version and satisfy this document. Security fields must not be retrofitted ambiguously into reserved bits of v1.

## 17. Protocol freeze gates

No UWB or host-interface format may be marked `deployment-frozen`, `production`, `secure` or equivalent until:

| Gate | Acceptance evidence |
|---|---|
| Field semantics | Required identity, epoch, sequence, key and tag fields defined |
| Cryptographic profile | Maintained library, algorithm, nonce and tag rules recorded |
| Deterministic vectors | Valid and invalid test vectors committed |
| Modification resistance | Header, metadata and payload alteration rejected |
| Identity resistance | Wrong-device or wrong-key input rejected |
| Replay resistance | Duplicate, stale and reordered cases tested |
| Bounds | Maximum lengths, fragments and queue use tested |
| Recovery | Malformed input cannot wedge the interface permanently |
| Provenance | Accepted record retains authentication and replay status |
| Version transition | v1 laboratory formats remain unambiguous and separately parsed |

## 18. Required test vectors

The repository SHOULD contain machine-readable vectors for:

- one valid authenticated record;
- one valid record at each maximum permitted length;
- wrong authentication tag;
- unknown key identifier;
- revoked key;
- modified sender identity;
- modified acquisition metadata;
- modified payload;
- duplicate sequence;
- stale sequence outside the replay window;
- new boot epoch;
- stale boot epoch;
- fragment duplication;
- fragment substitution across windows;
- oversized declared length;
- unsupported version and algorithm profile;
- sequence rollover boundary.

Each vector requires expected parser, authentication, replay and ownership outcomes.

## 19. Implementation sequence

1. Retain existing v1 and Draft A formats for laboratory bring-up.
2. Select the application authentication and optional confidentiality profile.
3. Define key provisioning and boot-epoch persistence.
4. Create a secure record version with deterministic vectors.
5. Implement verification first in host tooling.
6. Implement node generation and Gateway verification.
7. Implement replay state and adverse tests.
8. Carry authentication status through the host interface.
9. Fuzz all parsers within fixed memory limits.
10. Freeze the secure format only after the protocol gates pass.

## 20. Revision history

| Date | Revision | Change |
|---|---|---|
| 2026-07-11 | Draft A | Defined identity, sequence, boot epoch, key identifier, authentication tag, replay and CRC-versus-MAC requirements before protocol freeze |
