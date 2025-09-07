[CmdletBinding()]
param (
    [ValidateSet("debug", "release")]
    [string]$BuildType = "release",
    [switch]$NoServer
)

$ErrorActionPreference = "Stop"
$PSNativeCommandUseErrorActionPreference = $true

$ScriptDir = $PSScriptRoot
$ProjectRoot = (Resolve-Path (Join-Path $ScriptDir "..")).Path

$null = Get-Command cmake -ErrorAction Stop
$null = Get-Command python -ErrorAction Stop
$null = Get-Command ninja -ErrorAction Stop

if (-not $Env:VCPKG_ROOT) {
    throw "VCPKG_ROOT environment variable is not set. Please install vcpkg and set VCPKG_ROOT."
}

if (-not $Env:EMSDK) {
    throw "EMSDK environment variable is not set. Please install Emscripten and set EMSDK."
}
$EMSDKScript = Join-Path $Env:EMSDK "emsdk_env.ps1"
if (-not (Test-Path -Path $EMSDKScript)) {
    throw "emsdk_env.ps1 not found at: $EMSDKScript"
}
. $EMSDKScript
$null = Get-Command emcmake -ErrorAction Stop

try {
    Push-Location $ProjectRoot
    cmake --fresh --preset "wasm-$BuildType" .
} catch {
    throw "Failed to configure the project: $($_.Exception.Message)"
} finally {
    Pop-Location
}

$ThreadsCount = [Math]::Max(8, [Int][Math]::Ceiling([Environment]::ProcessorCount * 1.5))

try {
    Push-Location $ProjectRoot
    cmake --build --preset "wasm-$BuildType" --parallel $ThreadsCount
} catch {
    throw "Failed to build the project: $($_.Exception.Message)"
} finally {
    Pop-Location
}

if (-not $NoServer) {
    try {
        Push-Location $ScriptDir
        python "run-server.py"
    } catch {
        throw "Failed to run the server: $($_.Exception.Message)"
    } finally {
        Pop-Location
    }
}