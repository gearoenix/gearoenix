#include "gx-mtl-shd-common.hpp"
#include <metal_stdlib>

struct Vertex {
    float3 position [[ attribute(0) ]];
    float3 normal [[ attribute(1) ]];
    float4 tangent [[ attribute(2) ]];
    float2 uv [[ attribute(3) ]];
};

struct GBuffersFillerVertexShaderOut {
    float4 position [[position]];
    float3 world_position;
    float3 normal;
    float3 tangent;
    float3 bitangent;
    float3 reflection;
    float2 uv;
};

struct GBuffersFillerShaderArgs {
    device gearoenix::metal::ModelUniform* model [[id(GEAROENIX_METAL_GBUFFERS_FILLER_MODEL_UNIFORM_BIND_ID)]];
    metal::texture2d<float> albedo [[id(GEAROENIX_METAL_GBUFFERS_FILLER_ALBEDO_TEXTURE_BIND_ID)]];
    metal::texture2d<float> normal_metallic [[id(GEAROENIX_METAL_GBUFFERS_FILLER_NORMAL_METALLIC_TEXTURE_BIND_ID)]];
    metal::texture2d<float> emission_roughness [[id(GEAROENIX_METAL_GBUFFERS_FILLER_EMISSION_ROUGHNESS_TEXTURE_BIND_ID)]];
    metal::sampler texture_sampler [[id(GEAROENIX_METAL_GBUFFERS_FILLER_SAMPLER_BIND_ID)]];
};

vertex GBuffersFillerVertexShaderOut gbuffers_filler_vertex_shader(
    Vertex in [[stage_in]],
    constant gearoenix::metal::CameraUniform& camera [[buffer(GEAROENIX_METAL_GBUFFERS_FILLER_CAMERA_UNIFORM_BIND_INDEX)]],
    device GBuffersFillerShaderArgs& args [[buffer(GEAROENIX_METAL_GBUFFERS_FILLER_ARGUMENT_BUFFER_BIND_INDEX)]]) {

    GBuffersFillerVertexShaderOut out;

    float4 position = float4(in.position, 1.0);
    float4 world_position = args.model->model * position;
    out.world_position = world_position.xyz;
    out.position = camera.view_projection * world_position;
    out.normal = (args.model->transposed_reversed_model * float4(in.normal, 0.0)).xyz;
    out.tangent = (args.model->transposed_reversed_model * float4(in.tangent.xyz, 0.0)).xyz;
    out.bitangent = metal::cross(out.normal, out.tangent) * in.tangent.w;
    float3 v = out.world_position - camera.position.xyz;
    out.reflection = metal::reflect(v, out.normal);
    out.uv = in.uv;

    return out;
}

// declaring pipeline shader constants 
// constant int<type> <name> [[function_constant(100)]];

fragment float4 gbuffers_filler_fragment_shader(
    GBuffersFillerVertexShaderOut in [[stage_in]],
    constant gearoenix::metal::CameraUniform& camera [[buffer(GEAROENIX_METAL_GBUFFERS_FILLER_CAMERA_UNIFORM_BIND_INDEX)]],
    device GBuffersFillerShaderArgs& args [[buffer(GEAROENIX_METAL_GBUFFERS_FILLER_ARGUMENT_BUFFER_BIND_INDEX)]]) {
    return args.albedo.sample(args.texture_sampler, in.uv) * args.model->colour_factor;
}
