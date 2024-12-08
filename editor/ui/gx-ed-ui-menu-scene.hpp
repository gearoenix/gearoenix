#pragma once
#include <gearoenix/core/ecs/gx-cr-ecs-types.hpp>

namespace gearoenix::platform {
struct Application;
}

namespace gearoenix::editor::ui {
struct Manager;
struct MenuScene final {
private:
    platform::Application& platform_application;
    Manager& manager;

    core::ecs::entity_id_t active_scene = core::ecs::invalid_entity_id;

public:
    explicit MenuScene(Manager& manager);
    void update();
};
}