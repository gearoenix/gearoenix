#ifndef GEAROENIX_EDITOR_UI_MENU_PROJECT_HPP
#define GEAROENIX_EDITOR_UI_MENU_PROJECT_HPP

namespace gearoenix::editor::ui {
struct Manager;
struct MenuProject final {
private:
    Manager& manager;
    bool is_new_popup_open = false;
    bool is_settings_open = false;

    void show_new_popup();
    void show_settings();

public:
    explicit MenuProject(Manager& manager);
    ~MenuProject();
    void update();
};
}
#endif