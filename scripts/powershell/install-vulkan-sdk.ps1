$ErrorView = 'NormalView'
New-Item -ItemType Directory -Force -Path build
Set-Location -Path build
Invoke-WebRequest "https://sdk.lunarg.com/sdk/download/latest/windows/vulkan-sdk.exe" -OutFile build\vulkan-sdk.exe
.\build\vulkan-sdk.exe /S
