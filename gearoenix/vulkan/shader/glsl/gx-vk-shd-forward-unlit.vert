#version 460
#extension GL_EXT_nonuniform_qualifier : require
#extension GL_GOOGLE_include_directive : require

#include "gx-vk-shd-bindless.glslh"

// ========== Specialization Constants ==========
layout(constant_id = 0) const bool GX_SPEC_HAS_BONES = false;

layout(location = 0) in vec3 in_position;
layout(location = 3) in vec2 in_uv;
layout(location = 4) in vec4 in_bones_weight;
layout(location = 5) in vec4 in_bones_indices;

layout(location = 0) out vec2 out_uv;

void main() {
    GxShaderDataModel model = models[pc.model_index];
    GxShaderDataCamera camera = cameras[pc.camera_index];
    GxShaderDataMaterial material = materials[pc.material_index];

    // uv_transform stored in emission_roughness_factor: xy = scale, zw = offset
    out_uv = in_uv * material.emission_roughness_factor.xy + material.emission_roughness_factor.zw;

    if (GX_SPEC_HAS_BONES) {
        ivec4 bone_index = ivec4(in_bones_indices) + int(model.bones_begin_index);
        mat4 m = (bones[bone_index.x].m * in_bones_weight.x) +
                 (bones[bone_index.y].m * in_bones_weight.y) +
                 (bones[bone_index.z].m * in_bones_weight.z) +
                 (bones[bone_index.w].m * in_bones_weight.w);

        gl_Position = camera.view_projection * m * vec4(in_position, 1.0);
    } else {
        gl_Position = camera.view_projection * model.m * vec4(in_position, 1.0);
    }
}
