#version 460
#extension GL_EXT_nonuniform_qualifier : require
#extension GL_GOOGLE_include_directive : require

#include "gx-vk-shd-bindless.glslh"

layout(location = 0) in vec3 in_position;
layout(location = 1) in vec3 in_normal;
layout(location = 2) in vec4 in_tangent;
layout(location = 3) in vec2 in_uv;

layout(location = 0) out vec3 out_pos;

void main() {
    GxShaderDataCamera camera = cameras[pc.camera_index];

    out_pos = in_position;

    vec3 p = in_position;
    p *= camera.position_far.w / 1.7320508075688773;
    p += camera.position_far.xyz;
    gl_Position = camera.view_projection * vec4(p, 1.0);
}