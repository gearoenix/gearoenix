#include <gearoenix/core/gx-cr-application.hpp>
#include <gearoenix/platform/gx-plt-log.hpp>
#include "ui/gx-editor-ui-manager.hpp"

namespace gearoenix::editor {
    struct Application final : public core::Application {
    private:
        ui::Manager ui_manager;
    public:
        explicit Application(platform::Application *plt_app) noexcept;
        void update() noexcept final;
    };
}

gearoenix::editor::Application::Application(platform::Application* plt_app) noexcept
        : core::Application(plt_app)
        , ui_manager()
{
}

void gearoenix::editor::Application::update() noexcept
{
    ui_manager.update();
}

GEAROENIX_START(gearoenix::editor::Application)