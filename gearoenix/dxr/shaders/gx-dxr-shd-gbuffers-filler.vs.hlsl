#include "gx-dxr-shd-gbuffers-filler.hlsli"

[RootSignature(G_BUFFERS_FILLER_ROOT_SIGNATURE)]
PSInput main(float3 position : POSITION, float3 normal : NORMAL, float4 tangent: TANGENT, float2 uv : TEXCOORD)
{
    PSInput result;
    result.world_position = mul(ModelUniform.model, float4(position, 1.0));
    result.position = mul(CameraUniform.view_projection, result.world_position);
    result.normal = mul(ModelUniform.transposed_reversed_model, float4(normal, 1.0)).xyz;
    result.tangent = tangent;
    result.uv = uv;

    return result;
}