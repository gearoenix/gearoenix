FROM mcr.microsoft.com/windows/servercore:ltsc2022
LABEL authors="Thany Noroozpourthanyabady"
SHELL ["powershell", "-Command", "\"$ErrorActionPreference = 'Stop'; $ProgressPreference = 'Continue'; $verbosePreference='Continue';\""]

RUN Invoke-WebRequest "https://github.com/Kitware/CMake/releases/download/v3.29.3/cmake-3.29.3-windows-x86_64.zip" -OutFile "cmake.zip"
RUN Expand-Archive "cmake.zip" -DestinationPath "cmake"
RUN Remove-Item -Path "cmake.zip"
RUN $new_path = (Get-Location).Path + "'\cmake\cmake-3.29.3-windows-x86_64\bin;'"; \
    $new_path += [Environment]::GetEnvironmentVariable('"Path"', '"Machine"'); \
    [Environment]::SetEnvironmentVariable('"Path"', $new_path, '"Machine"');
RUN [Environment]::SetEnvironmentVariable('"CMAKE_BUILD_PARALLEL_LEVEL"', 32, '"Machine"')

RUN Invoke-WebRequest '"https://aka.ms/vs/17/release/vc_redist.x64.exe"' -OutFile '"vc_redist.exe"'
RUN ./vc_redist.exe /install /quiet /norestart
RUN Remove-Item -Path "vc_redist.exe"

RUN Invoke-WebRequest '"https://sdk.lunarg.com/sdk/download/latest/windows/vulkan-sdk.exe"' -OutFile '"vulkan.exe"'
RUN ./vulkan.exe --accept-licenses --default-answer --confirm-command install
RUN Remove-Item -Path "vulkan.exe"

RUN Write-Host "'Path: $Env:Path'"

ENTRYPOINT ["powershell", "-Command", "Wait-Event"]
