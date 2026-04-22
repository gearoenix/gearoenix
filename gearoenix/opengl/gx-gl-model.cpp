#include "gx-gl-model.hpp"
#if GX_RENDER_OPENGL_ENABLED
#include "../core/ecs/gx-cr-ecs-comp-type.hpp"
#include "../core/ecs/gx-cr-ecs-entity.hpp"
#include "../physics/animation/gx-phs-anm-armature.hpp"
#include "../physics/gx-phs-transformation.hpp"
#include "gx-gl-engine.hpp"
#include "gx-gl-mesh.hpp"
#include "material/gx-gl-material.hpp"

gearoenix::gl::Model::Model(
    core::ecs::Entity* const entity,
    render::model::meshes_set_t&& ms,
    std::string&& name,
    const bool is_transformable,
    std::shared_ptr<physics::Transformation>&& transformation,
    std::shared_ptr<physics::animation::Armature>&& armature)
    : render::model::Model(
          entity, core::ecs::ComponentType::create_index(this), is_transformable,
          std::move(transformation), std::move(ms), std::move(name),
          std::move(armature))
{
    for (const auto& mesh : meshes) {
        auto m = std::dynamic_pointer_cast<Mesh>(mesh);
        GX_ASSERT_D(m);
        gl_meshes.insert(std::move(m));
    }
}

gearoenix::gl::Model::~Model() = default;

void gearoenix::gl::Model::render_shadow(const render::record::Camera& camera, const render::record::CameraModel& camera_model, uint& current_shader)
{
    for (const auto& msh : gl_meshes) {
        msh->get_gl_material()->shadow(*msh, camera, camera_model, current_shader);
    }
}

void gearoenix::gl::Model::render_forward(const Scene& scene, const render::record::Camera& camera, const render::record::CameraModel& camera_model, uint& current_shader)
{
    for (const auto& msh : gl_meshes) {
        msh->get_gl_material()->render_forward(scene, camera, camera_model, *msh, current_shader);
    }
}

gearoenix::core::ecs::EntityPtr gearoenix::gl::ModelManager::build(
    std::string&& name,
    core::ecs::Entity* const parent,
    render::model::meshes_set_t&& meshes,
    const bool is_transformable,
    std::shared_ptr<physics::animation::Armature>&& armature)
{
    auto entity = Manager::build(std::string(name), parent, render::model::meshes_set_t(meshes), is_transformable, std::shared_ptr(armature));
    auto transform = entity->get_component_shared_ptr<physics::Transformation>();
    entity->add_component(core::Object::construct<Model>(entity.get(), std::move(meshes), std::move(name), is_transformable, std::move(transform), std::move(armature)));
    return entity;
}

gearoenix::gl::ModelManager::ModelManager()
{
    core::ecs::ComponentType::add<Model>();
}

gearoenix::gl::ModelManager::~ModelManager() = default;

#endif
