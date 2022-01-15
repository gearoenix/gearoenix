#define GX_PLATFORM_LOG_STD_OUT_ENABLED
#include <gearoenix/core/ecs/gx-cr-ecs-world.hpp>
#include <gearoenix/core/gx-cr-application.hpp>
#include <gearoenix/physics/gx-phs-transformation.hpp>
#include <gearoenix/platform/gx-plt-log.hpp>
#include <gearoenix/platform/gx-plt-main-entry.hpp>
#include <gearoenix/render/camera/gx-rnd-cmr-jet-controller.hpp>
#include <gearoenix/render/camera/gx-rnd-cmr-manager.hpp>
#include <gearoenix/render/engine/gx-rnd-eng-engine.hpp>
#include <gearoenix/render/gx-rnd-vertex.hpp>
#include <gearoenix/render/material/gx-rnd-mat-pbr.hpp>
#include <gearoenix/render/mesh/gx-rnd-msh-manager.hpp>
#include <gearoenix/render/model/gx-rnd-mdl-builder.hpp>
#include <gearoenix/render/model/gx-rnd-mdl-manager.hpp>
#include <gearoenix/render/scene/gx-rnd-scn-builder.hpp>
#include <gearoenix/render/scene/gx-rnd-scn-manager.hpp>
#include <gearoenix/render/texture/gx-rnd-txt-manager.hpp>
#include <random>

constexpr double position_limit = 10.0;

static std::random_device random_device;
static std::default_random_engine random_engine;
static std::uniform_real_distribution<double> space_distribution(-position_limit, position_limit);
static std::uniform_real_distribution<double> speed_distribution(-1.0, 1.0);
static std::uniform_real_distribution<float> colour_distribution(0.0f, 1.0f);

struct GameApp final : public gearoenix::core::Application {
    struct Speed final : public gearoenix::core::ecs::Component {
        gearoenix::math::Vec3<double> value;

        Speed() noexcept
            : gearoenix::core::ecs::Component(this)
            , value(
                  speed_distribution(random_engine),
                  speed_distribution(random_engine),
                  speed_distribution(random_engine))
        {
        }

        void update() noexcept
        {
            value.x = speed_distribution(random_engine);
            value.y = speed_distribution(random_engine);
            value.z = speed_distribution(random_engine);
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

        auto model_builder = render_engine.get_model_manager()->build(
            "triangle",
            std::shared_ptr<gearoenix::render::mesh::Mesh>(cube_mesh),
            gearoenix::core::sync::EndCallerIgnored(end_callback),
            true);
        gearoenix::render::material::Pbr material(render_engine);
        model_builder->set_material(material);
        scene_builder->add(std::move(model_builder));

        auto camera_builder = render_engine.get_camera_manager()->build("camera");
        camera_builder->get_transformation().set_location(0.0f, 0.0f, 5.0f);
        camera_controller = std::make_unique<gearoenix::render::camera::JetController>(
            render_engine,
            camera_builder->get_entity_builder()->get_builder().get_id());
        scene_builder->add(std::move(camera_builder));
    }

    void update() noexcept final
    {
        Application::update();
        camera_controller->update();
    }
};

GEAROENIX_START(GameApp)