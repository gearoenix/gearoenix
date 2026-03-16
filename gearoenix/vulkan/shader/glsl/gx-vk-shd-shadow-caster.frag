#version 460
#extension GL_EXT_nonuniform_qualifier : require
#extension GL_GOOGLE_include_directive : require

#include "gx-vk-shd-bindless.glslh"

layout(location = 0) in vec2 in_uv;

void main() {
    GxShaderDataModel model = models[pc.model_index];
    GxShaderDataMaterial material = materials[pc.material_index];

    // Sample albedo for alpha testing
    float alpha = texture(
        sampler2D(textures_2d[nonuniformEXT(material.albedo_texture_index)], samplers[nonuniformEXT(material.albedo_sampler_index)]),
        in_uv).a * material.albedo_factor.a;

    // Alpha test - discard fragments below cutoff
    if (alpha <= material.alpha_cutoff_occlusion_strength_reserved.x) {
        discard;
    }

    // Depth is written automatically - no color output needed for shadow maps
}