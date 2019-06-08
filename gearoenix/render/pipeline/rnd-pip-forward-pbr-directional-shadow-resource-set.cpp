#include "rnd-pip-forward-pbr-directional-shadow-resource-set.hpp"

void gearoenix::render::pipeline::ForwardPbrDirectionalShadowResourceSet::set_node_uniform_buffer(buffer::Uniform* const uniform_buffer) noexcept
{
    node_uniform_buffer = uniform_buffer;
}

void gearoenix::render::pipeline::ForwardPbrDirectionalShadowResourceSet::set_diffuse_environment(texture::Cube* const t) noexcept
{
    diffuse_environment = t;
}

void gearoenix::render::pipeline::ForwardPbrDirectionalShadowResourceSet::set_specular_environment(texture::Cube* const t) noexcept
{
    specular_environment = t;
}

void gearoenix::render::pipeline::ForwardPbrDirectionalShadowResourceSet::set_ambient_occlusion(texture::Texture2D* const t) noexcept
{
    ambient_occlusion = t;
}

void gearoenix::render::pipeline::ForwardPbrDirectionalShadowResourceSet::set_shadow_mapper(texture::Texture2D* const t) noexcept
{
    shadow_map = t;
}

void gearoenix::render::pipeline::ForwardPbrDirectionalShadowResourceSet::set_brdflut(texture::Texture2D* const t) noexcept
{
    brdflut = t;
}

void gearoenix::render::pipeline::ForwardPbrDirectionalShadowResourceSet::clean() noexcept
{
    ResourceSet::clean();
    diffuse_environment = nullptr;
    specular_environment = nullptr;
    ambient_occlusion = nullptr;
    shadow_map = nullptr;
    brdflut = nullptr;
}
