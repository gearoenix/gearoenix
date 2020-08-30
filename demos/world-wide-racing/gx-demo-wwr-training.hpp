#ifndef GX_DEMO_WWR_TRAINING_HPP
#define GX_DEMO_WWR_TRAINING_HPP

#include <memory>
#include <vector>

namespace gearoenix::physics::constraint {
    class WindowPlacer;
    class WindowScaler;
}

namespace gearoenix::render::camera { class ArcController; }

namespace gearoenix::render::model {
    class Model;
}

namespace gearoenix::render::scene {
    class Logo;
    class Game;
    class Scene;
    class Ui;
}

namespace gearoenix::demo::wwr {
    class TerrainManager;
    class Garage;
    class CountDown;
    class Training final {
    private:
        std::shared_ptr<render::scene::Game> game_scene;
        std::shared_ptr<render::scene::Ui> ui_scene;
        std::shared_ptr<render::model::Model> car;
        std::unique_ptr<TerrainManager> terrain_manager;
        std::unique_ptr<render::camera::ArcController> cam_ctrl;
        std::shared_ptr<physics::constraint::WindowPlacer> break_button_placer, accelerator_button_placer;
        std::shared_ptr<CountDown> count_down;

        void initialize_scenes(const std::vector<std::shared_ptr<render::scene::Scene>>& scenes) noexcept;
        void initialize_camera() noexcept;
        void initialize_ui() noexcept;
        void initialize_buttons() noexcept;
        void initialize_cars() noexcept;
        void initialize_terrain() noexcept;
        void count_down_finished() noexcept;
    public:
        Training(const std::vector<std::shared_ptr<render::scene::Scene>>& scenes, Garage * garage) noexcept;
        ~Training() noexcept;
    };
}
#endif