#pragma once
#include "../../render/gx-rnd-build-configuration.hpp"
#ifdef GX_RENDER_METAL_ENABLED
#include <simd/simd.h>

#if GX_DEBUG_MODE
#define GEAROENIX_METAL_RESOURCE_NAMING
#endif

#define GEAROENIX_METAL_BUFFER_UNIFORM_MAX_SIZE 4 * 1024 * 1024

#define GEAROENIX_METAL_FRAMES_COUNT 3

#define GEAROENIX_METAL_GBUFFERS_FILLER_VERTEX_BUFFER_BIND_INDEX 0
#define GEAROENIX_METAL_GBUFFERS_FILLER_CAMERA_UNIFORM_BIND_INDEX 1
#define GEAROENIX_METAL_GBUFFERS_FILLER_ARGUMENT_BUFFER_BIND_INDEX 2

#define GEAROENIX_METAL_GBUFFERS_FILLER_MODEL_UNIFORM_BIND_ID 0
#define GEAROENIX_METAL_GBUFFERS_FILLER_ALBEDO_TEXTURE_BIND_ID 1
#define GEAROENIX_METAL_GBUFFERS_FILLER_NORMAL_METALLIC_TEXTURE_BIND_ID 2
#define GEAROENIX_METAL_GBUFFERS_FILLER_EMISSION_ROUGHNESS_TEXTURE_BIND_ID 3
#define GEAROENIX_METAL_GBUFFERS_FILLER_SAMPLER_BIND_ID 4

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