#ifndef GEAROENIX_RENDER_MATERIAL_UNIFORM_HPP // no pragma, because of some of the shader compilers.
#define GEAROENIX_RENDER_MATERIAL_UNIFORM_HPP

#include "../buffer/gx-rnd-buf-uniform-struct.hpp"

GX_UNIFORM_STRUCT_DEF(Material, {
    vec4 albedo_factor;
    vec4 normal_metallic_factor;
    vec4 emission_roughness_factor;
    vec4 alpha_cutoff_occlusion_strength_reserved_reserved;
    uvec4 albedo_normal_texture_sampler_index;
    uvec4 orm_emission_texture_sampler_index;
});

#endif