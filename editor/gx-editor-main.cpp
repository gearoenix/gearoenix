#include "control/gx-editor-ctrl-manager.hpp"
#include "ui/gx-editor-ui-manager.hpp"
#include <gearoenix/core/gx-cr-application.hpp>

namespace gearoenix::editor {
struct Application final : core::Application {
private:
    control::Manager control_manager;
    ui::Manager ui_manager;

public:
    explicit Application(platform::Application& plt_app);
    void update() override;
};
}

gearoenix::editor::Application::Application(platform::Application& plt_app)
    : core::Application(plt_app)
    , control_manager(plt_app)
    , ui_manager(plt_app, control_manager)
{
}

void gearoenix::editor::Application::update()
{
    ui_manager.update();
}

GEAROENIX_START(gearoenix::editor::Application)