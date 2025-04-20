#include "gx-gl-scene.hpp"
#ifdef GX_RENDER_OPENGL_ENABLED
#include "../core/ecs/gx-cr-ecs-comp-type.hpp"

gearoenix::gl::Scene::Scene(std::string&& name, const double layer)
    : render::scene::Scene(core::ecs::ComponentType::create_index(this), layer, std::move(name))
{
}

gearoenix::gl::Scene::~Scene()
{
}

void gearoenix::gl::Scene::update()
{
    render::scene::Scene::update();
}

gearoenix::gl::SceneManager::SceneManager()
{
    core::ecs::ComponentType::add<Scene>();
}

gearoenix::gl::SceneManager::~SceneManager() = default;

gearoenix::core::ecs::EntityPtr gearoenix::gl::SceneManager::build(std::string&& name, double layer) const
{
    auto entity = Manager::build(std::move(name), layer);
    entity->add_component(core::Object::construct<Scene>(entity->get_object_name() + "-scene", layer));
    return entity;
}

#endif
