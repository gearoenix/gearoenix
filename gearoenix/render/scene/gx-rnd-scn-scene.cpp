#include "gx-rnd-scn-scene.hpp"
#include "../../core/ecs/gx-cr-ecs-world.hpp"
#include "../../physics/gx-phs-transformation.hpp"
#include "../camera/gx-rnd-cmr-camera.hpp"
#include "../engine/gx-rnd-eng-engine.hpp"
#include "../model/gx-rnd-mdl-model.hpp"

gearoenix::render::scene::Scene::Scene(engine::Engine& e, const double layer) noexcept
    : core::ecs::Component(this)
    , e(e)
    , ssao_settings(0.08f, 0.001f, 0.000f, 0.003f)
    , layer(layer)
{
    enabled = false;
}

gearoenix::render::scene::Scene::~Scene() noexcept = default;

gearoenix::render::scene::Scene::Scene(Scene&&) noexcept = default;

void gearoenix::render::scene::Scene::add_model(const core::ecs::entity_id_t entity, model::Model&) noexcept
{
    model_entities.insert(entity);
    entities.insert(entity);
}

void gearoenix::render::scene::Scene::add_camera(const core::ecs::entity_id_t entity, camera::Camera&) noexcept
{
    camera_entities.insert(entity);
    entities.insert(entity);
}

void gearoenix::render::scene::Scene::add_baked_reflection(const core::ecs::entity_id_t entity, reflection::Baked&) noexcept
{
    baked_reflection_entities.insert(entity);
    entities.insert(entity);
}

void gearoenix::render::scene::Scene::add_runtime_reflection(const core::ecs::entity_id_t entity, reflection::Runtime&) noexcept
{
    runtime_reflection_entities.insert(entity);
    entities.insert(entity);
}

void gearoenix::render::scene::Scene::add_skybox(const core::ecs::entity_id_t entity, skybox::Skybox&) noexcept
{
    skybox_entities.insert(entity);
    entities.insert(entity);
}

void gearoenix::render::scene::Scene::add_light(const core::ecs::entity_id_t entity, light::Light&) noexcept
{
    light_entities.insert(entity);
    entities.insert(entity);
}

void gearoenix::render::scene::Scene::add_empty(const std::string& name, const math::Vec3<double>& location) noexcept
{
    empties.emplace(name, location);
}

void gearoenix::render::scene::Scene::update(const core::ecs::entity_id_t scene_entity_id) noexcept
{
    auto* const world = e.get_world();
    std::uint64_t flag = 1;
    cameras_flags.clear();
    world->synchronised_system<camera::Camera>([&](const core::ecs::entity_id_t id, camera::Camera* const cam) {
        if (!cam->enabled || cam->get_scene_id() != scene_entity_id)
            return;
        cameras_flags[id] = flag;
        cam->set_flag(flag);
        flag <<= 1;
    });
    std::atomic<bool> refresh_bvh = false;
    world->parallel_system<core::ecs::And<model::Model, physics::Transformation>>(
        [&](const core::ecs::entity_id_t, model::Model* const mdl, physics::Transformation* const trn, const auto /*kernel_index*/) {
            if (!mdl->enabled || mdl->scene_id != scene_entity_id)
                return;
            if (mdl->cameras.has_value()) {
                mdl->cameras_flags = 0;
                const auto& cameras = mdl->cameras.value();
                for (const auto camera_id : cameras) {
                    if (auto camera_search = cameras_flags.find(camera_id); cameras_flags.end() != camera_search) {
                        mdl->cameras_flags |= camera_search->second;
                    }
                }
            } else {
                mdl->cameras_flags = static_cast<std::uint64_t>(-1);
            }
            if (!mdl->get_is_transformable() && trn->get_changed()) {
                refresh_bvh = true;
            }
        });
    if (refresh_bvh) {
        GX_LOG_D("Warning, recreation of BVH, do not transform static models,"
                 "if you are seeing this log only once for each scene it is ok,"
                 "otherwise you're going to have performance problem.");
        recreate_bvh = true;
    } else {
        recreate_bvh = false;
    }
}
