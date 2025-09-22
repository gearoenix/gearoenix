#include "gx-rnd-sky-skybox.hpp"
#include "../texture/gx-rnd-txt-texture.hpp"

gearoenix::render::skybox::Skybox::Skybox(
    core::ecs::Entity* const entity,
    const core::object_type_index_t final_component_type_index,
    std::shared_ptr<mesh::Mesh>&& bound_mesh,
    std::shared_ptr<texture::Texture>&& bound_texture,
    std::string&& name)
    : Component(entity, final_component_type_index, std::move(name))
    , bound_mesh(std::move(bound_mesh))
    , bound_texture(std::move(bound_texture))
{
}

gearoenix::render::skybox::Skybox::~Skybox() = default;

bool gearoenix::render::skybox::Skybox::is_equirectangular() const
{
    return texture::Type::Texture2D == bound_texture->get_info().get_type();
}

bool gearoenix::render::skybox::Skybox::is_cube() const
{
    return texture::Type::TextureCube == bound_texture->get_info().get_type();
}