#include "gx-gl-model.hpp"
#ifdef GX_RENDER_OPENGL_ENABLED
#include "../core/ecs/gx-cr-ecs-entity.hpp"
#include "../render/material/gx-rnd-mat-material.hpp"
#include "../render/model/gx-rnd-mdl-model.hpp"
#include "gx-gl-engine.hpp"
#include "gx-gl-mesh.hpp"
#include "material/gx-gl-material.hpp"

gearoenix::gl::Model::Model(
    Engine&,
    std::shared_ptr<Mesh>&& mesh,
    std::shared_ptr<material::Material>&& mat,
    const std::string&) noexcept
    : core::ecs::Component(this)
    , mesh(std::move(mesh))
    , mat(std::move(mat))
{
}

gearoenix::gl::Model::~Model() noexcept = default;

gearoenix::gl::Model::Model(Model&& o) noexcept = default;

gearoenix::gl::ModelBuilder::ModelBuilder(
    Engine& e,
    const std::string& name,
    std::shared_ptr<render::mesh::Mesh>&& mesh,
    std::shared_ptr<render::material::Material>&& mat,
    core::sync::EndCaller&& end_caller,
    bool is_transformable) noexcept
    : render::model::Builder(
        e, name,
        std::shared_ptr<render::mesh::Mesh>(mesh),
        std::shared_ptr<render::material::Material>(mat),
        std::move(end_caller),
        is_transformable)
    , e(e)
{
    entity_builder->get_builder().add_component(Model(
        e,
        std::dynamic_pointer_cast<Mesh>(mesh),
        std::dynamic_pointer_cast<material::Material>(mat),
        name));
}

gearoenix::gl::ModelBuilder::~ModelBuilder() noexcept = default;

std::shared_ptr<gearoenix::render::model::Builder> gearoenix::gl::ModelManager::build(
    std::string&& name,
    std::shared_ptr<render::mesh::Mesh>&& mesh,
    std::shared_ptr<render::material::Material>&& material,
    core::sync::EndCaller&& end_caller,
    const bool is_transformable) noexcept
{
    return std::shared_ptr<render::model::Builder>(new ModelBuilder(
        dynamic_cast<Engine&>(e), name, std::move(mesh), std::move(material), std::move(end_caller), is_transformable));
}

gearoenix::gl::ModelManager::ModelManager(Engine& e) noexcept
    : render::model::Manager(e)
{
    core::ecs::Component::register_type<Model>();
}

gearoenix::gl::ModelManager::~ModelManager() noexcept = default;

#endif
