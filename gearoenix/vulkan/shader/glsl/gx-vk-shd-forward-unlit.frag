#version 460
#extension GL_EXT_nonuniform_qualifier : require
#extension GL_GOOGLE_include_directive : require

#include "gx-vk-shd-bindless.glslh"

layout(location = 0) in vec2 in_uv;

layout(location = 0) out vec4 frag_out;

void main() {
    const GxShaderDataMaterial material = materials[pc.material_index];

    const vec4 colour = sample_texture_2d(material.albedo_normal_texture_sampler_index.xy, in_uv) * material.albedo_factor;

    if (colour.a <= material.alpha_cutoff_occlusion_strength_reserved_reserved.x) {
        discard;
    }

    frag_out = colour;
}
