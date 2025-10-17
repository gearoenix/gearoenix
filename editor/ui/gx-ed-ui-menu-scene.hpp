#pragma once
#include <boost/container/flat_set.hpp>
#include <gearoenix/core/ecs/gx-cr-ecs-entity-ptr.hpp>
#include <gearoenix/core/gx-cr-singleton.hpp>
#include <gearoenix/core/macro/gx-cr-mcr-getter-setter.hpp>

namespace gearoenix::editor::ui {
struct MenuScene final : core::Singleton<MenuScene> {
    GX_GET_PTR_PRV(core::ecs::Entity, current_scene);

    boost::container::flat_set<core::ecs::EntityPtr> active_scenes;

    bool is_new_popup_open = false;
    std::string new_scene_name;
    float new_scene_priority = 0.0f;

    void show_new_popup();
    void set_current_scene(core::ecs::Entity* scene);

public:
    MenuScene();
    ~MenuScene() override;
    void update();
    void renew();
    [[nodiscard]] bool has_active_scene() const;
};
}