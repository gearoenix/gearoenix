#version 460
#extension GL_EXT_nonuniform_qualifier : require
#extension GL_GOOGLE_include_directive : require

#include "gx-vk-shd-bindless.glslh"

// ========== Specialization Constants ==========
layout(constant_id = 0) const bool GX_SPEC_HAS_BONES = false;

layout(location = 0) in vec3 in_position;
layout(location = 1) in vec3 in_normal;
layout(location = 2) in vec4 in_tangent;
layout(location = 3) in vec2 in_uv;
layout(location = 4) in vec4 in_bones_weight;
layout(location = 5) in vec4 in_bones_indices;

layout(location = 0) out vec3 out_pos;
layout(location = 1) out vec3 out_nrm;
layout(location = 2) out vec3 out_tng;
layout(location = 3) out vec3 out_btg;
layout(location = 4) out vec2 out_uv;

void main() {
    out_uv = in_uv;

    GxShaderDataModel model = models[pc.model_index];
    GxShaderDataCamera camera = cameras[pc.camera_index];

    mat4 m;
    mat4 inv_trn_m;

    if (GX_SPEC_HAS_BONES) {
        ivec4 bone_index = ivec4(in_bones_indices) + int(model.bones_begin_index);
        m = (bones[bone_index.x].m * in_bones_weight.x) +
            (bones[bone_index.y].m * in_bones_weight.y) +
            (bones[bone_index.z].m * in_bones_weight.z) +
            (bones[bone_index.w].m * in_bones_weight.w);
        inv_trn_m = (bones[bone_index.x].inv_transpose_m * in_bones_weight.x) +
                (bones[bone_index.y].inv_transpose_m * in_bones_weight.y) +
                (bones[bone_index.z].inv_transpose_m * in_bones_weight.z) +
                (bones[bone_index.w].inv_transpose_m * in_bones_weight.w);
    } else {
        m = model.m;
        inv_trn_m = model.inv_transpose_m;
    }

    const vec4 pos = m * vec4(in_position, 1.0);
    out_pos = pos.xyz;
    out_nrm = normalize((inv_trn_m * vec4(in_normal, 0.0)).xyz);
    out_tng = normalize((inv_trn_m * vec4(in_tangent.xyz, 0.0)).xyz);
    out_btg = normalize(cross(out_nrm, out_tng) * in_tangent.w);
    gl_Position = camera.view_projection * pos;
}