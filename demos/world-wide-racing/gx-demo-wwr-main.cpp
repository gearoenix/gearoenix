#include "gx-demo-wwr-main.hpp"
#include <gearoenix/system/sys-app.hpp>

gearoenix::demo::wwr::Main::Main(system::Application* const sys_app) noexcept
    : core::Application(sys_app) {
    // render::scene::Logo(sys_app, {{layer, next_scene_id}, {layer, next_scene_id}});
}

GEAROENIX_START(gearoenix::demo::wwr::Main)