#pragma once
#include <set>
#include <string>

namespace gearoenix::editor::ui {
struct MenuWorld final {
private:
    bool is_new_popup_open = false;
    bool is_settings_open = false;
    bool is_quit_popup_open = false;
    bool is_file_chooser_open = false;
    bool is_file_chooser_save = false;
    bool has_active_save_file = false;
    std::set<std::string> recent_save_files;

    void show_new_popup();
    void show_settings();
    void show_quit_popup();

public:
    MenuWorld();
    ~MenuWorld();
    void update();
    void renew();
};
}