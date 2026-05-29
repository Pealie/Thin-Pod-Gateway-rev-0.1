# Thin-Pod Evidence-Grounded Maintenance Reporting Prototype v0.1

## High-Level Implementation Concept

### Purpose

This extension is both achievable and unusually well suited to Thin-Pod because it does not require pretending that a language model is a vibration-analysis engine. The project already has the beginnings of the correct hierarchy:

```text
physical measurement → deterministic DSP/features → evidence-bound interpretation → human decision
```

The AI component should sit at the third stage. Its job is to turn verified measurements, historical comparisons and engineering records into a careful maintenance report. It should never become the source of measured values, the owner of DSP calculation, or an autonomous issuer of fault diagnoses.

That distinction is what would make the project credible.

---

## 1. Preserve the Authority Boundary

The first design decision is conceptual rather than technical: decide which part of the system is allowed to claim what.

| System layer | What it may claim | What it must not claim |
|---|---|---|
| Thin-Pod sensor node | Captured acceleration window, sampling metadata, device identity | Mechanical fault diagnosis |
| Gateway / DSP pipeline | RMS, peak, crest factor, kurtosis, spectral band energy, comparison with baseline, data-quality flags | Free-form maintenance narrative |
| Assistant application | Explanation of supplied features, retrieval of relevant evidence, structured report, uncertainty statement, recommended inspection step | New measurements, invented trends, unsupported fault declarations |
| Human engineer/operator | Maintenance decision and intervention | None of the above should be silently delegated to the model |

A weak implementation would say: ‘Here is a waveform; ask an AI model what fault it suggests.’

A strong implementation would say: ‘Here is a cryptographically identifiable measurement event, processed by a known DSP version against a known baseline. Produce a report that may discuss only the evidence actually supplied.’

That second formulation is substantially more serious.

---

## 2. High-Level Architecture

The system could be organised as follows:

```text
Thin-Pod sensing node
    ↓
Timestamped vibration window transmitted by UWB
    ↓
Gateway receives and validates packet
    ↓
Deterministic DSP pipeline computes features and data-quality flags
    ↓
Immutable Measurement Event JSON record is created
    ↓
Maintenance-analysis assistant reads:
    • measurement event
    • historical comparison data
    • calibration / firmware / hardware evidence
    • maintenance-reporting policy
    ↓
Structured diagnostic report is generated
    ↓
Report and complete audit record are stored for review
```

The important new object is the **Measurement Event**. This is the bridge between the physical system and the AI application.

It might contain:

```text
event identifier
asset identifier
sensor-node revision
gateway revision
firmware version
DSP pipeline version
timestamp
sampling rate and window duration
raw-window file hash
computed features
baseline comparison
data-quality flags
deterministic alert state
links to calibration and test evidence
```

The assistant would receive this record through a controlled tool call. It would not calculate values from prose, reinterpret an oscilloscope screenshot, or infer measurements from incomplete context.

---

## 3. Scope of the First Version

The initial implementation should be deliberately modest:

> **An offline report-generation demonstrator using stored measurement events and known engineering evidence.**

There is no need initially for live streaming, cloud fleet management, autonomous alerting or a web dashboard. Those would create effort without strengthening the central claim.

The first proof could work from:

1. a small set of recorded or synthetic vibration scenarios;
2. deterministic feature outputs stored as JSON;
3. a curated evidence library containing Thin-Pod hardware documentation, Gateway/DSP description, calibration notes and test records;
4. an OpenAI API application that generates a constrained maintenance-analysis report;
5. an evaluation suite showing when the assistant reports correctly, refuses to overclaim and identifies inadequate evidence.

This would already be a strong portfolio artefact because it demonstrates an enterprise-relevant pattern: **trusted machine data combined with a controlled generative interface and measurable assurance behaviour**.

---

## 4. Recommended OpenAI Implementation Path

The first version should probably use the **Responses API directly**, rather than begin with a more elaborate agent framework. The Responses API is suited to direct model requests involving tools and structured output; more elaborate agent orchestration is better reserved for later workflows requiring deeper state, approvals and tracing.

Four API capabilities would be especially relevant.

### 4.1 Function Calling for Trusted Measurements

The assistant should obtain measurement facts only through functions owned by the application, for example:

```text
get_measurement_event(event_id)
get_baseline_comparison(asset_id, event_id)
get_dsp_definition(feature_name, pipeline_version)
get_test_record(test_id)
get_reporting_policy(policy_version)
```

The application executes each tool and returns structured facts to the model.

The critical design rule would be simple:

> **No numerical measurement claim may appear in the report unless it came from a measurement tool result or an explicitly cited evidence document.**

### 4.2 Retrieval for Engineering Evidence

The assistant would also need access to documentary material: hardware revisions, test procedures, sensor calibration notes, DSP feature definitions, engineering logs and maintenance-reporting guidance.

