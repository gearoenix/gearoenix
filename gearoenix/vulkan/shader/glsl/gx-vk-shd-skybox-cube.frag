#version 460
#extension GL_EXT_nonuniform_qualifier : require
#extension GL_GOOGLE_include_directive : require

#include "gx-vk-shd-bindless.glslh"

layout(location = 0) in vec3 in_pos;

layout(location = 0) out vec4 frag_out;

const vec2 inv_atan = vec2(0.1591, 0.3183);

void main() {
    const GxShaderDataMaterial material = materials[pc.material_index];

    frag_out = sample_texture_cube(material.albedo_normal_texture_sampler_index.xy, normalize(in_pos));
}