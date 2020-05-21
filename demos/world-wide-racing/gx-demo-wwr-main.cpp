#include "gx-demo-wwr-main.hpp"
#include <gearoenix/system/sys-app.hpp>
#include <gearoenix/render/scene/rnd-scn-logo.hpp>
#include <gearoenix/render/graph/tree/rnd-gr-tr-pbr.hpp>
#include <gearoenix/render/engine/rnd-eng-engine.hpp>

gearoenix::demo::wwr::Main::Main(system::Application* const sys_app) noexcept
    : core::Application(sys_app) {

    tree = std::make_unique<render::graph::tree::Pbr>(render_engine, core::sync::EndCaller<core::sync::EndCallerIgnore>([]() {}));
    render_engine->set_render_tree(tree.get());
    logo_scene = render::scene::Logo::construct(
            system_application,
            { {0.0, 1024}, { 0.1, 1070} },
            [this] (const std::vector<std::shared_ptr<render::scene::Scene>>& s) {
                active_scenes = s;
                logo_scene = nullptr;
            });

}

GEAROENIX_START(gearoenix::demo::wwr::Main)