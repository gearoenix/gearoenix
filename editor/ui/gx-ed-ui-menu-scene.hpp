#pragma once
#include <boost/container/flat_set.hpp>
#include <gearoenix/core/ecs/gx-cr-ecs-entity-ptr.hpp>
#include <gearoenix/core/gx-cr-singleton.hpp>

namespace gearoenix::editor::ui {
struct MenuScene final : core::Singleton<MenuScene> {
private:
    boost::container::flat_set<core::ecs::EntityPtr> active_scenes;
    bool is_gltf_popup_open = false;

    bool is_new_popup_open = false;
    std::string new_scene_name;
    float new_scene_priority = 0.0f;

    void show_new_popup();

public:
    MenuScene();
    ~MenuScene() override;
    void update();
    void renew();
    [[nodiscard]] bool has_active_scene() const;
};
}