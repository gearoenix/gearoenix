#version 460
#extension GL_EXT_nonuniform_qualifier : require
#extension GL_GOOGLE_include_directive : require

#include "gx-vk-shd-bindless.glslh"

layout(location = 0) in vec3 in_pos;

layout(location = 0) out vec4 frag_out;

const vec2 inv_atan = vec2(0.1591, 0.3183);

void main() {
    const GxShaderDataMaterial material = materials[pc.material_index];

    const vec3 v = normalize(in_pos);
    const vec2 uv = 0.5 - vec2(atan(v.y, v.x), asin(v.z)) * inv_atan;

    frag_out = sample_texture_2d(material.albedo_normal_texture_sampler_index.xy, uv) * material.albedo_factor;
}