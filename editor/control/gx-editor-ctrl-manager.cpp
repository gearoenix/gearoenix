#include "gx-editor-ctrl-manager.hpp"

gearoenix::editor::control::Manager::Manager(platform::Application * const platform_application) noexcept
    : project(platform_application)
    , platform_application(platform_application)
{
}
