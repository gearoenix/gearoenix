#ifndef GEAROENIX_EDITOR_MAIN_HPP
#define GEAROENIX_EDITOR_MAIN_HPP
#include <gearoenix/core/gx-cr-application.hpp>
#include <memory>

namespace gearoenix::editor::ui {
struct Manager;
}

namespace gearoenix::editor::viewport {
struct Viewport;
}

namespace gearoenix::editor {
struct Application final : core::Application {
    GX_GET_UPTR_PRV(ui::Manager, ui_manager);
    GX_GET_UPTR_PRV(viewport::Viewport, viewport);

    void update() override;

public:
    explicit Application(platform::Application& plt_app);
    ~Application() override;
};
}

#endif