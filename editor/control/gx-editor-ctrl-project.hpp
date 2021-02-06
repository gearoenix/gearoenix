#ifndef GEAROENIX_EDITOR_CONTROL_PROJECT_HPP
#define GEAROENIX_EDITOR_CONTROL_PROJECT_HPP

#include <gearoenix/core/gx-cr-project.hpp>
#include <gearoenix/core/macro/gx-cr-mcr-getter-setter.hpp>
#include <optional>

namespace gearoenix::platform {
    struct Application;
}

namespace gearoenix::editor::control {
    struct Manager;
    struct Project final {
    GX_GET_CREF_PRV(std::vector<std::string>, recent_file_addresses)
    GX_GET_REF_PRV(std::string, project_name)
    GX_GET_CREF_PRV(std::optional<core::Project>, project)

    private:
        platform::Application* const platform_application;
        Manager* const manager;

    public:
        explicit Project(platform::Application* platform_application, Manager* manager) noexcept;
        void create_project() noexcept;
        void caption_changed() noexcept;
        [[nodiscard]] std::string get_caption() noexcept;
        [[nodiscard]] bool is_project_opened() noexcept;
    };
}

#endif