#ifndef GEAROENIX_RENDER_SCENE_LOGO_HPP
#define GEAROENIX_RENDER_SCENE_LOGO_HPP
#include "../../core/event/cr-ev-listener.hpp"
#include "rnd-scn-scene.hpp"
#include <chrono>

namespace gearoenix::render::widget {
class Widget;
class Edit;
}

namespace gearoenix::render::scene {
class Logo final {
private:
public:
    Logo(
        core::Id logo_scene_id,
        system::Application* sys_app,
        const std::vector<std::pair<double, core::Id>>& next_scenes_with_priorities,
        std::function<void(std::vector<std::shared_ptr<Scene>>)> on_finished) noexcept;
};
}
#endif
