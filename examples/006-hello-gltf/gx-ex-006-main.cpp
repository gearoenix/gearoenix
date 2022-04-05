#include <gearoenix/core/gx-cr-application.hpp>
#include <gearoenix/platform/stream/gx-plt-stm-path.hpp>
#include <gearoenix/render/engine/gx-rnd-eng-engine.hpp>
#include <gearoenix/render/gx-rnd-gltf-loader.hpp>

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
    if (render_engine.get_frame_number_from_start() == 10)
        (void)gearoenix::render::load_gltf(render_engine, gearoenix::platform::stream::Path::create_asset("1.glb"));
}

GEAROENIX_START(GameApp)