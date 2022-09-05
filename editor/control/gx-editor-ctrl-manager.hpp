#ifndef GEAROENIX_EDITOR_CONTROL_MANAGER_HPP
#define GEAROENIX_EDITOR_CONTROL_MANAGER_HPP
#include "gx-editor-ctrl-project.hpp"

namespace gearoenix::platform {
    struct Application;
}

namespace gearoenix::editor::control {
    struct Manager final {
        GX_GET_REF_PRV(Project, project)
    private:
        platform::Application& platform_application;

    public:
        explicit Manager(platform::Application& platform_application) noexcept;
        void caption_changed() noexcept;
    };
}

#endif