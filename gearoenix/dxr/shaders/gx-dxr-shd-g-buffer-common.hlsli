#pragma once

#include "gx-dxr-shd-common.hpp"

#define G_BUFFERS_FILLER_ROOT_SIGNATURE                                    \
	"RootFlags(ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT), "                      \
	"CBV(b0, space = 0), "                                                 \
	"DescriptorTable(SRV(t0, space = 0, numDescriptors = unbounded)), "    \
    "DescriptorTable(Sampler(s0, space = 0, numDescriptors = unbounded))"

struct PSInput
{
    float4 position : SV_POSITION;
    float3 normal : NORMAL;
    float4 tangent : TANGENT;
    float2 uv : TEXCOORD;
};

ConstantBuffer<MeshUniform> MeshUniform : register(b0, space0);
Texture2D<float4> tex2ds[] : register(t0, space0);
sampler samplers[] : register(s0, space0);