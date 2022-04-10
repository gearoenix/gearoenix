#include <gearoenix/core/ecs/gx-cr-ecs-world.hpp>
#include <gearoenix/core/gx-cr-application.hpp>
#include <gearoenix/platform/stream/gx-plt-stm-path.hpp>
#include <gearoenix/render/camera/gx-rnd-cmr-jet-controller.hpp>
#include <gearoenix/render/engine/gx-rnd-eng-engine.hpp>
#include <gearoenix/render/gx-rnd-gltf-loader.hpp>
#include <gearoenix/render/scene/gx-rnd-scn-builder.hpp>
#include <gearoenix/render/scene/gx-rnd-scn-scene.hpp>

struct GameApp final : public gearoenix::core::Application {
    const std::unique_ptr<gearoenix::render::camera::JetController> camera_controller;

    explicit GameApp(gearoenix::platform::Application& plt_app) noexcept;
    void update() noexcept final;
};

GameApp::GameApp(gearoenix::platform::Application& plt_app) noexcept
    : Application(plt_app)
    , camera_controller(new gearoenix::render::camera::JetController(
          render_engine,
          *gearoenix::render::load_gltf(
              render_engine,
              gearoenix::platform::stream::Path::create_asset("1.glb"))[0]
               ->get_entity_builder()
               ->get_builder()
               .get_component<gearoenix::render::scene::Scene>()
               ->get_camera_entities()
               .begin()))
{
}

void GameApp::update() noexcept
{
    Application::update();
    camera_controller->update();
}

GEAROENIX_START(GameApp)