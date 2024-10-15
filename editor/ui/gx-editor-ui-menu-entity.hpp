#ifndef GEAROENIX_EDITOR_UI_MENU_ENTITY_HPP
#define GEAROENIX_EDITOR_UI_MENU_ENTITY_HPP
#include <memory>
#include <string>

namespace gearoenix::render::scene {
struct Selector;
}

namespace gearoenix::editor::ui {
struct Manager;
struct MenuEntity final {
private:
    Manager& manager;

    bool is_create_skybox_open = false;
    std::string create_skybox_entity_name;
    std::string create_skybox_path;

    std::unique_ptr<render::scene::Selector> scene_selector;

    void show_create_skybox_window();
    void show_create_menu();

public:
    explicit MenuEntity(Manager& manager);
    ~MenuEntity();
    void update();
};
}

#endif