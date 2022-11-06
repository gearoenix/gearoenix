#define GX_PLATFORM_LOG_STD_OUT_ENABLED
#include <gearoenix/core/ecs/gx-cr-ecs-world.hpp>
#include <gearoenix/core/gx-cr-application.hpp>
#include <gearoenix/physics/gx-phs-transformation.hpp>
#include <gearoenix/platform/gx-plt-main-entry.hpp>
#include <gearoenix/render/camera/gx-rnd-cmr-jet-controller.hpp>
#include <gearoenix/render/camera/gx-rnd-cmr-manager.hpp>
#include <gearoenix/render/engine/gx-rnd-eng-engine.hpp>
#include <gearoenix/render/gx-rnd-vertex.hpp>
#include <gearoenix/render/light/gx-rnd-lt-builder.hpp>
#include <gearoenix/render/light/gx-rnd-lt-light.hpp>
#include <gearoenix/render/light/gx-rnd-lt-manager.hpp>
#include <gearoenix/render/material/gx-rnd-mat-manager.hpp>
#include <gearoenix/render/material/gx-rnd-mat-pbr.hpp>
#include <gearoenix/render/mesh/gx-rnd-msh-manager.hpp>
#include <gearoenix/render/model/gx-rnd-mdl-builder.hpp>
#include <gearoenix/render/model/gx-rnd-mdl-manager.hpp>
#include <gearoenix/render/scene/gx-rnd-scn-builder.hpp>
#include <gearoenix/render/scene/gx-rnd-scn-manager.hpp>
#include <gearoenix/render/texture/gx-rnd-txt-manager.hpp>
#include <random>

constexpr static double position_limit = 2.0;
constexpr static double cube_size = 0.05;
constexpr static double max_speed = cube_size * 2.5;

static std::random_device random_device;
static std::default_random_engine random_engine;
static std::uniform_real_distribution<double> space_distribution(-position_limit, position_limit);
static std::uniform_real_distribution<double> speed_distribution(-max_speed, max_speed);
static std::uniform_real_distribution<float> colour_distribution(0.5f, 1.0f);

struct GameApp final : public gearoenix::core::Application {
    template <typename... Ts>
    using And = gearoenix::core::ecs::And<Ts...>;

    struct Position;
    struct Speed final : public gearoenix::core::ecs::Component {
        gearoenix::math::Vec3<double> value;

        Speed() noexcept
            : gearoenix::core::ecs::Component(this)
            , value(
                  speed_distribution(random_engine),
                  speed_distribution(random_engine),
                  speed_distribution(random_engine))
        {
            value.x += value.x > 0 ? max_speed : -max_speed;
            value.y += value.y > 0 ? max_speed : -max_speed;
            value.z += value.z > 0 ? max_speed : -max_speed;
        }

        void update(const Position& p) noexcept
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

        Speed(Speed&&) noexcept = default;
    };

    struct Position final : public gearoenix::core::ecs::Component {
        gearoenix::math::Vec3<double> value;

        Position() noexcept
            : gearoenix::core::ecs::Component(this)
            , value(
                  space_distribution(random_engine),
                  space_distribution(random_engine),
                  space_distribution(random_engine))
        {
        }

        void update(const double delta_time, const Speed& speed) noexcept
        {
            value += speed.value * delta_time;
            value.clamp(-position_limit, position_limit);
        }

        Position(Position&&) noexcept = default;
    };

    std::unique_ptr<gearoenix::render::camera::JetController> camera_controller;

    explicit GameApp(gearoenix::platform::Application& plt_app) noexcept
        : Application(plt_app)
    {
        const auto scene_builder = render_engine.get_scene_manager()->build("scene");

        auto end_callback = gearoenix::core::sync::EndCallerIgnored([scene_builder] {});

        auto cube_mesh = render_engine.get_mesh_manager()->build_cube(
            gearoenix::core::sync::EndCallerIgnored(end_callback));

        const auto threads_count = std::thread::hardware_concurrency();
        const auto models_count = threads_count * 1000;
        for (auto model_index = decltype(threads_count) { 0 }; model_index < models_count; ++model_index) {
            auto material = render_engine.get_material_manager()->get_pbr(
                "material" + std::to_string(model_index),
                end_callback);
            material->set_albedo(render_engine.get_texture_manager()->create_2d_from_colour({ colour_distribution(random_engine),
                                                                                                colour_distribution(random_engine),
                                                                                                colour_distribution(random_engine),
                                                                                                1.0f },
                end_callback));
            auto model_builder = render_engine.get_model_manager()->build(
                "triangle" + std::to_string(model_index),
                std::shared_ptr<gearoenix::render::mesh::Mesh>(cube_mesh),
                std::move(material),
                gearoenix::core::sync::EndCallerIgnored(end_callback),
                true);
            Speed speed;
            Position position;
            auto& model_transformation = model_builder->get_transformation();
            model_transformation.set_local_location(position.value);
            model_transformation.local_scale(cube_size);
            model_builder->get_entity_builder()->get_builder().add_components(std::move(speed), std::move(position));
            scene_builder->add(std::move(model_builder));
        }

        auto camera_builder = render_engine.get_camera_manager()->build("camera");
        camera_builder->get_transformation().set_local_location({ 0.0f, 0.0f, 5.0f });
        camera_controller = std::make_unique<gearoenix::render::camera::JetController>(
            render_engine,
            camera_builder->get_entity_builder()->get_builder().get_id());
        scene_builder->add(std::move(camera_builder));

        auto light_builder_0 = render_engine.get_light_manager()->build_shadow_caster_directional(
            "directional-light-0",
            1024,
            10.0f,
            1.0f,
            35.0f,
            end_callback);
        light_builder_0->get_transformation().local_look_at({ 0.0, 0.0, 5.0 }, { 0.0, 0.0, 0.0 }, { 0.0, 1.0, 0.0 });
        light_builder_0->get_light().colour = { 2.0f, 2.0f, 2.0f };
        scene_builder->add(std::move(light_builder_0));
    }

    void update() noexcept final
    {
        Application::update();
        camera_controller->update();
        render_engine.get_world()->parallel_system<And<Speed, Position, gearoenix::physics::Transformation>>(
            [&](auto, Speed* const speed, Position* const position, gearoenix::physics::Transformation* const trn, const auto /*kernel_index*/) noexcept {
                position->update(render_engine.get_delta_time(), *speed);
                speed->update(*position);
                trn->set_local_location(position->value);
            });
    }
};

GEAROENIX_START(GameApp)
