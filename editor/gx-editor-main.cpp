#include "ui/gx-editor-ui-manager.hpp"
#include "control/gx-editor-ctrl-manager.hpp"

#include <gearoenix/core/gx-cr-application.hpp>

namespace gearoenix::editor {
    struct Application final : public core::Application {
    private:
        control::Manager control_manager;
        ui::Manager ui_manager;

    public:
        explicit Application(platform::Application& plt_app) noexcept
        : core::Application(plt_app)
        , control_manager(plt_app)
        , ui_manager(plt_app, control_manager)
{
}

        void update() noexcept final;
    };
}

void gearoenix::editor::Application::update() noexcept
{
    ui_manager.update();
}

GEAROENIX_START(gearoenix::editor::Application)