# DW3110 One-Way RF Exchange Procedure

## Judgement

The next defensible Gateway firmware gate is a one-way physical UWB frame
exchange from the Gateway-side DWM3001-CDK to the Node-side DWM3001-CDK. The
proof uses the already validated external Qorvo driver, SPI3 path, reset path and
DW3110 initialisation sequence. It adds only fixed PHY configuration, immediate
transmission, immediate reception, polled status handling, deterministic payload
validation, bounded suite accounting and durable RTT evidence.

The receiver remains polled. IRQ is sampled and reported around events, while
TX and RX completion are established from DW3110 status bits. This preserves the
existing evidence boundary around IRQ polarity, idle behaviour and event
clearing.

## Fixed roles

```text
Gateway CDK 760203854: transmitter
Node CDK    760222856: receiver
Direction: Gateway -> Node
```

## Fixed PHY profile

```text
Channel:              5
Preamble length:      128 symbols
PAC:                   8
TX preamble code:      9
RX preamble code:      9
SFD:                   DW 8-symbol
Data rate:             6.8 Mbps
PHR mode/rate:         standard
STS:                   disabled
PDOA:                  disabled
TX PG delay:           0x34
TX power register:     0xFDFDFDFD
Application payload:   24 bytes
On-air frame length:   26 bytes, including the DW3110-generated FCS
TX interval:           500 ms
Suite size:            20 frames
RX hardware timeout:   900000 UUS
RX host guard timeout: 1200 ms
```

The TX power register value is a development-board starting point. This proof
makes no calibrated-power, RF-performance or regulatory claim.

## Deterministic payload

| Offset | Length | Field |
|---:|---:|---|
| 0 | 4 | ASCII magic `TPRF` |
| 4 | 1 | Version `1` |
| 5 | 1 | Frame type `1` |
| 6 | 2 | Sequence, little-endian |
| 8 | 2 | Source ID, little-endian |
| 10 | 2 | Destination ID, little-endian |
| 12 | 4 | Gateway uptime in milliseconds, little-endian |
| 16 | 4 | Fixed pattern `A5 5A C3 3C` |
| 20 | 4 | Thin-Pod checksum over bytes 0 to 19 |

The DW3110 appends the two-byte PHY/MAC FCS. The receiver requires an exact
26-byte reported frame length, reads the 24 application bytes and validates all
fields and the application checksum.

## PASS criteria

A defensible passing record requires both of these structured summaries:

```text
TPRF_TX_SUMMARY role=gateway suite=<n> attempted=20 pass=20 fail=0 ... result=PASS
TPRF_RX_SUMMARY role=node suite=<n> valid=20 invalid=0 timeouts=0 errors=0 gaps=0 duplicates=0 ... result=PASS
```

The supplied checker also requires at least 20 individual passing TX records and
20 individual passing RX records. A TX-only PASS is insufficient.

## 1. Prepare the repository and apply the implementation patch

Open PowerShell:

```powershell
$Repo = "C:\ThinPod\Thin-Pod-Gateway-rev-0.1"
$Patch = "$HOME\Downloads\thinpod-dw3110-one-way-rf-proof.patch"
$ExpectedBase = "6988263aef77a63e214045627d8691e1f9ca2777"

Set-Location $Repo

git fetch --all --prune
git switch gateway-dw3110-rf-exchange
git pull --ff-only

git status --short --branch
$ActualBase = (git rev-parse HEAD).Trim()
"base_commit=$ActualBase"

if ($ActualBase -ne $ExpectedBase) {
    Write-Warning "The branch has moved beyond the recorded identity/initialisation milestone. Review git log before applying the patch."
}

if (-not (Test-Path $Patch)) {
    throw "Patch not found: $Patch"
}

git apply --check $Patch
if ($LASTEXITCODE -ne 0) {
    throw "git apply --check failed"
}

git apply --whitespace=nowarn $Patch
if ($LASTEXITCODE -ne 0) {
    throw "git apply failed"
}

git diff --check
if ($LASTEXITCODE -ne 0) {
    throw "git diff --check failed"
}

git status --short
git diff --stat
```

## 2. Establish the validated NCS and Qorvo environment

Continue in the same PowerShell session:

