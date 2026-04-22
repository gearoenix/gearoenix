#ifndef GEAROENIX_RENDER_REFLECTION_UNIFORM_HPP // no pragma, because of some of the shader compilers.
#define GEAROENIX_RENDER_REFLECTION_UNIFORM_HPP

#include "../buffer/gx-rnd-buf-uniform-struct.hpp"

GX_UNIFORM_STRUCT_DEF(ReflectionProbe, {
    vec4 position_radius;
    uvec4 irradiance_radiance_texture_sampler_index;
    vec4 radiance_lod_coefficient;
});

#endif