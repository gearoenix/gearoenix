#ifndef GEAROENIX_RENDER_SCENE_LOGO_HPP
#define GEAROENIX_RENDER_SCENE_LOGO_HPP
#include "../../core/cr-static.hpp"
#include <functional>
#include <vector>

namespace gearoenix::system {
class Application;
}

namespace gearoenix::render::model {
class Manager;
}

namespace gearoenix::render::engine {
class Engine;
}

namespace gearoenix::render::scene {
class Scene;
/// This is a helper class to display Gearoenix logo for loading
///
/// You can use this class to display Gearoenix logo for loading other scene.
/// It can be used for cut scenes and starting scene and on the other hand it can be a good acknowledgement for
/// this project
class Logo final {
public:
    typedef std::function<void(const std::vector<std::shared_ptr<Scene>>&)> FinishCallback;
    typedef std::vector<std::pair<double, core::Id>> PriorityIds;

    GX_GET_CREF_PRV(std::shared_ptr<Scene>, logo_scene)
    GX_GET_CREF_PRV(std::vector<std::shared_ptr<Scene>>, next_scenes)
private:
    model::Manager* const model_manager;
    engine::Engine* const render_engine;
    const PriorityIds next_scenes_priority_id;
    const FinishCallback on_finished;
    std::weak_ptr<Logo> self;
    const double gear_rotation_speed = 1.5;
    const double glare_rotation_speed = -1.0;
    const double wing_rotation_speed = 0.7;
    const double wing_max_rotation = 0.5;
    const double max_scale = 1.6;
    const double scale_duration = 0.7;
    double wing_rotation = -wing_max_rotation;
    double scale = 1.0 / max_scale;

    void on_load(const std::shared_ptr<Scene>&) noexcept;

    Logo(
        model::Manager* model_manager,
        engine::Engine* render_engine,
        PriorityIds next_scenes_priority_id,
        FinishCallback on_finished) noexcept;

public:
    static std::shared_ptr<Logo> construct(
        system::Application* sys_app,
        PriorityIds next_scenes_priority_id,
        FinishCallback on_finished) noexcept;
};
}
#endif
