#version 460
#extension GL_EXT_nonuniform_qualifier : require
#extension GL_GOOGLE_include_directive : require

#include "gx-vk-shd-bindless.glslh"

// ========== Specialization Constants ==========
layout(constant_id = 0) const bool GX_SPEC_HAS_BONES = false;

// ========== Vertex Shader ==========
#ifdef VERTEX_SHADER

layout(location = 0) in vec3 in_position;
layout(location = 3) in vec2 in_uv;
layout(location = 4) in vec4 in_bones_weight;
layout(location = 5) in vec4 in_bones_indices;

layout(location = 0) out vec2 out_uv;

void main() {
    out_uv = in_uv;

    if (GX_SPEC_HAS_BONES) {
        GxShaderDataModel model = models[pc.model_index];
        GxShaderDataCamera camera = cameras[pc.camera_index];

        ivec4 bone_index = ivec4(in_bones_indices) + int(model.bones_begin_index);
        mat4 m = (bones[bone_index.x].m * in_bones_weight.x) +
                 (bones[bone_index.y].m * in_bones_weight.y) +
                 (bones[bone_index.z].m * in_bones_weight.z) +
                 (bones[bone_index.w].m * in_bones_weight.w);

        gl_Position = camera.view_projection * m * vec4(in_position, 1.0);
    } else {
        GxShaderDataCameraJointModel cjm = cameras_joint_models[pc.camera_model_index];
        gl_Position = cjm.mvp * vec4(in_position, 1.0);
    }
}

#endif // VERTEX_SHADER

// ========== Fragment Shader ==========
#ifdef FRAGMENT_SHADER

layout(location = 0) in vec2 in_uv;

void main() {
    GxShaderDataModel model = models[pc.model_index];
    GxShaderDataMaterial material = materials[model.material_index];

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

#endif // FRAGMENT_SHADER