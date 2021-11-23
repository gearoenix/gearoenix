#include "gx-dxr-shd-root-signature.hlsli"

struct PSInput
{
    float4 position : SV_POSITION;
    float3 norml : NORMAL;
    float4 tangent : TANGENT;
    float2 uv : TEXCOORD;
};

[RootSignature(G_BUFFERS_FILLER_ROOT_SIGNATURE)]
float4 main(PSInput input) : SV_TARGET
{
    return float4(input.position.xyz * 0.001, 1.0);
}
