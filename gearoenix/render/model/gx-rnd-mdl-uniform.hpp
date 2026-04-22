#ifndef GEAROENIX_RENDER_MODEL_UNIFORM_HPP // no pragma, because of some of the shader compilers.
#define GEAROENIX_RENDER_MODEL_UNIFORM_HPP

#include "../buffer/gx-rnd-buf-uniform-struct.hpp"

GX_UNIFORM_STRUCT_DEF(Model, {
    mat4 m;
    mat4 inv_transpose_m;
    uvec4 bones_point_lights_directional_lights_shadow_caster_directional_lights_count;
    uvec4 point_lights[GX_RENDER_MAX_POINT_LIGHTS / 4];
    uvec4 directional_lights[GX_RENDER_MAX_DIRECTIONAL_LIGHTS / 4];
    uvec4 shadow_caster_directional_lights[GX_RENDER_MAX_DIRECTIONAL_LIGHTS_SHADOW_CASTER / 4];
    uvec4 reflection_probe_bones_begin_reserved_reserved;
});

GX_UNIFORM_STRUCT_DEF(Bone, {
    mat4 m;
    mat4 inv_transpose_m;
});

#ifdef __cplusplus

static_assert(GX_RENDER_MAX_POINT_LIGHTS % 4 == 0);
static_assert(GX_RENDER_MAX_DIRECTIONAL_LIGHTS % 4 == 0);
static_assert(GX_RENDER_MAX_DIRECTIONAL_LIGHTS_SHADOW_CASTER % 4 == 0);

#endif

#endif