For the first implementation, the evidence library should be small and controlled. Suitable material might include:

```text
Thin-Pod system overview
certified hardware scope statement
Gateway architecture document
DSP feature definitions
test-rig procedure
calibration / baseline procedure
known limitations document
maintenance-reporting policy
```

The assistant would then be able to say not merely ‘crest factor increased’, but:

> ‘Crest factor increased relative to the recorded baseline; the project documentation defines this as an observation requiring inspection rather than a definitive bearing-fault classification.’

### 4.3 Structured Outputs for Report Discipline

The final response should be generated to a strict schema rather than as unrestricted prose. A report schema might require:

```text
report_id
measurement_event_id
asset_id
measurement_facts[]
comparison_with_baseline[]
data_quality_status
interpretation_level
possible_explanations[]
prohibited_conclusions_avoided[]
recommended_next_action
evidence_citations[]
uncertainties[]
human_review_required
```

The field `interpretation_level` could have tightly restricted values:

```text
NO_ABNORMALITY_OBSERVED
FEATURE_CHANGE_OBSERVED
INSUFFICIENT_DATA
INSPECTION_RECOMMENDED
```

It should not initially contain values such as:

```text
BEARING_FAILURE_CONFIRMED
MISALIGNMENT_CONFIRMED
MOTOR_REPLACEMENT_REQUIRED
```

unless a separately validated deterministic diagnostic model eventually supports those claims.

### 4.4 Evals for Proof of Reliability

Evaluation is not a decorative extra. It is the part that turns an interesting demonstration into a credible engineering project.

The assistant must be tested for correct restraint, evidence citation, missing-data recognition, resistance to overclaiming and stability across prompt or model changes.

---

## 5. What the Assistant Would Actually Do

A maintenance-analysis request might begin with a stored event:

```text
Event TP-00017:
RMS elevated against baseline
crest factor unchanged
high-frequency band energy mildly elevated
sample integrity valid
firmware version known
calibration record present
no validated fault classifier result
```

The assistant could then produce something like:

```text
Observation:
The measurement window shows increased vibration energy relative to the recorded baseline, with no corresponding increase in crest factor.

Interpretation:
The evidence supports reporting a change in machine vibration behaviour. It does not support identifying a specific mechanical fault.

Suggested next action:
Repeat measurement under the same operating condition and inspect mounting security, load condition and sensor attachment before escalating to fault classification.

Evidence:
Measurement event TP-00017; baseline record BL-00003; DSP definition v0.1; reporting policy RP-00001.
```

That report is useful precisely because it is restrained. It communicates technical evidence, preserves uncertainty and points towards a sensible next step.

---

## 6. Evidence Model

A credible assistant needs a well-defined evidence hierarchy.

At the top would be immutable machine-generated evidence:

```text
raw sample-window hash
DSP feature outputs
sampling configuration
sensor/gateway firmware version
baseline comparison result
data-quality status
```

Below that would sit controlled engineering documentation:

```text
hardware revision documentation
calibration procedure
known test-rig condition
DSP definitions
maintenance interpretation rules
```

Only below those should the assistant be allowed to offer interpretive prose.

This creates a useful rule:

> **Every report sentence should belong to one of three categories: recorded fact, rule-based interpretation, or explicitly labelled uncertainty.**

That is the discipline that differentiates an industrial AI demonstrator from a casual generative application.

---

## 7. Staged Build Path

### Stage 1: Define the Reporting Contract

Before any API coding, define:

- the Measurement Event JSON schema;
- the allowed report schema;
- the permitted and prohibited forms of conclusion;
- the evidence documents required for a valid report;
- the boundary between DSP output and AI interpretation.

This is the conceptual heart of the work. A two-page design document would be sufficient to begin.

### Stage 2: Create a Small Evidence-Backed Dataset

Create perhaps ten to twenty measurement scenarios:

- normal baseline;
- elevated RMS only;
- increased crest factor;
- elevated frequency-band energy;
- missing baseline;
- poor sample quality;
- absent calibration record;
- conflicting metadata;
- apparently abnormal data but insufficient evidence for fault identification.

These can initially be synthetic or derived from controlled bench tests. A later version can use real motor-rig measurements.

### Stage 3: Build the Report Generator

A Python application would:

1. load the selected Measurement Event;
2. expose measurement and documentation retrieval functions;
3. call the OpenAI Responses API;
4. require structured report output;
5. render the JSON as readable Markdown or HTML;
6. store the report alongside its inputs and metadata.

The first visible product would therefore be a set of trustworthy, reproducible maintenance-analysis reports.

### Stage 4: Build the Refusal and Overclaim Tests

This is where the project becomes particularly interesting.

Test questions should include:

