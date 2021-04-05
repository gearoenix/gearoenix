#version 460
#extension GL_EXT_ray_tracing : require
#extension GL_EXT_nonuniform_qualifier : enable
#extension GL_EXT_scalar_block_layout : enable
#extension GL_GOOGLE_include_directive : enable

#include "../gx-vk-shd-bindings.hpp"
#include "gx-vk-shd-common.glsl"
#include "gx-vk-shd-sampling.glsl"

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

    vec4 albedo_factor;
    vec3 emissive_factor;
};

struct Mesh {
    uint vertex_index;
    uint index_index;
    uint material_index;
};

hitAttributeEXT vec2 attribs;

layout(location = 0) rayPayloadInEXT hitPayload prd;
layout(location = 1) rayPayloadEXT bool isShadowed;


layout(set = 0, binding = GX_VK_BIND_RAY_VERTICES, scalar) readonly buffer VerticisBuffer { Vertex vertices[]; };
layout(set = 0, binding = GX_VK_BIND_RAY_INDICES, scalar) readonly buffer IndicesBuffer { uint indices[]; };
layout(set = 0, binding = GX_VK_BIND_RAY_MATERIALS, scalar) readonly buffer MaterialBuffer { Material materials[]; };
layout(set = 0, binding = GX_VK_BIND_RAY_MESH, scalar) readonly buffer MeshBuffer { Mesh meshes[]; };
layout(set = 0, binding = GX_VK_BIND_RAY_TLAS) uniform accelerationStructureEXT tlas;
layout(set = 0, binding = GX_VK_BIND_RAY_2D_TEXTURES) uniform sampler2D textures[];
layout(set = 0, binding = GX_VK_BIND_RAY_CUBE_TEXTURES) uniform samplerCube cube_textures[];
layout(set = 0, binding = GX_VK_BIND_RAY_OUT_IMAGE, rgba32f) uniform image2D out_image;

// clang-format on

layout(push_constant) uniform Constants
{
  vec4  clearColor;
  vec3  lightPosition;
  float lightIntensity;
  int   lightType;
}
pushC;

// Return the vertex position
vec3 getVertex(uint index)
{
  return vertices[3 * index].position;
}

vec3 getNormal(uint index)
{
  return vertices[3 * index].normal;
}

vec2 getTexCoord(uint index)
{
  return vertices[3 * index].uv;
}


void main()
{
  // Retrieve the Primitive mesh buffer information
  Mesh pinfo = meshes[gl_InstanceCustomIndexEXT];

  // Getting the 'first index' for this mesh (offset of the mesh + offset of the triangle)
  uint indexOffset  = pinfo.index_index + (3 * gl_PrimitiveID);
  uint vertexOffset = pinfo.vertex_index;           // Vertex offset as defined in glTF
  uint matIndex     = max(0, pinfo.material_index);  // material of primitive mesh

  // Getting the 3 indices of the triangle (local)
  ivec3 triangleIndex = ivec3(indices[nonuniformEXT(indexOffset + 0)],  //
                              indices[nonuniformEXT(indexOffset + 1)],  //
                              indices[nonuniformEXT(indexOffset + 2)]);
  triangleIndex += ivec3(vertexOffset);  // (global)

  const vec3 barycentrics = vec3(1.0 - attribs.x - attribs.y, attribs.x, attribs.y);

  // Vertex of the triangle
  const vec3 pos0           = getVertex(triangleIndex.x);
  const vec3 pos1           = getVertex(triangleIndex.y);
  const vec3 pos2           = getVertex(triangleIndex.z);
  const vec3 position       = pos0 * barycentrics.x + pos1 * barycentrics.y + pos2 * barycentrics.z;
  const vec3 world_position = vec3(gl_ObjectToWorldEXT * vec4(position, 1.0));

  // Normal
  const vec3 nrm0 = getNormal(triangleIndex.x);
  const vec3 nrm1 = getNormal(triangleIndex.y);
  const vec3 nrm2 = getNormal(triangleIndex.z);
  vec3 normal = normalize(nrm0 * barycentrics.x + nrm1 * barycentrics.y + nrm2 * barycentrics.z);
  const vec3 world_normal = normalize(vec3(normal * gl_WorldToObjectEXT));
  const vec3 geom_normal  = normalize(cross(pos1 - pos0, pos2 - pos0));

  // TexCoord
  const vec2 uv0       = getTexCoord(triangleIndex.x);
  const vec2 uv1       = getTexCoord(triangleIndex.y);
  const vec2 uv2       = getTexCoord(triangleIndex.z);
  const vec2 texcoord0 = uv0 * barycentrics.x + uv1 * barycentrics.y + uv2 * barycentrics.z;

  // https://en.wikipedia.org/wiki/Path_tracing
  // Material of the object
  Material mat       = materials[nonuniformEXT(matIndex)];
  vec3              emittance = mat.emissive_factor;

  // Pick a random direction from here and keep going.
  vec3 tangent, bitangent;
  createCoordinateSystem(world_normal, tangent, bitangent);
  vec3 rayOrigin    = world_position;
  vec3 rayDirection = samplingHemisphere(prd.seed, tangent, bitangent, world_normal);

  // Probability of the newRay (cosine distributed)
  const float p = 1 / M_PI;

  // Compute the BRDF for this ray (assuming Lambertian reflection)
  float cos_theta = dot(rayDirection, world_normal);
  vec3  albedo    = mat.albedo_factor.xyz;
  if(mat.albedo_texture_index > -1)
  {
    uint txtId = mat.albedo_texture_index;
    albedo *= texture(textures[nonuniformEXT(txtId)], texcoord0).xyz;
  }
  vec3 BRDF = albedo / M_PI;

  prd.rayOrigin    = rayOrigin;
  prd.rayDirection = rayDirection;
  prd.hitValue     = emittance;
  prd.weight       = BRDF * cos_theta / p;
  return;

  // Recursively trace reflected light sources.
  if(prd.depth < 10)
  {
    prd.depth++;
    float tMin  = 0.001;
    float tMax  = 100000000.0;
    uint  flags = gl_RayFlagsOpaqueEXT;
    traceRayEXT(tlas,    // acceleration structure
                flags,         // rayFlags
                0xFF,          // cullMask
                0,             // sbtRecordOffset
                0,             // sbtRecordStride
                0,             // missIndex
                rayOrigin,     // ray origin
                tMin,          // ray min range
                rayDirection,  // ray direction
                tMax,          // ray max range
                0              // payload (location = 0)
    );
  }
  vec3 incoming = prd.hitValue;

  // Apply the Rendering Equation here.
  prd.hitValue = emittance + (BRDF * incoming * cos_theta / p);
}