```powershell
$NcsRoot    = "C:\ncs\v3.3.1"
$NcsBin     = "C:\ncs\toolchains\936afb6332\opt\bin"
$NcsPython  = Join-Path $NcsBin "python.exe"
$ZephyrBase = Join-Path $NcsRoot "zephyr"
$ZephyrSdk  = "C:\ncs\toolchains\936afb6332\opt\zephyr-sdk"
$QorvoRoot  = "C:\Qorvo\DW3_QM33_SDK_1.1.0"

$GatewayApp = Join-Path $Repo "firmware\gateway\uwb_initiator_stage2_rf"
$NodeApp    = Join-Path $Repo "firmware\node\uwb_responder_stage2_rf"

$GatewayBuild = Join-Path $NcsRoot "build_thinpod_gateway_dw3110_rf"
$NodeBuild    = Join-Path $NcsRoot "build_thinpod_node_dw3110_rf"

$Evidence = Join-Path $Repo "logs\validation\dw3110-one-way-rf"
$GatewayLog = Join-Path $Evidence "gateway-dwm3001cdk-rf-rtt.log"
$NodeLog = Join-Path $Evidence "node-dwm3001cdk-rf-rtt.log"
$BuildHashFile = Join-Path $Evidence "build-artifact-sha256.txt"
$LogHashFile = Join-Path $Evidence "rtt-evidence-sha256.txt"
$ValidationReport = Join-Path $Repo "docs\validation\DW3110_One_Way_RF_Exchange_Validation.md"

$env:PYTHONHOME = $NcsBin
Remove-Item Env:PYTHONPATH -ErrorAction SilentlyContinue
Remove-Item Env:PYTHONNOUSERSITE -ErrorAction SilentlyContinue

$env:PATH = "$NcsBin;$env:PATH"
$env:ZEPHYR_BASE = $ZephyrBase
$env:ZEPHYR_TOOLCHAIN_VARIANT = "zephyr"
$env:ZEPHYR_SDK_INSTALL_DIR = $ZephyrSdk
$env:QORVO_DW3_SDK_ROOT = $QorvoRoot

$RequiredPaths = @(
    $NcsPython,
    $ZephyrBase,
    $ZephyrSdk,
    $QorvoRoot,
    $GatewayApp,
    $NodeApp
)

foreach ($Path in $RequiredPaths) {
    if (-not (Test-Path $Path)) {
        throw "Required path is missing: $Path"
    }
}

$ApiHeader = Get-ChildItem `
    (Join-Path $QorvoRoot "Drivers\API\Shared\dwt_uwb_driver") `
    -Recurse -Filter "deca_device_api.h" | Select-Object -First 1

$RegsHeader = Get-ChildItem `
    (Join-Path $QorvoRoot "Drivers\API\Shared\dwt_uwb_driver") `
    -Recurse -File |
    Where-Object { $_.Name -in @("deca_regs.h", "dw3000_deca_regs.h") } |
    Select-Object -First 1

if (-not $ApiHeader) {
    throw "deca_device_api.h was not found under the Qorvo driver root"
}

if (-not $RegsHeader) {
    throw "No DW3000 register-definition header was found under the Qorvo driver root"
}

Select-String -Path $ApiHeader.FullName -Pattern `
    "dwt_configure|dwt_configuretxrf|dwt_writetxdata|dwt_writetxfctrl|dwt_starttx|dwt_rxenable|dwt_readrxdata|dwt_setrxtimeout"

Select-String -Path $RegsHeader.FullName -Pattern `
    "SYS_STATUS_TXFRS|SYS_STATUS_RXFCG|SYS_STATUS_ALL_RX_TO|SYS_STATUS_ALL_RX_ERR|RX_FINFO_RXFLEN"

& $NcsPython -m py_compile `
    (Join-Path $Repo "scripts\check_dw3110_rf_exchange.py")

if ($LASTEXITCODE -ne 0) {
    throw "RF evidence checker syntax validation failed"
}
```

## 3. Build both roles from clean build directories

```powershell
Set-Location $NcsRoot

& $NcsPython -m west build `
    --sysbuild `
    -b decawave_dwm3001cdk/nrf52833 `
    -p always `
    -d $GatewayBuild `
    $GatewayApp

if ($LASTEXITCODE -ne 0) {
    throw "Gateway RF transmitter build failed"
}

& $NcsPython -m west build `
    --sysbuild `
    -b decawave_dwm3001cdk/nrf52833 `
    -p always `
    -d $NodeBuild `
    $NodeApp