```text
Can the assistant be induced to claim bearing failure when no classifier result exists?
Does it invent numerical values absent from the event record?
Does it acknowledge missing calibration evidence?
Does it distinguish a feature change from a diagnosed fault?
Does it cite the correct supporting records?
Does a changed prompt or model alter safety-critical conclusions?
```

A successful demonstrator should report failure rates, not simply show attractive example outputs.

### Stage 5: Connect Live Gateway Output

Only once the offline workflow is sound should a real Gateway measurement be admitted into the pipeline.

The Gateway would create a Measurement Event from an actual vibration window; the assistant would then produce the same constrained report from live-derived data. At that point the demonstration becomes:

```text
physical event → sensor capture → UWB transport → gateway DSP → evidence-grounded AI report
```

That is a compelling end-to-end story.

---

## 8. Observability and Auditability

Every generated report should carry a provenance record:

```text
measurement event identifier
raw data hash
DSP version
firmware version
evidence-document versions
prompt version
model identifier
tool results supplied to the model
generated structured output
evaluation status
date and reviewer
```

For the initial version, this could simply be an append-only JSONL log and a report directory under version control.

At a later stage, agent tracing may become relevant for model calls, tool calls, guardrails and custom execution spans. Starting there, however, would be premature. The first system is best understood as a **single focused reporting workflow with controlled tools**, not a multi-agent maintenance platform.

---

## 9. Security and Integrity Principles

This extension should inherit the existing Thin-Pod principle that measurement trust matters.

The main principles would be:

- the model receives read-only measurement facts;
- raw windows and feature records are hashed or otherwise integrity-tracked;
- the assistant cannot alter baseline records or diagnostic thresholds;
- prompt and schema versions are controlled;
- API keys remain outside the repository;
- no employer or customer network data enters the demonstration;
- reports clearly state that they support inspection decisions rather than replace engineering judgement;
- future automated maintenance actions remain explicitly out of scope.

This would align with the project’s existing security-and-trust posture rather than creating a separate AI ornament.

---

## 10. Position Within the Thin-Pod Platform

Thin-Pod rev 0.1 has achieved OSHWA certification for a defined hardware scope. The assistant should not be folded into the certification claim or described as part of the certified object.

A clean conceptual separation would be:

```text
Thin-Pod rev 0.1
    Certified sensing hardware

Thin-Pod Gateway
    Communications, DSP and feature extraction platform

Thin-Pod Maintenance Analysis Assistant
    Evidence-grounded AI application consuming Gateway/DSP records
```

This preserves the integrity of the open-hardware achievement while allowing the wider Thin-Pod platform to demonstrate contemporary AI deployment capability.

A repository structure might eventually look like:

```text
docs/
    ai-maintenance-assistant-overview.md
    measurement-event-schema.md
    reporting-policy.md
    evaluation-plan.md
    security-and-provenance.md

assistant/
    schemas/
    tools/
    prompts/
    evals/
    sample-events/
    generated-reports/
```

Whether this sits in the existing broader project repository or in a linked companion repository can be decided later. Conceptually, it should remain a distinct application layer.

---

## 11. What Not to Build First

Several tempting directions would weaken the project if pursued too early:

- an unrestricted chatbot over vibration data;
- AI analysis of waveform images without deterministic features;
- autonomous fault diagnosis;
- automatic maintenance recommendations with no rule boundary;
- live cloud streaming before offline evaluation works;
- multi-agent complexity before one audited reporting workflow is reliable;
- fine-tuning before the problem is understood through schemas, tools and evals.

The merit of this project lies in disciplined integration, not maximal AI theatricality.

---

## First Practical Milestone

The first milestone should be a document and a small executable demonstrator:

# Thin-Pod Evidence-Grounded Maintenance Reporting Prototype v0.1

It would contain:

1. a Measurement Event schema;
2. a constrained report schema;
3. five to ten representative events;
4. a Python report generator using OpenAI function calling and Structured Outputs;
5. a small evidence library;
6. an evaluation table showing correct restraint, correct citation and refusal to diagnose unsupported faults;
7. two or three rendered example reports.

This is small enough to complete without derailing Gateway development, but significant enough to change how the wider platform is perceived. It would demonstrate that Thin-Pod is not merely a sensor project with AI vocabulary attached, but an engineered measurement system capable of supporting traceable, evaluated and professionally bounded AI-assisted maintenance reasoning.

---

## Official OpenAI Documentation Relevant to the Prototype

- [OpenAI API documentation](https://developers.openai.com/api/docs)
- [Function calling](https://developers.openai.com/api/docs/guides/function-calling)
- [File search](https://developers.openai.com/api/docs/guides/tools-file-search)
- [Structured Outputs](https://developers.openai.com/api/docs/guides/structured-outputs)
- [Evals](https://developers.openai.com/api/docs/guides/evals)
- [Agents and observability](https://developers.openai.com/api/docs/guides/agents/integrations-observability)
