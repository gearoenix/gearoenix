#include "gx-demo-wwr-main.hpp"
#include "gx-demo-wwr-garage.hpp"
#include <gearoenix/render/engine/gx-rnd-eng-engine.hpp>
#include <gearoenix/render/graph/tree/gx-rnd-gr-tr-pbr.hpp>
#include <gearoenix/render/scene/gx-rnd-scn-logo.hpp>
#include <gearoenix/system/gx-sys-app.hpp>

gearoenix::demo::wwr::Main::Main(system::Application* const sys_app) noexcept
    : core::Application(sys_app)
{

    tree = std::make_unique<render::graph::tree::Pbr>(render_engine, core::sync::EndCaller<core::sync::EndCallerIgnore>([]() {}));
    render_engine->set_render_tree(tree.get());
    logo_scene = render::scene::Logo::construct(
        system_application,
        { { 0.0, "garage" }, { 0.1, "garage-main-ui" } },
        [this](const std::vector<std::shared_ptr<render::scene::Scene>>& s) {
            garage = std::make_unique<Garage>(s);
            logo_scene = nullptr;
        });
}

GEAROENIX_START(gearoenix::demo::wwr::Main)