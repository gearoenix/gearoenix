#pragma once
#include <boost/container/flat_set.hpp>
#include <gearoenix/core/ecs/gx-cr-ecs-entity-ptr.hpp>

namespace gearoenix::editor::ui {
struct MenuScene final {
private:
    boost::container::flat_set<core::ecs::EntityPtr> active_scenes;

public:
    MenuScene();
    void update();
    void renew();
};
}