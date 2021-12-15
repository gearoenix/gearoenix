#pragma once

#ifdef __cplusplus
#include "../../render/gx-rnd-build-configuration.hpp"
#endif

#if defined(GX_RENDER_DXR_ENABLED) || !defined(__cplusplus)

#ifdef __cplusplus
#include "../../math/gx-math-vector-4d.hpp"
#endif

#define GX_DXR_TEXTURE_2DS_COUNT 200
#define GX_DXR_TEXTURE_CUBES_COUNT 50
#define GX_DXR_SAMPLERS_COUNT 3

#ifdef __cplusplus
namespace gearoenix::dxr {
typedef math::Vec4<float> float4;
#endif

struct MeshUniform {
    float4 colour_factor;
    float4 emission_factor__alpha_cutoff;
    float4 normal_scale__occlusion_strength;
    float4 metallic_factor__roughness_factor__radiance_lod_coefficient;
};

#ifdef __cplusplus
}
#endif

#endif
