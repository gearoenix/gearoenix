#include <gearoenix/core/ecs/gx-cr-ecs-entity-builder.hpp>
#include <gearoenix/core/gx-cr-application.hpp>
#include <gearoenix/physics/constraint/gx-phs-cns-manager.hpp>
#include <gearoenix/physics/gx-phs-engine.hpp>
#include <gearoenix/physics/gx-phs-transformation.hpp>
#include <gearoenix/platform/gx-plt-log.hpp>
#include <gearoenix/platform/stream/gx-plt-stm-path.hpp>
#include <gearoenix/render/camera/gx-rnd-cmr-builder.hpp>
#include <gearoenix/render/camera/gx-rnd-cmr-manager.hpp>
#include <gearoenix/render/engine/gx-rnd-eng-engine.hpp>
#include <gearoenix/render/scene/gx-rnd-scn-builder.hpp>
#include <gearoenix/render/scene/gx-rnd-scn-manager.hpp>
#include <gearoenix/render/scene/gx-rnd-scn-scene.hpp>
#include <gearoenix/render/skybox/gx-rnd-sky-manager.hpp>

struct GameApp final : gearoenix::core::Application {
    explicit GameApp(gearoenix::platform::Application& plt_app)
        : Application(plt_app)
    {
        const auto scene_builder = render_engine.get_scene_manager()->build(
            "scene", 0.0, gearoenix::core::job::EndCaller([] { }));
        scene_builder->get_scene().set_enabled(true);

        render_engine.get_skybox_manager()->build(
            "hello-skybox",
            gearoenix::platform::stream::Path::create_asset("sky.hdr"),
            gearoenix::core::job::EndCaller([] {}),
            gearoenix::core::job::EndCallerShared<gearoenix::render::skybox::Builder>([scene_builder](auto&& skybox_builder) {
                scene_builder->add(std::move(skybox_builder));
            }));

        render_engine.get_camera_manager()->build(
            "camera", nullptr,
            gearoenix::core::job::EndCallerShared<gearoenix::render::camera::Builder>([this, scene_builder](auto&& camera_builder) {
                auto trn = std::dynamic_pointer_cast<gearoenix::physics::Transformation>(camera_builder->get_transformation().get_component_self().lock());
                (void)render_engine.get_physics_engine()->get_constraint_manager()->create_jet_controller(
                    camera_builder->get_entity_builder()->get_builder().get_name() + "-controller",
                    std::move(trn),
                    gearoenix::core::job::EndCaller([] { }));
                scene_builder->add(std::move(camera_builder));
            }),
            gearoenix::core::job::EndCaller([] {}));

        GX_LOG_D("Initialised");
    }

    void update() override
    {
        Application::update();
    }
};

GEAROENIX_START(GameApp)
