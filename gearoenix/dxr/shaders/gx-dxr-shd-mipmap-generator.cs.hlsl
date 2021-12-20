cbuffer root_constants : register(b0, space0) { 
    uint src_texture_mip_level; 
    float2 texel_size; // 1 / dst-dimention 
}
Texture2D<float4> src_texture : register(t0, space0);
RWTexture2D<float4> dst_texture : register(u0, space0);
SamplerState clamp_sampler : register(s0, space0);

[RootSignature("RootFlags(0), " \
    "RootConstants(b0, space = 0, num32BitConstants = 3), " \
    "DescriptorTable(SRV(t0, space = 0, numDescriptors = 1)), " \
    "DescriptorTable(UAV(u0, space = 0, numDescriptors = 1)), " \
    "StaticSampler(s0, space = 0," \
        "addressU = TEXTURE_ADDRESS_CLAMP, " \
        "addressV = TEXTURE_ADDRESS_CLAMP, " \
        "addressW = TEXTURE_ADDRESS_CLAMP, " \
        "filter = FILTER_MIN_MAG_MIP_LINEAR)")]
[numthreads(8, 8, 1)]
void main(uint3 dtid : SV_DispatchThreadID)
{
    float4 colour = src_texture.SampleLevel(clamp_sampler, texel_size * (float2(dtid.xy) + float2(0.25, 0.25)), src_texture_mip_level);
    colour += src_texture.SampleLevel(clamp_sampler, texel_size * (float2(dtid.xy) + float2(0.75, 0.25)), src_texture_mip_level);
    colour += src_texture.SampleLevel(clamp_sampler, texel_size * (float2(dtid.xy) + float2(0.25, 0.75)), src_texture_mip_level);
    colour += src_texture.SampleLevel(clamp_sampler, texel_size * (float2(dtid.xy) + float2(0.75, 0.75)), src_texture_mip_level);
	dst_texture[dtid.xy] = colour * 0.25;
}