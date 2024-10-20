#ifndef GEAROENIX_EDITOR_UI_WINDOW_SCENE_HPP
#define GEAROENIX_EDITOR_UI_WINDOW_SCENE_HPP

namespace gearoenix::platform {
struct Application;
}

namespace gearoenix::editor::control {
struct Manager;
}

namespace gearoenix::editor::ui {
struct Manager;
struct WindowScene final {
private:
    platform::Application& platform_application;
    control::Manager& control_manager;
    Manager& manager;

    bool show_window = false;

public:
    explicit WindowScene(Manager& manager);
    void update();
    [[nodiscard]] bool* get_show_window_ptr();
};
}

#endif