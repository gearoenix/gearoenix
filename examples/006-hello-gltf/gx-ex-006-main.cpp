#include <gearoenix/core/ecs/gx-cr-ecs-world.hpp>
#include <gearoenix/core/gx-cr-application.hpp>
#include <gearoenix/platform/stream/gx-plt-stm-path.hpp>
#include <gearoenix/render/camera/gx-rnd-cmr-jet-controller.hpp>
#include <gearoenix/render/engine/gx-rnd-eng-engine.hpp>
#include <gearoenix/render/gx-rnd-gltf-loader.hpp>
#include <gearoenix/render/scene/gx-rnd-scn-builder.hpp>
#include <gearoenix/render/scene/gx-rnd-scn-scene.hpp>

struct GameApp final : public gearoenix::core::Application {
    std::unique_ptr<gearoenix::render::camera::JetController> camera_controller;
    gearoenix::core::ecs::entity_id_t scene_id;

    explicit GameApp(gearoenix::platform::Application& plt_app)
        : Application(plt_app)
    {
        gearoenix::core::job::EndCaller end_callback([this] {
            render_engine.get_world()->get_component<gearoenix::render::scene::Scene>(scene_id)->set_enabled(true);
        });

        gearoenix::render::gltf::load(
            render_engine,
            gearoenix::platform::stream::Path::create_asset("1.glb"),
            gearoenix::core::job::EndCaller<std::vector<std::shared_ptr<gearoenix::render::scene::Builder>>>([this, end_callback](auto&& ss) {
                auto scene_builder = std::move(ss[0]);
                scene_id = scene_builder->get_id();
                camera_controller = std::make_unique<gearoenix::render::camera::JetController>(
                    render_engine,
                    *scene_builder->get_scene().get_camera_entities().begin());
            }),
            end_callback);
    }

    void update() final
    {
        Application::update();
        camera_controller->update();
    }
};

GEAROENIX_START(GameApp)