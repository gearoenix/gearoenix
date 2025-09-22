#pragma once
#include <gearoenix/platform/stream/gx-plt-stm-path.hpp>

#include <memory>
#include <string>

namespace gearoenix::platform::stream {
struct Stream;
}

namespace gearoenix::render::imgui {
struct EntitySelector;
}

namespace gearoenix::editor::ui {
struct MenuEntity final {
private:
    bool is_create_camera_open = false;
    std::string create_camera_entity_name;
    bool is_create_skybox_open = false;
    std::string create_skybox_entity_name;
    std::shared_ptr<platform::stream::Stream> skybox_stream;
    platform::stream::Path create_skybox_file_path;

    std::unique_ptr<render::imgui::EntitySelector> scene_selector;

    void show_create_camera_window();
    void show_create_skybox_window();
    void show_create_menu();

public:
    MenuEntity();
    ~MenuEntity();
    void update();
    void renew();
};
}