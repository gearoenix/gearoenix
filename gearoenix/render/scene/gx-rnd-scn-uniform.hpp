#ifndef GEAROENIX_RENDER_SCENE_UNIFORM_HPP // no pragma, because of some of the shader compilers.
#define GEAROENIX_RENDER_SCENE_UNIFORM_HPP

#include "../buffer/gx-rnd-buf-uniform-struct.hpp"

GX_UNIFORM_STRUCT_DEF(Scene, {
    vec4 ambient_light_layer;
    vec4 ssao_settings;
    uvec4 brdflut_sheen_lut_texture_sampler_index;
    uvec4 clearcoat_lut_sss_lut_texture_sampler_index;
});

#endif