New-Item -ItemType Directory -Force -Path build
Set-Location -Path build
$cmake_ver = "3.19.2"
Invoke-WebRequest https://github.com/Kitware/CMake/releases/download/v${cmake_ver}/cmake-${cmake_ver}-win64-x64.zip -OutFile build\cmake.zip
Expand-Archive build\cmake.zip -DestinationPath build\cmake
Remove-Item –path build\cmake.zip
Add-Content -Path "${GITHUB_PATH}" -Value "$((Get-Item .).FullName)\build\cmake\cmake-${cmake_ver}-win64-x64\bin"
Add-Content -Path "${GITHUB_ENV}" -Value "CMAKE_BUILD_PARALLEL_LEVEL=$((Get-CimInstance Win32_ComputerSystem).NumberOfLogicalProcessors * 8)"
