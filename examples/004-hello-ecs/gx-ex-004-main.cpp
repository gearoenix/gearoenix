#define GX_PLATFORM_LOG_STD_OUT_ENABLED
#include <gearoenix/core/allocator/gx-cr-alc-shared-array.hpp>
#include <gearoenix/core/ecs/gx-cr-ecs-world.hpp>
#include <gearoenix/core/gx-cr-application.hpp>
#include <gearoenix/physics/gx-phs-transformation.hpp>
#include <gearoenix/render/camera/gx-rnd-cmr-builder.hpp>
#include <gearoenix/render/camera/gx-rnd-cmr-jet-controller.hpp>
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
constexpr std::size_t objects_count = 8000;

std::random_device random_device {};
std::default_random_engine random_engine { random_device() };
std::uniform_real_distribution<double> space_distribution(-position_limit, position_limit);
std::uniform_real_distribution<double> speed_distribution(-max_speed, max_speed);
std::uniform_real_distribution<float> colour_distribution(0.5f, 1.0f);

template <typename... Ts>
using GxAll = gearoenix::core::ecs::All<Ts...>;

template <typename T>
using GxEndCallerShared = gearoenix::core::job::EndCallerShared<T>;
using GxEndCaller = gearoenix::core::job::EndCaller<>;

using GxComp = gearoenix::core::ecs::Component;
using GxCoreApp = gearoenix::core::Application;
using GxPltApp = gearoenix::platform::Application;
using GxTransformComp = gearoenix::physics::TransformationComponent;

using GxScene = gearoenix::render::scene::Scene;

using GxCameraBuilder = gearoenix::render::camera::Builder;
using GxCameraBuilderPtr = std::shared_ptr<GxCameraBuilder>;
using GxCameraBuilderEndCaller = GxEndCallerShared<GxCameraBuilder>;
using GxJetCtrl = gearoenix::render::camera::JetController;

using GxLightBuilder = gearoenix::render::light::Builder;
using GxLightBuilderPtr = std::shared_ptr<GxLightBuilder>;
using GxLightBuilderEndCaller = GxEndCallerShared<GxLightBuilder>;

using GxMesh = gearoenix::render::mesh::Mesh;
using GxMeshPtr = std::shared_ptr<GxMesh>;
using GxMeshEndCaller = GxEndCallerShared<GxMesh>;

using GxPbr = gearoenix::render::material::Pbr;
using GxPbrPtr = std::shared_ptr<GxPbr>;
using GxPbrEndCaller = GxEndCallerShared<GxPbr>;

template <typename T>
using GxAllocator = gearoenix::core::allocator::SharedArray<T, objects_count>;

struct Position;
struct Speed final : GxComp {
    gearoenix::math::Vec3<double> value;

    Speed();
    [[nodiscard]] static std::shared_ptr<Speed> construct();
    void update(const Position& p);
    [[nodiscard]] const HierarchyTypes& get_hierarchy_types() const override;
};

struct Position final : GxComp {
    gearoenix::math::Vec3<double> value;

    Position();
    [[nodiscard]] static std::shared_ptr<Position> construct();
    void update(double delta_time, const Speed& speed);
    [[nodiscard]] const HierarchyTypes& get_hierarchy_types() const override;
};

Speed::Speed()
    : GxComp(create_this_type_index(this), "speed")
    , value(
          speed_distribution(random_engine),
          speed_distribution(random_engine),
          speed_distribution(random_engine))
{
    value.x += value.x > 0 ? max_speed : -max_speed;
    value.y += value.y > 0 ? max_speed : -max_speed;
    value.z += value.z > 0 ? max_speed : -max_speed;
}

std::shared_ptr<Speed> Speed::construct()
{
    static auto allocator = GxAllocator<Speed>::construct();
    auto result = allocator->make_shared();
    result->set_component_self(result);
    return result;
}

