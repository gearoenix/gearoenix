#include "gx-rnd-mdl-model.hpp"
#include "../mesh/gx-rnd-msh-mesh.hpp"
#include "../texture/gx-rnd-txt-texture.hpp"

gearoenix::render::model::Model::Model(
    std::shared_ptr<mesh::Mesh>&& bound_mesh,
    const std::type_index material_type_index,
    const bool is_transformable,
    const TranslucencyMode translucency,
    const bool is_enabled_rendering,
    const bool is_shadow_caster,
    const bool is_shadow_receiver) noexcept
    : core::ecs::Component(this)
    , material_type_index(material_type_index)
    , translucency(translucency)
    , is_enabled_rendering(is_enabled_rendering)
    , is_shadow_caster(is_shadow_caster)
    , is_shadow_receiver(is_shadow_receiver)
    , is_transformable(is_transformable)
    , bound_mesh(std::move(bound_mesh))
{
}

gearoenix::render::model::Model::~Model() noexcept = default;

gearoenix::render::model::Model::Model(Model&&) noexcept = default;
