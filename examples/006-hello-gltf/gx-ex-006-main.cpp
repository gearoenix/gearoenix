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

    explicit GameApp(gearoenix::platform::Application& plt_app) noexcept;
    void update() noexcept final;
};

GameApp::GameApp(gearoenix::platform::Application& plt_app) noexcept
    : Application(plt_app)
{
    gearoenix::core::sync::EndCaller end_callback([this] {
        render_engine.get_world()->get_component<gearoenix::render::scene::Scene>(scene_id)->enabled = true;
    });
    auto scene_builder = gearoenix::render::load_gltf(
        render_engine,
        gearoenix::platform::stream::Path::create_asset("1.glb"),
        end_callback)[0];
    scene_id = scene_builder->get_id();
    camera_controller = std::make_unique<gearoenix::render::camera::JetController>(
        render_engine,
        *scene_builder->get_scene().get_camera_entities().begin());
}

void GameApp::update() noexcept
{
    Application::update();
    camera_controller->update();
}

GEAROENIX_START(GameApp)