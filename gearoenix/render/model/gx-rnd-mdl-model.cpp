#include "gx-rnd-mdl-model.hpp"
#include "../mesh/gx-rnd-msh-mesh.hpp"
#include "../texture/gx-rnd-txt-texture.hpp"

gearoenix::render::model::Model::Model(
    const std::type_index final_component_type,
    const bool is_transformable,
    std::shared_ptr<mesh::Mesh>&& bound_mesh,
    std::shared_ptr<material::Material>&& bound_material,
    std::string&& name)
    : Component(final_component_type, std::move(name))
    , is_transformable(is_transformable)
    , bound_mesh(std::move(bound_mesh))
    , bound_material(std::move(bound_material))
{
}

gearoenix::render::model::Model::~Model() = default;