#ifndef GEAROENIX_METAL_SHADER_COMMON_HPP
#define GEAROENIX_METAL_SHADER_COMMON_HPP
#include "../../render/gx-rnd-build-configuration.hpp"
#ifdef GX_RENDER_METAL_ENABLED
#include <simd/simd.h>

#define GEAROENIX_METAL_FRAMES_COUNT 3

#define GEAROENIX_METAL_GBUFFERS_FILLER_VERTEX_BUFFER_BIND_INDEX 0
#define GEAROENIX_METAL_GBUFFERS_FILLER_MODEL_UNIFORM_BIND_INDEX 1
#define GEAROENIX_METAL_GBUFFERS_FILLER_CAMERA_UNIFORM_BIND_INDEX 2

namespace gearoenix::metal {

struct ModelUniform {
    matrix_float4x4 model;
    matrix_float4x4 transposed_reversed_model;
    vector_float4 colour_factor;
    vector_float4 emission_factor__alpha_cutoff;
    vector_float4 normal_scale__occlusion_strength;
    vector_float4 metallic_factor__roughness_factor__radiance_lod_coefficient;
    vector_uint4 sampler_albedo_normal_emission;
};

struct CameraUniform {
    matrix_float4x4 view_projection;
    vector_float4 position;
};

}

#endif
#endif
