#include "gx-gl-model.hpp"
#ifdef GX_RENDER_OPENGL_ENABLED
#include "../core/ecs/gx-cr-ecs-entity.hpp"
#include "../render/model/gx-rnd-mdl-model.hpp"
#include "gx-gl-constants.hpp"
#include "gx-gl-engine.hpp"
#include "gx-gl-mesh.hpp"
#include "gx-gl-texture.hpp"

gearoenix::gl::Model::Model(
    Engine&,
    std::shared_ptr<Mesh>&& bound_mesh,
    render::material::Pbr&& _material,
    const std::string&) noexcept
    : core::ecs::Component(this)
    , bound_mesh(std::move(bound_mesh))
    , material(std::move(_material))
    , albedo(std::dynamic_pointer_cast<Texture2D>(material.get_albedo()))
    , normal(std::dynamic_pointer_cast<Texture2D>(material.get_normal()))
    , emission(std::dynamic_pointer_cast<Texture2D>(material.get_emission()))
    , metallic_roughness(std::dynamic_pointer_cast<Texture2D>(material.get_metallic_roughness()))
    , occlusion(std::dynamic_pointer_cast<Texture2D>(material.get_occlusion()))
{
}

gearoenix::gl::Model::~Model() noexcept = default;

gearoenix::gl::Model::Model(Model&& o) noexcept = default;

gearoenix::gl::ModelBuilder::ModelBuilder(
    Engine& e,
    const std::string& name,
    std::shared_ptr<render::mesh::Mesh>&& bound_mesh,
    bool is_transformable) noexcept
    : render::model::Builder(e, name, std::move(bound_mesh), is_transformable)
    , e(e)
{
}

void gearoenix::gl::ModelBuilder::set_material(const render::material::Pbr& mat) noexcept
{
    render::model::Builder::set_material(mat);
    auto& b = entity_builder->get_builder();
    Model m(
        e,
        std::dynamic_pointer_cast<Mesh>(b.get_component<render::model::Model>()->bound_mesh),
        render::material::Pbr(mat),
        *b.get_name());
    b.add_component(std::move(m));
}

gearoenix::gl::ModelBuilder::~ModelBuilder() noexcept = default;

std::shared_ptr<gearoenix::render::model::Builder> gearoenix::gl::ModelManager::build(
    std::string&& name,
    std::shared_ptr<render::mesh::Mesh>&& mesh,
    core::sync::EndCallerIgnored&&,
    const bool is_transformable) noexcept
{
    return std::shared_ptr<render::model::Builder>(new ModelBuilder(
        dynamic_cast<Engine&>(e), name, std::move(mesh), is_transformable));
}

gearoenix::gl::ModelManager::ModelManager(Engine& e) noexcept
    : render::model::Manager(e)
{
}

gearoenix::gl::ModelManager::~ModelManager() noexcept = default;

#endif