if ($LASTEXITCODE -ne 0) {
    throw "Node RF receiver build failed"
}
```

## 4. Record build artefact hashes

```powershell
$GatewayElf = Join-Path $GatewayBuild `
    "uwb_initiator_stage2_rf\zephyr\zephyr.elf"
$GatewayHex = Join-Path $GatewayBuild "merged.hex"
$NodeElf = Join-Path $NodeBuild `
    "uwb_responder_stage2_rf\zephyr\zephyr.elf"
$NodeHex = Join-Path $NodeBuild "merged.hex"

$BuildArtifacts = @(
    @{ Label = "gateway zephyr.elf"; Path = $GatewayElf },
    @{ Label = "gateway merged.hex"; Path = $GatewayHex },
    @{ Label = "node zephyr.elf"; Path = $NodeElf },
    @{ Label = "node merged.hex"; Path = $NodeHex }
)

foreach ($Artifact in $BuildArtifacts) {
    if (-not (Test-Path $Artifact.Path)) {
        throw "Build artefact missing: $($Artifact.Path)"
    }
}

New-Item -ItemType Directory -Force $Evidence | Out-Null

$BuildHashLines = foreach ($Artifact in $BuildArtifacts) {
    $Hash = (Get-FileHash -Algorithm SHA256 $Artifact.Path).Hash
    "SHA256  $Hash  $($Artifact.Label)"
}

$BuildHashLines | Set-Content -Encoding ascii $BuildHashFile
Get-Content $BuildHashFile
```

## 5. Commit the implementation before collecting physical evidence

```powershell
Set-Location $Repo

git add `
    .gitattributes `
    docs/firmware/UWB_RF_Proof_Stage2.md `
    docs/firmware/DW3110_One_Way_RF_Exchange_Procedure.md `
    firmware/common/qorvo_dw3000/CMakeLists.txt `
    firmware/common/qorvo_dw3000/include/thinpod_dw3000_rf_proof.h `
    firmware/common/qorvo_dw3000/src/thinpod_dw3000_rf_proof.c `
    firmware/gateway/uwb_initiator_stage2_rf/src/main.c `
    firmware/node/uwb_responder_stage2_rf/src/main.c `
    scripts/check_dw3110_rf_exchange.py

git diff --cached --check
if ($LASTEXITCODE -ne 0) {
    throw "Staged source diff failed whitespace validation"
}

git diff --cached --stat
git commit -m "Implement DW3110 one-way RF frame proof"
if ($LASTEXITCODE -ne 0) {
    throw "Implementation commit failed"
}

$SourceCommit = (git rev-parse HEAD).Trim()
"source_commit=$SourceCommit"
```

The generated `build-artifact-sha256.txt` remains unstaged until the physical
validation record is complete.

## 6. Bench arrangement

Use the two independently identified DWM3001-CDKs:

```text
Gateway transmitter: J-Link serial 760203854
Node receiver:       J-Link serial 760222856
```

Place both boards on a clear bench with their antenna regions unobstructed and
separated. Avoid stacking the boards or placing the antennas directly against
metal or each other. Power and debug each board through its own J-Link/USB path.

## 7. Flash the Node receiver first

Return to the NCS PowerShell session:

```powershell
Set-Location $NcsRoot

& $NcsPython -m west flash `
    -d $NodeBuild `
    --runner jlink `
    --dev-id 760222856

if ($LASTEXITCODE -ne 0) {
    throw "Node receiver flash failed"
}
```

## 8. Start raw RTT capture for the Node receiver

Open a second PowerShell window. Run:

```powershell
$Repo = "C:\ThinPod\Thin-Pod-Gateway-rev-0.1"
$Evidence = Join-Path $Repo "logs\validation\dw3110-one-way-rf"
$NodeLog = Join-Path $Evidence "node-dwm3001cdk-rf-rtt.log"

New-Item -ItemType Directory -Force $Evidence | Out-Null
Remove-Item $NodeLog -ErrorAction SilentlyContinue

$RttLogger = (Get-Command JLinkRTTLogger.exe -ErrorAction SilentlyContinue).Source

if (-not $RttLogger) {
    $RttLogger = Get-ChildItem `
        "C:\Program Files\SEGGER", `
        "C:\Program Files (x86)\SEGGER" `
        -Recurse -Filter "JLinkRTTLogger.exe" `
        -ErrorAction SilentlyContinue |
        Sort-Object FullName -Descending |
        Select-Object -First 1 -ExpandProperty FullName
}

