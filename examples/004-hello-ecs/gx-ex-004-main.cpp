#define GX_PLATFORM_LOG_STD_OUT_ENABLED
#include <gearoenix/core/allocator/gx-cr-alc-shared-array.hpp>
#include <gearoenix/core/ecs/gx-cr-ecs-world.hpp>
#include <gearoenix/core/gx-cr-application.hpp>
#include <gearoenix/physics/constraint/gx-phs-cns-manager.hpp>
#include <gearoenix/physics/gx-phs-engine.hpp>
#include <gearoenix/physics/gx-phs-transformation.hpp>
#include <gearoenix/render/camera/gx-rnd-cmr-builder.hpp>
#include <gearoenix/render/camera/gx-rnd-cmr-manager.hpp>
#include <gearoenix/render/engine/gx-rnd-eng-engine.hpp>
#include <gearoenix/render/gx-rnd-vertex.hpp>
#include <gearoenix/render/light/gx-rnd-lt-builder.hpp>
#include <gearoenix/render/light/gx-rnd-lt-directional.hpp>
#include <gearoenix/render/light/gx-rnd-lt-light.hpp>
#include <gearoenix/render/light/gx-rnd-lt-manager.hpp>
#include <gearoenix/render/material/gx-rnd-mat-manager.hpp>
#include <gearoenix/render/material/gx-rnd-mat-pbr.hpp>
#include <gearoenix/render/mesh/gx-rnd-msh-manager.hpp>
#include <gearoenix/render/model/gx-rnd-mdl-builder.hpp>
#include <gearoenix/render/model/gx-rnd-mdl-manager.hpp>
#include <gearoenix/render/scene/gx-rnd-scn-builder.hpp>
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
using GxEndCaller = gearoenix::core::job::EndCaller<>;

using GxComp = gearoenix::core::ecs::Component;
using GxCoreApp = gearoenix::core::Application;
using GxPltApp = gearoenix::platform::Application;
using GxTransformComp = gearoenix::physics::Transformation;
using GxTransform = gearoenix::physics::Transformation;

using GxScene = gearoenix::render::scene::Scene;

using GxCameraBuilder = gearoenix::render::camera::Builder;
using GxCameraBuilderPtr = std::shared_ptr<GxCameraBuilder>;
using GxCameraBuilderEndCaller = GxEndCallerShared<GxCameraBuilder>;

using GxLightBuilder = gearoenix::render::light::Builder;
using GxLightBuilderPtr = std::shared_ptr<GxLightBuilder>;
using GxLightBuilderEndCaller = GxEndCallerShared<GxLightBuilder>;

using GxMesh = gearoenix::render::mesh::Mesh;
using GxMeshPtr = std::shared_ptr<GxMesh>;
using GxMeshEndCaller = GxEndCallerShared<GxMesh>;

using GxPbr = gearoenix::render::material::Pbr;
using GxPbrPtr = std::shared_ptr<GxPbr>;
using GxPbrEndCaller = GxEndCallerShared<GxPbr>;

struct Position;
struct Speed final : GxComp {
    constexpr static std::uint32_t MAX_COUNT = objects_count;
    constexpr static TypeIndex TYPE_INDEX = 100;
    constexpr static TypeIndexSet ALL_PARENT_TYPE_INDICES {};
    constexpr static TypeIndexSet IMMEDIATE_PARENT_TYPE_INDICES {};

    gearoenix::math::Vec3<double> value;

    Speed();
    void update(const Position& p);
};

struct Position final : GxComp {
    constexpr static std::uint32_t MAX_COUNT = objects_count;
    constexpr static TypeIndex TYPE_INDEX = 101;
    constexpr static TypeIndexSet ALL_PARENT_TYPE_INDICES {};
    constexpr static TypeIndexSet IMMEDIATE_PARENT_TYPE_INDICES {};

    gearoenix::math::Vec3<double> value;

    Position();
    void update(double delta_time, const Speed& speed);
};

Speed::Speed()
    : GxComp(create_this_type_index(this), "speed", 0)
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

