#!/bin/bash
../dxc/bin/x64/dxc.exe -T vs_6_5 -Qembed_debug -nologo -Zi -Od -Fo ../assets/gx-dxr-shd-g-buffer.vs.cso gearoenix/dxr/shaders/gx-dxr-shd-g-buffer.vs.hlsl
../dxc/bin/x64/dxc.exe -T ps_6_5 -Qembed_debug -nologo -Zi -Od -Fo ../assets/gx-dxr-shd-g-buffer.ps.cso gearoenix/dxr/shaders/gx-dxr-shd-g-buffer.ps.hlsl