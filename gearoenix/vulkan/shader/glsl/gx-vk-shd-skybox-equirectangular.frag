#version 460
#extension GL_EXT_nonuniform_qualifier : require
#extension GL_GOOGLE_include_directive : require

#include "gx-vk-shd-bindless.glslh"

layout(location = 0) in vec3 in_pos;

layout(location = 0) out vec4 frag_out;

const vec2 inv_atan = vec2(0.1591, 0.3183);

void main() {
    GxShaderDataMaterial material = materials[pc.material_index];

    vec3 v = normalize(in_pos);
    vec2 uv = vec2(atan(v.y, v.x), asin(v.z)) * inv_atan + 0.5;

    frag_out = texture(sampler2D(
        textures_2d[nonuniformEXT(material.albedo_texture_index)],
        samplers[nonuniformEXT(material.albedo_sampler_index)]), uv) * material.albedo_factor;
}