#ifndef GX_DEMO_WWR_GARAGE_HPP
#define GX_DEMO_WWR_GARAGE_HPP

#include <memory>
#include <vector>

namespace gearoenix::physics::constraint {
    class WindowPlacer;
    class WindowScaler;
}

namespace gearoenix::render::camera { class ArcController; }

namespace gearoenix::render::scene {
    class Logo;
    class Scene;
}

namespace gearoenix::demo::wwr {
    class RotatingButton;
    class Garage final {
    private:
        std::shared_ptr<render::scene::Scene> game_scene, ui_scene;
        std::unique_ptr<render::camera::ArcController> cam_ctrl;
        std::unique_ptr<RotatingButton> shop_button, multiplayer_button, tuning_button, training_button;
        std::shared_ptr<physics::constraint::WindowPlacer> next_button_placer, previous_button_placer;
        std::shared_ptr<physics::constraint::WindowScaler> bottom_button_scaler;

        void initialize_scenes(const std::vector<std::shared_ptr<render::scene::Scene>>& scenes) noexcept;
        void initialize_camera() noexcept;
        void initialize_buttons() noexcept;
    public:
        explicit Garage(const std::vector<std::shared_ptr<render::scene::Scene>>& scenes) noexcept;
        ~Garage() noexcept;
    };
}
#endif