#define GX_PLATFORM_LOG_STD_OUT_ENABLED

#include <gearoenix/core/allocator/gx-cr-alc-shared-array.hpp>
#include <gearoenix/core/ecs/gx-cr-ecs-entity-ptr.hpp>
#include <gearoenix/core/ecs/gx-cr-ecs-world.hpp>
#include <gearoenix/core/gx-cr-application.hpp>
#include <gearoenix/physics/constraint/gx-phs-cns-manager.hpp>
#include <gearoenix/physics/gx-phs-engine.hpp>
#include <gearoenix/physics/gx-phs-transformation.hpp>
#include <gearoenix/render/camera/gx-rnd-cmr-manager.hpp>
#include <gearoenix/render/engine/gx-rnd-eng-engine.hpp>
#include <gearoenix/render/gx-rnd-vertex.hpp>
#include <gearoenix/render/light/gx-rnd-lt-directional.hpp>
#include <gearoenix/render/light/gx-rnd-lt-light.hpp>
#include <gearoenix/render/light/gx-rnd-lt-manager.hpp>
#include <gearoenix/render/material/gx-rnd-mat-manager.hpp>
#include <gearoenix/render/material/gx-rnd-mat-pbr.hpp>
#include <gearoenix/render/mesh/gx-rnd-msh-manager.hpp>
#include <gearoenix/render/model/gx-rnd-mdl-manager.hpp>
#include <gearoenix/render/scene/gx-rnd-scn-manager.hpp>
#include <gearoenix/render/scene/gx-rnd-scn-scene.hpp>
#include <gearoenix/render/texture/gx-rnd-txt-manager.hpp>
#include <random>

namespace {
constexpr double position_limit = 2.0;
constexpr double cube_size = 0.05;
constexpr double max_speed = cube_size * 2.5;
constexpr std::uint32_t objects_count = 8000;

std::random_device random_device {};
std::default_random_engine random_engine { random_device() };
std::uniform_real_distribution space_distribution(-position_limit, position_limit);
std::uniform_real_distribution speed_distribution(-max_speed, max_speed);
std::uniform_real_distribution colour_distribution(0.5f, 1.0f);

template <typename... Ts>
using GxAll = gearoenix::core::ecs::All<Ts...>;
template <typename T>
using GxEndCallerShared = gearoenix::core::job::EndCallerShared<T>;
template <typename T>
using GxEndCaller = gearoenix::core::job::EndCaller<T>;

typedef gearoenix::core::Application GxCoreApp;
typedef gearoenix::core::ecs::Entity GxEntity;
typedef gearoenix::core::ecs::EntityPtr GxEntityPtr;
typedef gearoenix::core::ecs::Component GxComp;
typedef gearoenix::core::ecs::World GxWorld;
typedef gearoenix::physics::constraint::Manager GxConstraintManager;
typedef gearoenix::physics::Transformation GxTransform;
typedef gearoenix::render::camera::Manager GxCameraManager;
typedef gearoenix::render::engine::Engine GxRenderEngine;
typedef gearoenix::render::light::Manager GxLightManager;
typedef gearoenix::render::light::ShadowCasterDirectional GxShadowCaster;
typedef gearoenix::render::material::Manager GxMatManager;
typedef gearoenix::render::material::Pbr GxPbr;
typedef gearoenix::render::mesh::Manager GxMeshManager;
typedef gearoenix::render::mesh::Mesh GxMesh;
typedef gearoenix::render::model::Manager GxModelManager;
typedef gearoenix::render::scene::Manager GxSceneManager;
typedef GxEndCaller<GxEntityPtr> GxEntityEndCaller;
typedef GxEndCallerShared<GxMesh> GxMeshEndCaller;
typedef GxEndCallerShared<GxPbr> GxPbrEndCaller;
typedef std::shared_ptr<GxMesh> GxMeshPtr;
typedef std::shared_ptr<GxPbr> GxPbrPtr;

struct Position;

struct Speed final : GxComp {
    constexpr static auto max_count = objects_count;
    constexpr static auto object_type_index = gearoenix_last_component_type_index + 1;

    gearoenix::math::Vec3<double> value;

    explicit Speed(GxEntity* e);
    void update(const Position& p);
};

struct Position final : GxComp {
    constexpr static auto max_count = objects_count;
    constexpr static auto object_type_index = gearoenix_last_component_type_index + 2;

    gearoenix::math::Vec3<double> value;

