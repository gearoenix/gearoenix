#ifndef GEAROENIX_EDITOR_UI_MENU_BAR_HPP
#define GEAROENIX_EDITOR_UI_MENU_BAR_HPP

namespace gearoenix::platform {
    struct Application;
}

namespace gearoenix::editor::control {
    struct Manager;
}

namespace gearoenix::editor::ui {
struct MenuBar final {
private:
    platform::Application* const platform_application;
    editor::control::Manager *const control_manager;

    bool show_new_project_popup = false;

    void show_project() noexcept;
public:
    explicit MenuBar(platform::Application* platform_application, editor::control::Manager *control_manager) noexcept;
    void update() noexcept;
};
}

#endif