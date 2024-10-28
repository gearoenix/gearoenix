#include "gx-editor-main.hpp"
#include "ui/gx-ed-ui-manager.hpp"
#include "viewport/gx-ed-vp-viewport.hpp"

void gearoenix::editor::Application::update()
{
    ui_manager->update();
    viewport->update();
}

gearoenix::editor::Application::Application(platform::Application& plt_app)
    : core::Application(plt_app)
    , ui_manager(new ui::Manager(plt_app))
    , viewport(new viewport::Viewport(*this))
{
}

gearoenix::editor::Application::~Application() = default;

GEAROENIX_START(gearoenix::editor::Application)