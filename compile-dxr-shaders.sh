#!/bin/bash
../dxc/bin/dxc.exe -T lib_6_5 -Qembed_debug -nologo -Zi -Od -Fo ../assets/gx-dxr-shd-g-buffer.vs.cso gearoenix/dxr/shaders/gx-dxr-shd-g-buffer.vs.hlsl
../dxc/bin/dxc.exe -T lib_6_5 -Qembed_debug -nologo -Zi -Od -Fo ../assets/gx-dxr-shd-g-buffer.ps.cso gearoenix/dxr/shaders/gx-dxr-shd-g-buffer.ps.hlsl