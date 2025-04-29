#include "gx-gl-model.hpp"
#ifdef GX_RENDER_OPENGL_ENABLED
#include "../core/allocator/gx-cr-alc-shared-array.hpp"
#include "../core/ecs/gx-cr-ecs-comp-type.hpp"
#include "../core/ecs/gx-cr-ecs-entity.hpp"
#include "gx-gl-engine.hpp"
#include "gx-gl-mesh.hpp"
#include "material/gx-gl-material.hpp"

gearoenix::gl::Model::Model(render::model::meshes_set_t&& ms, std::string&& name, const bool is_transformable)
    : render::model::Model(core::ecs::ComponentType::create_index(this), is_transformable, std::move(ms), std::move(name))
{
    for (const auto& mesh : meshes) {
        auto m = std::dynamic_pointer_cast<Mesh>(mesh);
        GX_ASSERT_D(m);
        gl_meshes.insert(std::move(m));
    }
}

gearoenix::gl::Model::~Model() = default;

void gearoenix::gl::Model::render_shadow(
        const render::record::Camera& camera,
        const render::record::CameraModel& camera_model,
        uint& current_shader)
{
    for (const auto& msh : gl_meshes) {
        msh->get_gl_material()->shadow(*msh, camera, camera_model, current_shader);
    }
}

void gearoenix::gl::Model::render_forward(const Scene& scene, const render::record::Camera& camera,
    const render::record::CameraModel& camera_model, uint& current_shader)
{
    for (const auto& msh : gl_meshes) {
        msh->get_gl_material()->render_forward(scene, camera, camera_model, *this, *msh, current_shader);
    }
}

gearoenix::core::ecs::EntityPtr gearoenix::gl::ModelManager::build(
    std::string&& name,
    core::ecs::Entity* const parent,
    render::model::meshes_set_t&& meshes,
    const bool is_transformable)
{
    auto entity = Manager::build(std::move(name), parent, std::move(meshes), is_transformable);
    entity->add_component(core::Object::construct<Model>(std::move(meshes), std::move(name), is_transformable));
    return entity;
}

gearoenix::gl::ModelManager::ModelManager()
{
    core::ecs::ComponentType::add<Model>();
}

gearoenix::gl::ModelManager::~ModelManager() = default;

#endif
