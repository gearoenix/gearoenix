param (
    [string]$BuildType
)

if ($null -eq $BuildType -or ($BuildType -ne "Debug" -and $BuildType -ne "Release" -and $BuildType -ne "MinRelSize")) {
    $BuildType = "MinRelSize"
}


New-Item -ItemType Directory -Force -Path "build"

. "$Env:EMSDK\emsdk_env.ps1"

emcmake cmake -B "build" -DCMAKE_BUILD_TYPE="$BuildType" -DGX_TEST_ENABLED=OFF ..
if (-not $?) {
    throw "Failed to config the project."
}

cmake --build "build" --config "$BuildType" --parallel $([Environment]::ProcessorCount)
if (-not $?) {
    throw "Failed to build the project."
}

python ./run-server.py
if (-not $?) {
    throw "Failed to run the python."
}