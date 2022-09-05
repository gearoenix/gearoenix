#include "gx-editor-ctrl-manager.hpp"
#include <gearoenix/platform/gx-plt-application.hpp>

gearoenix::editor::control::Manager::Manager(platform::Application& platform_application) noexcept
    : project(platform_application, *this)
    , platform_application(platform_application)
{
    caption_changed();
}

void gearoenix::editor::control::Manager::caption_changed() noexcept {
    platform_application.set_caption("Gearoenix Game Engine Editor" + project.get_caption());
}
