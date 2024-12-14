#pragma once

namespace gearoenix::editor::ui {
struct Manager;
struct MenuWorld final {
private:
    Manager& manager;
    bool is_new_popup_open = false;
    bool is_settings_open = false;
    bool is_file_chooser_open = false;
    const char* file_chooser_title = nullptr;

    void show_new_popup();
    void show_settings();

public:
    explicit MenuWorld(Manager& manager);
    ~MenuWorld();
    void update();
};
}