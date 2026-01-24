#version 460
#extension GL_EXT_nonuniform_qualifier : require
#extension GL_GOOGLE_include_directive : require

#include "gx-vk-shd-bindless.glslh"

layout(location = 0) in vec2 in_uv;

layout(location = 0) out vec4 frag_out;

void main() {
    GxShaderDataModel model = models[pc.model_index];
    GxShaderDataMaterial material = materials[model.material_index];

    // Sample albedo texture and multiply by factor
    vec4 colour = texture(
        sampler2D(textures_2d[nonuniformEXT(material.albedo_texture_index)], samplers[nonuniformEXT(material.albedo_sampler_index)]),
        in_uv) * material.albedo_factor;

    // Alpha test
    if (colour.a <= material.alpha_cutoff_occlusion_strength_reserved.x) {
        discard;
    }

    frag_out = colour;
}
