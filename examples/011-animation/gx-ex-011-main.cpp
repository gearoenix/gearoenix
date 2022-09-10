#include <gearoenix/core/ecs/gx-cr-ecs-world.hpp>
#include <gearoenix/core/gx-cr-application.hpp>
#include <gearoenix/platform/stream/gx-plt-stm-path.hpp>
#include <gearoenix/render/camera/gx-rnd-cmr-jet-controller.hpp>
#include <gearoenix/render/engine/gx-rnd-eng-engine.hpp>
#include <gearoenix/render/gx-rnd-gltf-loader.hpp>
#include <gearoenix/render/reflection/gx-rnd-rfl-manager.hpp>
#include <gearoenix/render/scene/gx-rnd-scn-builder.hpp>
#include <gearoenix/render/scene/gx-rnd-scn-scene.hpp>
#include <gearoenix/render/skybox/gx-rnd-sky-manager.hpp>

struct GameApp final : public gearoenix::core::Application {
    std::unique_ptr<gearoenix::render::camera::JetController> camera_controller;

    explicit GameApp(gearoenix::platform::Application& plt_app) noexcept;
    void update() noexcept final;
};

GameApp::GameApp(gearoenix::platform::Application& plt_app) noexcept
    : Application(plt_app)
{
    auto scene_builders = gearoenix::render::load_gltf(
        render_engine,
        gearoenix::platform::stream::Path::create_asset("example-11.glb"));
    auto& scene_builder = scene_builders[0];

    gearoenix::core::sync::EndCallerIgnored end_callback([scene_builder] {});

    auto skybox_builder = render_engine.get_skybox_manager()->build(
        "hello-skybox",
        gearoenix::platform::stream::Path::create_asset("sky.gx-cube-texture"),
        end_callback);
    scene_builder->add(std::move(skybox_builder));

    auto baked_reflection_probe_builder = render_engine.get_reflection_manager()->build_baked(
        "baked-reflection",
        gearoenix::platform::stream::Path::create_asset("exported.gx-reflection"),
        end_callback);
    scene_builder->add(std::move(baked_reflection_probe_builder));

    camera_controller = std::make_unique<gearoenix::render::camera::JetController>(
        render_engine,
        *scene_builder
             ->get_entity_builder()
             ->get_builder()
             .get_component<gearoenix::render::scene::Scene>()
             ->get_camera_entities()
             .begin());
}

void GameApp::update() noexcept
{
    Application::update();
    camera_controller->update();
}

GEAROENIX_START(GameApp)