if (-not $RttLogger) {
    throw "JLinkRTTLogger.exe was not found"
}

& $RttLogger `
    -Device NRF52833_XXAA `
    -If SWD `
    -Speed 4000 `
    -USB 760222856 `
    -RTTChannel 0 `
    $NodeLog
```

Leave this logger attached while the transmitter is flashed and started.
Initial `TPRF_RX_WAIT` records are expected before the first valid frame and do
not count against the suite.

## 9. Flash the Gateway transmitter

In the original NCS PowerShell window:

```powershell
Set-Location $NcsRoot

& $NcsPython -m west flash `
    -d $GatewayBuild `
    --runner jlink `
    --dev-id 760203854

if ($LASTEXITCODE -ne 0) {
    throw "Gateway transmitter flash failed"
}
```

## 10. Start raw RTT capture for the Gateway transmitter

Open a third PowerShell window. Run:

```powershell
$Repo = "C:\ThinPod\Thin-Pod-Gateway-rev-0.1"
$Evidence = Join-Path $Repo "logs\validation\dw3110-one-way-rf"
$GatewayLog = Join-Path $Evidence "gateway-dwm3001cdk-rf-rtt.log"

New-Item -ItemType Directory -Force $Evidence | Out-Null
Remove-Item $GatewayLog -ErrorAction SilentlyContinue

$RttLogger = (Get-Command JLinkRTTLogger.exe -ErrorAction SilentlyContinue).Source

if (-not $RttLogger) {
    $RttLogger = Get-ChildItem `
        "C:\Program Files\SEGGER", `
        "C:\Program Files (x86)\SEGGER" `
        -Recurse -Filter "JLinkRTTLogger.exe" `
        -ErrorAction SilentlyContinue |
        Sort-Object FullName -Descending |
        Select-Object -First 1 -ExpandProperty FullName
}

if (-not $RttLogger) {
    throw "JLinkRTTLogger.exe was not found"
}

& $RttLogger `
    -Device NRF52833_XXAA `
    -If SWD `
    -Speed 4000 `
    -USB 760203854 `
    -RTTChannel 0 `
    $GatewayLog
```

Continue capture until both logs contain a complete summary. Stop each logger
with `Ctrl+C` only after the summary has been written.

Expected Gateway record:

```text
TPRF_TX_SUMMARY role=gateway suite=1 attempted=20 pass=20 fail=0 first_seq=<n> last_seq=<n+19> result=PASS
```

Expected Node record:

```text
TPRF_RX_SUMMARY role=node suite=1 valid=20 invalid=0 timeouts=0 errors=0 gaps=0 duplicates=0 first_seq=<n> last_seq=<n+19> result=PASS
```

## 11. Inspect and validate the captured evidence

Return to the original PowerShell window:

```powershell
$Repo = "C:\ThinPod\Thin-Pod-Gateway-rev-0.1"
$NcsBin = "C:\ncs\toolchains\936afb6332\opt\bin"
$NcsPython = Join-Path $NcsBin "python.exe"

$Evidence = Join-Path $Repo "logs\validation\dw3110-one-way-rf"
$GatewayLog = Join-Path $Evidence "gateway-dwm3001cdk-rf-rtt.log"
$NodeLog = Join-Path $Evidence "node-dwm3001cdk-rf-rtt.log"
$BuildHashFile = Join-Path $Evidence "build-artifact-sha256.txt"
$LogHashFile = Join-Path $Evidence "rtt-evidence-sha256.txt"
$ValidationReport = Join-Path $Repo "docs\validation\DW3110_One_Way_RF_Exchange_Validation.md"
$Checker = Join-Path $Repo "scripts\check_dw3110_rf_exchange.py"

Set-Location $Repo
$SourceCommit = (git rev-parse HEAD).Trim()

if (-not (Test-Path $GatewayLog)) {
    throw "Gateway RTT log is missing: $GatewayLog"
}

if (-not (Test-Path $NodeLog)) {
    throw "Node RTT log is missing: $NodeLog"
}

Select-String -Path $GatewayLog -Pattern `
    "TPRF_INIT|TPRF_PHY|TPRF_READY|TPRF_TX_SUMMARY|TPRF_FATAL"

