#version 460
#extension GL_EXT_ray_tracing : require
#extension GL_GOOGLE_include_directive : enable
#include "gx-vk-shd-common.glslh"

layout(location = 0) rayPayloadInEXT hitPayload prd;

void main()
{
  if(prd.depth == 0)
    prd.hitValue = vec3(0.0);
  else
    prd.hitValue = vec3(0.01);  // No contribution from environment
  prd.depth = 100;              // Ending trace
}
