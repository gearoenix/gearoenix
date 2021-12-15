#include "gx-dxr-shd-g-buffer-common.hlsli"

[RootSignature(G_BUFFERS_FILLER_ROOT_SIGNATURE)]
float4 main(PSInput input) : SV_TARGET
{
    return tex2ds[0].Sample(samplers, input.uv * 10.0);
}
