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
            render::scene::Logo::PriorityIds(),
            [] (const std::vector<std::shared_ptr<render::scene::Scene>>&) {});

}

GEAROENIX_START(gearoenix::demo::wwr::Main)