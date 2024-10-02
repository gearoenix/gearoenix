#include "gx-rnd-mdl-model.hpp"
#include "../mesh/gx-rnd-msh-mesh.hpp"
#include "../texture/gx-rnd-txt-texture.hpp"

gearoenix::render::model::Model::Model(
    const std::type_index final_component_type,
    const bool is_transformable,
    std::vector<std::shared_ptr<mesh::Mesh>>&& bound_meshes,
    std::string&& name,
    const core::ecs::entity_id_t entity_id)
    : Component(final_component_type, std::move(name), entity_id)
    , is_transformable(is_transformable)
    , meshes(std::move(bound_meshes))
{
}

gearoenix::render::model::Model::~Model() = default;