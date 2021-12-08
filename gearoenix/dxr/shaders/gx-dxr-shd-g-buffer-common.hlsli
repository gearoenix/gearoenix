#pragma once

#define G_BUFFERS_FILLER_ROOT_SIGNATURE \
	"RootFlags(ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT), " \
	"CBV(b0, space = 0), " \
	"DescriptorTable(SRV(t0, space = 1, numDescriptors = unbounded))"

struct PSInput
{
    float4 position : SV_POSITION;
    float3 normal : NORMAL;
    float4 tangent : TANGENT;
    float2 uv : TEXCOORD;
};

cbuffer MeshBuffer : register(b0, space0)
{
    float4 colour_factor;
    float4 emission_factor__alpha_cutoff;
    float4 normal_scale__occlusion_strength;
    float4 metallic_factor__roughness_factor__radiance_lod_coefficient;
};

Texture2D<float4> tex2ds[] : register(t0, space1);