#include "gx-rnd-sky-skybox.hpp"

gearoenix::render::skybox::Skybox::Skybox(
    const std::type_index final_component_type_index,
    std::shared_ptr<mesh::Mesh>&& bound_mesh,
    Texture&& bound_texture,
    std::string&& name,
    const core::ecs::entity_id_t entity_id)
    : Component(final_component_type_index, std::move(name), entity_id)
    , bound_mesh(std::move(bound_mesh))
    , bound_texture(std::move(bound_texture))
{
}

gearoenix::render::skybox::Skybox::~Skybox() = default;