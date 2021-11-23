#include "gx-dxr-shd-root-signature.hlsli"

struct PSInput
{
    float4 position : SV_POSITION;
    float3 normal : NORMAL;
    float4 tangent : TANGENT;
    float2 uv : TEXCOORD;
};

[RootSignature(G_BUFFERS_FILLER_ROOT_SIGNATURE)]
PSInput main(float3 position : POSITION, float3 normal : NORMAL, float4 tangent: TANGENT, float2 uv : TEXCOORD)
{
    PSInput result;

    result.position = float4(position, 1.0);
    result.normal = normal;
    result.tangent = tangent;
    result.uv = uv;

    return result;
}