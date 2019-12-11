#ifndef GEAROENIX_EXAMPLE_003_UI_WIDGETS_HPP
#define GEAROENIX_EXAMPLE_003_UI_WIDGETS_HPP
#include <atomic>
#include <gearoenix/core/cr-application.hpp>
#include <gearoenix/core/event/cr-ev-listner.hpp>
#include <random>

namespace gearoenix::render::graph::tree {
class Pbr;
}

namespace gearoenix::render::scene {
class Ui;
}

class Example003UiWidgetsApp : public gearoenix::core::Application,
                               public gearoenix::core::event::Listener {
private:
    using GxUiScene = gearoenix::render::scene::Ui;
    using GxGrPbr = gearoenix::render::graph::tree::Pbr;

    std::shared_ptr<GxUiScene> uiscn;
    std::unique_ptr<GxGrPbr> render_tree;

public:
    Example003UiWidgetsApp(gearoenix::system::Application* sys_app) noexcept;
    ~Example003UiWidgetsApp() noexcept final;
    void update() noexcept final;
    void terminate() noexcept final;
    bool on_event(const gearoenix::core::event::Data& event_data) noexcept final;
};
#endif
