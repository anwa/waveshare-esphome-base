<#
.SYNOPSIS
Validates and optionally compiles example.yaml, which builds the base on its own.

.PARAMETER ConfigOnly
Run only `esphome config`, without compiling.

.PARAMETER Setup
Create the project-local .venv and install the pinned ESPHome version first.
#>
[CmdletBinding()]
param(
    [switch]$ConfigOnly,
    [switch]$Setup
)

Set-StrictMode -Version Latest
$ErrorActionPreference = "Stop"

$RepoRoot = (Resolve-Path (Join-Path $PSScriptRoot "..")).Path
$VenvDir = Join-Path $RepoRoot ".venv"
$VenvPython = Join-Path $VenvDir "Scripts/python.exe"
$Requirements = Join-Path $RepoRoot "requirements-dev.txt"
$Example = "example.yaml"

function Write-Step([string]$Message) { Write-Host "[ STEP ] $Message" -ForegroundColor Cyan }
function Write-Ok([string]$Message)   { Write-Host "[  OK  ] $Message" -ForegroundColor Green }

function Invoke-Checked([string]$Description, [scriptblock]$Command) {
    & $Command
    if ($LASTEXITCODE -ne 0) {
        throw "$Description failed with exit code $LASTEXITCODE."
    }
}

if ($Setup -or -not (Test-Path -LiteralPath $VenvPython -PathType Leaf)) {
    if (-not (Test-Path -LiteralPath $VenvPython -PathType Leaf)) {
        Write-Step "Creating virtual environment with Python 3.13"
        Invoke-Checked "Creating the virtual environment" { & py -3.13 -m venv $VenvDir }
    }
    Write-Step "Installing pinned development requirements"
    Invoke-Checked "Installing requirements" { & $VenvPython -m pip install --quiet -r $Requirements }
}

# ESPHome prints UTF-8 glyph lists; force UTF-8 output on Windows consoles.
$env:PYTHONIOENCODING = "utf-8"

Push-Location $RepoRoot
try {
    Write-Step "esphome config $Example"
    Invoke-Checked "esphome config" { & $VenvPython -m esphome config $Example | Out-Null }
    Write-Ok "Configuration is valid"

    if (-not $ConfigOnly) {
        Write-Step "esphome compile $Example"
        Invoke-Checked "esphome compile" { & $VenvPython -m esphome compile $Example }
        Write-Ok "Compilation succeeded"
    }
}
finally {
    Pop-Location
}
