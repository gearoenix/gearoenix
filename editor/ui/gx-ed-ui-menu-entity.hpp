#pragma once
#include <memory>
#include <string>

namespace gearoenix::render::imgui {
struct EntitySelector;
}

namespace gearoenix::editor::ui {
struct MenuEntity final {
private:
    bool is_create_skybox_open = false;
    std::string create_skybox_entity_name;
    std::string create_skybox_path;

    std::unique_ptr<render::imgui::EntitySelector> scene_selector;

    void show_create_skybox_window();
    void show_create_menu();

public:
    MenuEntity();
    ~MenuEntity();
    void update();
    void renew();
};
}