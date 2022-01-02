#include "gx-rnd-scn-scene.hpp"
#include "../../core/ecs/gx-cr-ecs-world.hpp"
#include "../../core/macro/gx-cr-mcr-assert.hpp"
#include "../camera/gx-rnd-cmr-camera.hpp"
#include "../engine/gx-rnd-eng-engine.hpp"
#include <algorithm>

gearoenix::render::scene::Scene::Scene(engine::Engine& e, const double layer) noexcept
    : core::ecs::Component(this)
    , e(e)
    , layer(layer)
{
}

gearoenix::render::scene::Scene::~Scene() noexcept = default;

gearoenix::render::scene::Scene::Scene(Scene&&) noexcept = default;

void gearoenix::render::scene::Scene::add_model(const core::ecs::Entity::id_t entity, model::Model&) noexcept
{
    model_entities.insert(entity);
    entities.insert(entity);
}

void gearoenix::render::scene::Scene::add_camera(const core::ecs::Entity::id_t entity, camera::Camera&) noexcept
{
    camera_entities.insert(entity);
    entities.insert(entity);
}

void gearoenix::render::scene::Scene::update() noexcept
{
}