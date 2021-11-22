#include "gx-dxr-shd-root-signature.hlsli"

struct PSInput
{
    float3 position : SV_POSITION;
    float3 norml : SV_NORMAL;
    float4 tangent : SV_TANGENT;
    float2 uv : TEXCOORD;
};

[RootSignature(G_BUFFERS_FILLER_ROOT_SIGNATURE)]
float4 main(PSInput input) : SV_TARGET
{
    return float4(input.position, 1.0);
}
