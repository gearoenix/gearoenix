#include "gx-dxr-shd-root-signature.hlsli"

struct PSInput
{
    float3 position : SV_POSITION;
    float3 normal : SV_NORMAL;
    float4 tangent : SV_TANGENT;
    float2 uv : TEXCOORD;
};

[RootSignature(G_BUFFERS_FILLER_ROOT_SIGNATURE)]
PSInput main(float3 position : POSITION, float3 normal : NORMAL, float4 tangent: TANGENT, float2 uv : TEXCOORD)
{
    PSInput result;

    result.position = position + ((normal + tangent.xyz) * 0.001);
    result.normal = normal;
    result.tangent = tangent;
    result.uv = uv;

    return result;
}