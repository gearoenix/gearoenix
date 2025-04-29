#include "gx-rnd-mdl-model.hpp"
#include "../material/gx-rnd-mat-material.hpp"
#include "../mesh/gx-rnd-msh-mesh.hpp"

gearoenix::render::model::Model::Model(
    const core::object_type_index_t final_component_type, const bool is_transformable, meshes_set_t&& meshes, std::string&& name)
    : Component(final_component_type, std::move(name))
    , is_transformable(is_transformable)
    , meshes(std::move(meshes))
{
}

gearoenix::render::model::Model::~Model() = default;

bool gearoenix::render::model::Model::has_transparent_material() const
{
    for (const auto& msh : meshes) {
        if (msh->get_bound_material()->get_transparency() == material::Transparency::Transparent) {
            return true;
        }
    }
    return false;
}

bool gearoenix::render::model::Model::needs_mvp() const
{
    for (const auto& msh : meshes) {
        if (msh->get_bound_material()->get_need_model_view_projection_matrix()) {
            return true;
        }
    }
    return false;
}