    explicit Position(GxEntity* e);
    void update(double delta_time, const Speed& speed);
};

Speed::Speed(GxEntity* const e)
    : GxComp(e, gearoenix::core::ecs::ComponentType::create_index(this), "speed")
    , value(
          speed_distribution(random_engine),
          speed_distribution(random_engine),
          speed_distribution(random_engine))
{
    value.x += value.x > 0 ? max_speed : -max_speed;
    value.y += value.y > 0 ? max_speed : -max_speed;
    value.z += value.z > 0 ? max_speed : -max_speed;
}

void Speed::update(const Position& p)
{
    const auto pos = p.value.abs() + 0.1f;
    if (pos.greater(position_limit).or_elements()) {
        gearoenix::math::Vec3<double> n;
        if (pos.x > position_limit) {
            if (p.value.x < 0.0) {
                n = gearoenix::math::Vec3(1.0, 0.0, 0.0);
            } else {
                n = gearoenix::math::Vec3(-1.0, 0.0, 0.0);
            }
            value = n.reflect(value);
        }
        if (pos.y > position_limit) {
            if (p.value.y < 0.0) {
                n = gearoenix::math::Vec3(0.0, 1.0, 0.0);
            } else {
                n = gearoenix::math::Vec3(0.0, -1.0, 0.0);
            }
            value = n.reflect(value);
        }
        if (pos.z > position_limit) {
            if (p.value.z < 0.0) {
                n = gearoenix::math::Vec3(0.0, 0.0, 1.0);
            } else {
                n = gearoenix::math::Vec3(0.0, 0.0, -1.0);
            }
            value = n.reflect(value);
        }
    }
}

Position::Position(GxEntity* const e)
    : GxComp(e, gearoenix::core::ecs::ComponentType::create_index(this), "position")
    , value(
          space_distribution(random_engine),
          space_distribution(random_engine),
          space_distribution(random_engine))
{
}

void Position::update(const double delta_time, const Speed& speed)
{
    value += speed.value * delta_time;
    value.clamp(-position_limit, position_limit);
}
}

struct GameApp final : GxCoreApp {
private:
    GxEntityPtr scene_entity;

public:
    GameApp()
        : scene_entity(GxSceneManager::get().build("scene", 0.0))
    {
        gearoenix::core::ecs::ComponentType::add<Position>();
        gearoenix::core::ecs::ComponentType::add<Speed>();

        const auto materials = std::make_shared<std::array<GxPbrPtr, objects_count>>();

        const GxEndCaller<void> end([this, materials] { materials_ready(*materials); });

        for (std::uint32_t model_index = 0; model_index < objects_count; ++model_index) {
            GxMatManager::get().get_pbr(
                "material-" + std::to_string(model_index),
                GxPbrEndCaller([model_index, materials, end](GxPbrPtr&& m) {
                    m->get_albedo_factor() = {
                        colour_distribution(random_engine),
                        colour_distribution(random_engine),
                        colour_distribution(random_engine),
                        1.0f
                    };
                    (*materials)[model_index] = std::move(m);
                    (void)end; }));
        }
    }

    void materials_ready(std::array<GxPbrPtr, objects_count>& materials)
    {
        const auto meshes = std::make_shared<std::array<GxMeshPtr, objects_count>>();

        const GxEndCaller<void> end([this, meshes] { meshes_ready(*meshes); });

        for (std::uint32_t model_index = 0; model_index < objects_count; ++model_index) {
            GxMeshManager::get().build_cube(
                std::move(materials[model_index]),
                GxMeshEndCaller([meshes, end, model_index](GxMeshPtr&& mesh) {
                    (*meshes)[model_index] = std::move(mesh);
                    (void)end; }));
        }
    }

    void meshes_ready(std::array<GxMeshPtr, objects_count>& meshes)
    {
        for (std::uint32_t model_index = 0; model_index < objects_count; ++model_index) {
            auto entity = GxModelManager::get().build(
                "triangle" + std::to_string(model_index), scene_entity.get(),
                { std::move(meshes[model_index]) }, true);
            auto speed = Speed::construct<Speed>(entity.get());
            auto position = Position::construct<Position>(entity.get());
            auto* const trn = entity->get_component<GxTransform>();
            trn->set_local_position(position->value);
            trn->local_inner_scale(cube_size);
            entity->add_component(std::move(speed));
            entity->add_component(std::move(position));
        }

        const GxEndCaller<void> end([this] { scene_entity->add_to_world(); });

        GxCameraManager::get().build(
            "camera", scene_entity.get(),
            GxEntityEndCaller([this, end](GxEntityPtr&& entity) -> void {
                auto trn = entity->get_component_shared_ptr<GxTransform>();
                trn->set_local_position({ 0.0f, 0.0f, 5.0f });
                (void)GxConstraintManager::get().create_jet_controller(
                    entity->get_object_name() + "-controller", std::move(trn), scene_entity.get());
                (void)end;
            }));

        GxLightManager::get().build_shadow_caster_directional(
            "directional-light", scene_entity.get(),
            1024, 10.0f, 1.0f, 20.0f,
            GxEntityEndCaller([this, end](GxEntityPtr&& entity) {
                auto* const l = entity->get_component<GxShadowCaster>();
                l->get_shadow_transform()->local_look_at(
                    { 0.0, 0.0, 5.0 }, { 0.0, 0.0, 0.0 }, { 0.0, 1.0, 0.0 });
                l->colour = { 8.0f, 8.0f, 8.0f };
                (void)end;
            }));
    }

    void update() override
    {
        Application::update();
        GxWorld::get().parallel_system<GxAll<Speed, Position, GxTransform>>(
            [&](auto, Speed* const speed, Position* const position, GxTransform* const trn, const auto /*kernel_index*/) noexcept {
                position->update(GxRenderEngine::get().get_delta_time(), *speed);
                speed->update(*position);
                trn->set_local_position(position->value);
            });
    }
};

GEAROENIX_START(GameApp)