Position::Position()
    : GxComp(create_this_type_index(this), "position", 0)
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
    gearoenix::core::ecs::entity_id_t scene_id = gearoenix::core::ecs::INVALID_ENTITY_ID;

    explicit GameApp(GxPltApp& plt_app)
        : GxCoreApp(plt_app)
    {
        GxComp::register_type<Position>();
        GxComp::register_type<Speed>();

        const auto materials = std::make_shared<std::array<GxPbrPtr, objects_count>>();

        const GxEndCaller end([this, materials] { materials_ready(*materials); });

        for (std::uint32_t model_index = 0; model_index < objects_count; ++model_index) {
            render_engine.get_material_manager()->get_pbr(
                "material-" + std::to_string(model_index),
                GxPbrEndCaller([model_index, materials, end](GxPbrPtr&& m) {
                    m->get_albedo_factor() = {
                        colour_distribution(random_engine),
                        colour_distribution(random_engine),
                        colour_distribution(random_engine),
                        1.0f
                    };
                    (*materials)[model_index] = std::move(m);
                    (void)end;
                }));
        }
    }

    void materials_ready(std::array<GxPbrPtr, objects_count>& materials)
    {
        const auto meshes = std::make_shared<std::array<GxMeshPtr, objects_count>>();

        const GxEndCaller end([this, meshes] { meshes_ready(*meshes); });

        for (std::uint32_t model_index = 0; model_index < objects_count; ++model_index) {
            render_engine.get_mesh_manager()->build_cube(
                std::move(materials[model_index]),
                GxMeshEndCaller([meshes, end, model_index](GxMeshPtr&& mesh) {
                    (*meshes)[model_index] = std::move(mesh);
                    (void)end;
                }));
        }
    }

    void meshes_ready(std::array<GxMeshPtr, objects_count>& meshes)
    {
        const auto scene_builder = render_engine.get_scene_manager()->build(
            "scene", 0.0, GxEndCaller([this] {
                render_engine.get_world()->get_component<GxScene>(scene_id)->set_enabled(true);
            }));
        scene_id = scene_builder->get_id();

        for (std::uint32_t model_index = 0; model_index < objects_count; ++model_index) {
            auto model_builder = render_engine.get_model_manager()->build(
                "triangle" + std::to_string(model_index), nullptr,
                { std::move(meshes[model_index]) },
                GxEndCaller([] { }),
                true);
            auto speed = Speed::construct<Speed>();
            auto position = Position::construct<Position>();
            auto& model_transformation = model_builder->get_transformation();
            model_transformation.set_local_position(position->value);
            model_transformation.local_inner_scale(cube_size);
            model_builder->get_entity_builder()->get_builder().add_components(std::move(speed), std::move(position));
            scene_builder->add(std::move(model_builder));
        }

        render_engine.get_camera_manager()->build(
            "camera", nullptr,
            GxCameraBuilderEndCaller([this, scene_builder](GxCameraBuilderPtr&& camera_builder) {
                auto trn = std::dynamic_pointer_cast<GxTransform>(camera_builder->get_transformation().get_component_self().lock());
                trn->set_local_position({ 0.0f, 0.0f, 5.0f });
                const auto& cm = *render_engine.get_physics_engine()->get_constraint_manager();
                auto ctrl_name = camera_builder->get_entity_builder()->get_builder().get_name() + "-controller";
                (void)cm.create_jet_controller(std::move(ctrl_name), std::move(trn), GxEndCaller([] { }));
                scene_builder->add(std::move(camera_builder));
            }),
            GxEndCaller([] {}));

        render_engine.get_light_manager()->build_shadow_caster_directional(
            "directional-light", nullptr,
            1024,
            10.0f,
            1.0f,
            20.0f,
            GxLightBuilderEndCaller([scene_builder](GxLightBuilderPtr&& light_builder) {
                light_builder->get_shadow_caster_directional()->get_shadow_transform()->local_look_at(
                    { 0.0, 0.0, 5.0 }, { 0.0, 0.0, 0.0 }, { 0.0, 1.0, 0.0 });
                light_builder->get_light().colour = { 8.0f, 8.0f, 8.0f };
                scene_builder->add(std::move(light_builder));
            }),
            GxEndCaller([] {}));
    }

    void update() override
    {
        Application::update();
        render_engine.get_world()->parallel_system<GxAll<Speed, Position, GxTransformComp>>(
            [&](auto, Speed* const speed, Position* const position, GxTransformComp* const trn, const auto /*kernel_index*/) noexcept {
                position->update(render_engine.get_delta_time(), *speed);
                speed->update(*position);
                trn->set_local_position(position->value);
            });
    }
};

GEAROENIX_START(GameApp)
