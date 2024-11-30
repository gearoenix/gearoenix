#pragma once
#include <memory>
#include <string>

namespace gearoenix::render::imgui {
struct EntitySelector;
}

namespace gearoenix::editor::ui {
struct Manager;
struct MenuEntity final {
private:
    Manager& manager;

    bool is_create_skybox_open = false;
    std::string create_skybox_entity_name;
    std::string create_skybox_path;

    std::unique_ptr<render::imgui::EntitySelector> scene_selector;

    void show_create_skybox_window();
    void show_create_menu();

public:
    explicit MenuEntity(Manager& manager);
    ~MenuEntity();
    void update();
};
}