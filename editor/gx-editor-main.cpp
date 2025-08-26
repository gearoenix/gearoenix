#include "gx-editor-main.hpp"
#include "ui/gx-ed-ui-manager.hpp"
#include "viewport/gx-ed-vp-viewport.hpp"

#include <gearoenix/platform/gx-plt-application.hpp>

namespace {
constexpr char default_caption[] = "Gearoenix Game Engine Editor [project: untitled, scene: gx-editor-default-scene]";
}

void gearoenix::editor::EditorApplication::update()
{
    ui_manager->update();
    viewport->update();
}

gearoenix::editor::EditorApplication::EditorApplication()
    : Singleton<EditorApplication>(this)
    , ui_manager(new ui::Manager())
    , viewport(new viewport::Viewport())
{
    platform::Application::get().set_caption(default_caption);
}

gearoenix::editor::EditorApplication::~EditorApplication() = default;

void gearoenix::editor::EditorApplication::renew()
{
    ui_manager->renew();
    viewport->renew();
    platform::Application::get().set_caption(default_caption);
}

void gearoenix::editor::EditorApplication::quit()
{
    platform::BaseApplication::get().close();
}

GEAROENIX_START(gearoenix::editor::EditorApplication)