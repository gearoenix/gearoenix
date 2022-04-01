#include <gearoenix/core/gx-cr-application.hpp>
#include <gearoenix/platform/gx-plt-log.hpp>
#include <gearoenix/render/engine/gx-rnd-eng-engine.hpp>
#include <gearoenix/render/scene/gx-rnd-scn-manager.hpp>
#include <imgui.h>

struct GameApp final : public gearoenix::core::Application {
    explicit GameApp(gearoenix::platform::Application& plt_app) noexcept;
    void update() noexcept final;
};

GameApp::GameApp(gearoenix::platform::Application& plt_app) noexcept
    : Application(plt_app)
{
}

void GameApp::update() noexcept
{
    // if (render_engine.get_frame_number_from_start() == 10)
    //     platform_application.get_base().get_render_engine()->get_scene_manager()->load_gltf(
    //         gearoenix::platform::stream::Path::create_absolute("/home/thany/Projects/assets/1.gltf"));
}

GEAROENIX_START(GameApp)