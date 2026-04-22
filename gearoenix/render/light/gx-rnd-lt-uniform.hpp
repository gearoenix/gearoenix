#ifndef GEAROENIX_RENDER_LIGHT_UNIFORM_HPP // no pragma, because of some of the shader compilers.
#define GEAROENIX_RENDER_LIGHT_UNIFORM_HPP

#include "../buffer/gx-rnd-buf-uniform-struct.hpp"

GX_UNIFORM_STRUCT_DEF(PointLight, {
    vec4 position;
    vec4 colour;
});

GX_UNIFORM_STRUCT_DEF(DirectionalLight, {
    vec4 direction;
    vec4 colour;
});

GX_UNIFORM_STRUCT_DEF(ShadowCasterDirectionalLight, {
    vec4 direction_bit_shadow_map_texture_index;
    vec4 colour;
    mat4 normalised_vp;
});

#endif