Select-String -Path $NodeLog -Pattern `
    "TPRF_INIT|TPRF_PHY|TPRF_READY|TPRF_RX_SUMMARY|TPRF_FATAL"

& $NcsPython $Checker `
    --gateway-log $GatewayLog `
    --node-log $NodeLog `
    --source-commit $SourceCommit `
    --build-hash-file $BuildHashFile `
    --report $ValidationReport

if ($LASTEXITCODE -ne 0) {
    throw "DW3110 one-way RF evidence validation failed"
}
```

The checker writes the validation document only after the required TX and RX
PASS records are present.

## 12. Record raw RTT hashes

```powershell
$GatewayLogHash = (Get-FileHash -Algorithm SHA256 $GatewayLog).Hash
$NodeLogHash = (Get-FileHash -Algorithm SHA256 $NodeLog).Hash

@(
    "SHA256  $GatewayLogHash  gateway-dwm3001cdk-rf-rtt.log",
    "SHA256  $NodeLogHash  node-dwm3001cdk-rf-rtt.log"
) | Set-Content -Encoding ascii $LogHashFile

Get-Content $LogHashFile
Get-Content $ValidationReport
```

## 13. Stage the raw evidence without line-ending conversion

The patch adds this attribute before the logs are created:

```text
logs/validation/dw3110-one-way-rf/*.log -text -diff -whitespace
```

Confirm it and stage the evidence:

```powershell
Set-Location $Repo

$GatewayRelative = "logs/validation/dw3110-one-way-rf/gateway-dwm3001cdk-rf-rtt.log"
$NodeRelative = "logs/validation/dw3110-one-way-rf/node-dwm3001cdk-rf-rtt.log"

 git check-attr text diff whitespace -- `
    $GatewayRelative `
    $NodeRelative

 git add `
    logs/validation/dw3110-one-way-rf/build-artifact-sha256.txt `
    logs/validation/dw3110-one-way-rf/rtt-evidence-sha256.txt `
    $GatewayRelative `
    $NodeRelative `
    docs/validation/DW3110_One_Way_RF_Exchange_Validation.md

$GatewayWorktreeBlob = (git hash-object --no-filters $GatewayRelative).Trim()
$GatewayIndexBlob = (git rev-parse ":$GatewayRelative").Trim()
$NodeWorktreeBlob = (git hash-object --no-filters $NodeRelative).Trim()
$NodeIndexBlob = (git rev-parse ":$NodeRelative").Trim()

"gateway_worktree_blob=$GatewayWorktreeBlob"
"gateway_index_blob=$GatewayIndexBlob"
"node_worktree_blob=$NodeWorktreeBlob"
"node_index_blob=$NodeIndexBlob"

if ($GatewayWorktreeBlob -ne $GatewayIndexBlob) {
    throw "Gateway RTT bytes changed while entering the Git index"
}

if ($NodeWorktreeBlob -ne $NodeIndexBlob) {
    throw "Node RTT bytes changed while entering the Git index"
}

git diff --cached --check
if ($LASTEXITCODE -ne 0) {
    throw "Staged evidence diff failed validation"
}

git status --short
git diff --cached --stat
```

The leading spaces before `git` in the displayed block are harmless in
PowerShell. They may also be removed.

## 14. Commit and push the physical RF evidence

```powershell
Set-Location $Repo

git commit -m "Record DW3110 one-way RF exchange proof"
if ($LASTEXITCODE -ne 0) {
    throw "Evidence commit failed"
}

git status --short --branch
git log --oneline --decorate --max-count=5

git push origin gateway-dw3110-rf-exchange
if ($LASTEXITCODE -ne 0) {
    throw "Branch push failed"
}

git status
```

## Interpretation

A full PASS supports this claim:

```text
The Thin-Pod Gateway-side DWM3001-CDK transmitted deterministic UWB frames
through its internal DW3110, and the Node-side DWM3001-CDK received those
frames through its internal DW3110, verified the exact frame length and payload,
and completed a repeatable 20-frame one-way RF exchange.
```

It does not support claims about the reverse path, ranging, vibration-window or
telemetry transport, IRQ-driven receive operation, RF performance, security or
regulatory compliance.

## Immediate follow-on actions after PASS

1. Preserve the one-way proof unchanged as a known-good RF baseline.
2. Add the reverse one-way direction as a separate, equally bounded gate if the
   return path is needed before ranging.
3. Introduce the smallest Thin-Pod record payload only after the basic RF path
   remains repeatable under clean power cycles.