void Speed::update(const Position& p)
{
    const auto pos = p.value.abs() + 0.1f;
    if (pos.greater(position_limit).or_elements()) {
        gearoenix::math::Vec3<double> n;
        if (pos.x > position_limit) {
            if (p.value.x < 0.0) {
                n = gearoenix::math::Vec3<double>(1.0, 0.0, 0.0);
            } else {
                n = gearoenix::math::Vec3<double>(-1.0, 0.0, 0.0);
            }
            value = n.reflect(value);
        }
        if (pos.y > position_limit) {
            if (p.value.y < 0.0) {
                n = gearoenix::math::Vec3<double>(0.0, 1.0, 0.0);
            } else {
                n = gearoenix::math::Vec3<double>(0.0, -1.0, 0.0);
            }
            value = n.reflect(value);
        }
        if (pos.z > position_limit) {
            if (p.value.z < 0.0) {
                n = gearoenix::math::Vec3<double>(0.0, 0.0, 1.0);
            } else {
                n = gearoenix::math::Vec3<double>(0.0, 0.0, -1.0);
            }
            value = n.reflect(value);
        }
    }
}

const gearoenix::core::ecs::Component::HierarchyTypes& Speed::get_hierarchy_types() const
{
    const static auto types = generate_hierarchy_types(this);
    return types;
}

Position::Position()
    : GxComp(create_this_type_index(this), "position")
    , value(
          space_distribution(random_engine),
          space_distribution(random_engine),
          space_distribution(random_engine))
{
}

std::shared_ptr<Position> Position::construct()
{
    static auto allocator = GxAllocator<Position>::construct();
    auto result = allocator->make_shared();
    result->set_component_self(result);
    return result;
}

void Position::update(const double delta_time, const Speed& speed)
{
    value += speed.value * delta_time;
    value.clamp(-position_limit, position_limit);
}

const gearoenix::core::ecs::Component::HierarchyTypes& Position::get_hierarchy_types() const
{
    const static auto types = generate_hierarchy_types(this);
    return types;
}
}

struct GameApp final : GxCoreApp {
    std::unique_ptr<GxJetCtrl> camera_controller;
    gearoenix::core::ecs::entity_id_t scene_id;

    explicit GameApp(GxPltApp& plt_app) noexcept
        : GxCoreApp(plt_app)
    {
        const auto materials = std::make_shared<std::array<GxPbrPtr, objects_count>>();

        const GxEndCaller end([this, materials] { materials_ready(*materials); });

        for (std::size_t model_index = 0; model_index < objects_count; ++model_index) {
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

        for (std::size_t model_index = 0; model_index < objects_count; ++model_index) {
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

        for (std::size_t model_index = 0; model_index < objects_count; ++model_index) {
            auto model_builder = render_engine.get_model_manager()->build(
                "triangle" + std::to_string(model_index), nullptr,
                { std::move(meshes[model_index]) },
                GxEndCaller([] {}),
                true);
            auto speed = Speed::construct();
            auto position = Position::construct();
            auto& model_transformation = model_builder->get_transformation();
            model_transformation.set_local_location(position->value);
            model_transformation.local_scale(cube_size);
            model_builder->get_entity_builder()->get_builder().add_components(std::move(speed), std::move(position));
            scene_builder->add(std::move(model_builder));
        }

        render_engine.get_camera_manager()->build(
            "camera", nullptr,
            GxCameraBuilderEndCaller([this, scene_builder](GxCameraBuilderPtr&& camera_builder) {
                camera_builder->get_transformation().set_local_location({ 0.0f, 0.0f, 5.0f });
                camera_controller = std::make_unique<GxJetCtrl>(
                    render_engine,
                    camera_builder->get_id());
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

    void update() noexcept override
    {
        Application::update();
        camera_controller->update();
        render_engine.get_world()->parallel_system<GxAll<Speed, Position, GxTransformComp>>(
            [&](auto, Speed* const speed, Position* const position, GxTransformComp* const trn, const auto /*kernel_index*/) noexcept {
                position->update(render_engine.get_delta_time(), *speed);
                speed->update(*position);
                trn->set_local_location(position->value);
            });
    }
};

GEAROENIX_START(GameApp)
