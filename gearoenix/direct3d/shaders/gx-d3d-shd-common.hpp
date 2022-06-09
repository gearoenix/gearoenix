#pragma once

#ifdef __cplusplus
#include "../../render/gx-rnd-build-configuration.hpp"
#endif

#if defined(GX_RENDER_DIRECT3D_ENABLED) || !defined(__cplusplus)

#ifdef __cplusplus
#include "../../math/gx-math-matrix-4d.hpp"
#endif

#define GX_D3D_TEXTURE_2DS_COUNT 200
#define GX_D3D_TEXTURE_CUBES_COUNT 50
#define GX_D3D_SAMPLERS_COUNT 3

#ifdef __cplusplus
namespace gearoenix::d3d {
typedef math::Vec4<float> float4;
typedef math::Vec4<std::uint32_t> uint4;
typedef math::Mat4x4<float> float4x4;
#endif

struct ModelUniform {
    float4x4 model;
    float4x4 transposed_reversed_model;
    float4 colour_factor;
    float4 emission_factor__alpha_cutoff;
    float4 normal_scale__occlusion_strength;
    float4 metallic_factor__roughness_factor__radiance_lod_coefficient;
    uint4 sampler_albedo_normal_emission;
};

struct CameraUniform {
    float4x4 view_projection;
};

#ifdef __cplusplus
}
#endif

#endif
