#version 460
#extension GL_EXT_nonuniform_qualifier : require
#extension GL_GOOGLE_include_directive : require

#include "gx-vk-shd-bindless.glslh"

layout(location = 0) in vec2 in_uv;

void main() {
    const GxShaderDataMaterial material = materials[pc.material_index];

    const float alpha = sample_texture_2d(material.albedo_normal_texture_sampler_index.xy, in_uv).a * material.albedo_factor.a;

    if (alpha <= material.alpha_cutoff_occlusion_strength_reserved_reserved.x) {
        discard;
    }
}