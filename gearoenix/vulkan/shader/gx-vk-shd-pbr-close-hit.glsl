#version 460
#extension GL_EXT_ray_tracing : require
#extension GL_EXT_nonuniform_qualifier : enable
#extension GL_EXT_scalar_block_layout : enable
#extension GL_GOOGLE_include_directive : enable

#include "gx-vk-shd-bindings.hpp"

struct Vertex {
    vec3 position;
    vec3 normal;
    vec4 tangent;
    vec2 uv;
};

struct Material {
    uint albedo_texture_index;
    uint normal_texture_index;
    uint metalic_roughness_texture_index;
    uint emission_texture_index;
    uint ambient_oclussion_texture_index;
};

struct Mesh {
    uint vertex_index;
    uint index_index;
    uint material_index;
};

hitAttributeEXT vec2 attributes;

layout(location = 0) rayPayloadInEXT hitPayload prd;
layout(location = 1) rayPayloadEXT bool isShadowed;


layout(set = 1, binding = GX_VK_BIND_RAY_VERTICES) readonly buffer VerticisBuffer { Vertex vertices[]; };
layout(set = 1, binding = GX_VK_BIND_RAY_INDICES) readonly buffer IndicesBuffer { uint indices[]; };
layout(set = 1, binding = GX_VK_BIND_RAY_MATERIALS) readonly buffer MaterialBuffer { Material materials[]; };
layout(set = 1, binding = GX_VK_BIND_RAY_MESH) readonly buffer MeshBuffer { Mesh meshes[]; };
layout(set = 0, binding = GX_VK_BIND_RAY_TLAS) uniform accelerationStructureEXT tlas;
layout(set = 1, binding = GX_VK_BIND_RAY_2D_TEXTURES) uniform sampler2D textures[];
layout(set = 1, binding = GX_VK_BIND_RAY_CUBE_TEXTURES) uniform samplerCube cube_textures[];