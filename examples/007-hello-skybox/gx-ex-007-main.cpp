#include <gearoenix/core/gx-cr-application.hpp>
#include <gearoenix/platform/gx-plt-log.hpp>
#include <gearoenix/platform/stream/gx-plt-stm-path.hpp>
#include <gearoenix/render/camera/gx-rnd-cmr-builder.hpp>
#include <gearoenix/render/camera/gx-rnd-cmr-jet-controller.hpp>
#include <gearoenix/render/camera/gx-rnd-cmr-manager.hpp>
#include <gearoenix/render/engine/gx-rnd-eng-engine.hpp>
#include <gearoenix/render/scene/gx-rnd-scn-builder.hpp>
#include <gearoenix/render/scene/gx-rnd-scn-manager.hpp>
#include <gearoenix/render/scene/gx-rnd-scn-scene.hpp>
#include <gearoenix/render/skybox/gx-rnd-sky-manager.hpp>

struct GameApp final : public gearoenix::core::Application {
    std::unique_ptr<gearoenix::render::camera::JetController> camera_controller;

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
                camera_controller = std::make_unique<gearoenix::render::camera::JetController>(
                    render_engine,
                    camera_builder->get_id());
                scene_builder->add(std::move(camera_builder));
            }),
            gearoenix::core::job::EndCaller([] {}));

        GX_LOG_D("Initialised");
    }

    void update() noexcept final
    {
        Application::update();
        camera_controller->update();
    }
};

GEAROENIX_START(GameApp)
