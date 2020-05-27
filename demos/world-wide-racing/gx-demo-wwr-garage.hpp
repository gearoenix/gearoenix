#ifndef GX_DEMO_WWR_GARAGE_HPP
#define GX_DEMO_WWR_GARAGE_HPP

#include <memory>
#include <vector>

namespace gearoenix::render::camera { class ArcController; }

namespace gearoenix::render::scene {
    class Logo;
    class Scene;
}

namespace gearoenix::demo::wwr {
    class Garage final {
    private:
        std::shared_ptr<render::scene::Scene> game_scene, ui_scene;
        std::unique_ptr<render::camera::ArcController> cam_ctrl;

    public:
        explicit Garage(const std::vector<std::shared_ptr<render::scene::Scene>>& scenes) noexcept;
        ~Garage() noexcept;
    };
}
#endif