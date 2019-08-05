#include "rnd-pip-forward-pbr-resource-set.hpp"
#include "../../core/cr-static.hpp"

void gearoenix::render::pipeline::ForwardPbrResourceSet::set_node_uniform_buffer(buffer::Uniform* const uniform_buffer) noexcept
{
    node_uniform_buffer = uniform_buffer;
}

void gearoenix::render::pipeline::ForwardPbrResourceSet::set_diffuse_environment(texture::Cube* const t) noexcept
{
    diffuse_environment = t;
}

void gearoenix::render::pipeline::ForwardPbrResourceSet::set_specular_environment(texture::Cube* const t) noexcept
{
    specular_environment = t;
}

void gearoenix::render::pipeline::ForwardPbrResourceSet::set_ambient_occlusion(texture::Texture2D* const t) noexcept
{
    ambient_occlusion = t;
}

void gearoenix::render::pipeline::ForwardPbrResourceSet::set_directional_lights_shadow_maps(texture::Texture2D* const t, const int light_index, const int cas_index) noexcept
{
    directional_lights_shadow_maps[light_index][cas_index] = t;
}

void gearoenix::render::pipeline::ForwardPbrResourceSet::set_brdflut(texture::Texture2D* const t) noexcept
{
    brdflut = t;
}

void gearoenix::render::pipeline::ForwardPbrResourceSet::clean() noexcept
{
    ResourceSet::clean();
    diffuse_environment = nullptr;
    specular_environment = nullptr;
    ambient_occlusion = nullptr;
    GX_SET_ARRAY_ZERO(directional_lights_shadow_maps)
    brdflut = nullptr;
}
