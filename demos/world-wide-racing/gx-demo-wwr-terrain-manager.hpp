#ifndef GX_DEMO_WWR_TERRAIN_MANAGER_HPP
#define GX_DEMO_WWR_TERRAIN_MANAGER_HPP

#include <memory>
#include <vector>

namespace gearoenix::physics {
    class Transformation;
}

namespace gearoenix::render::camera {
    class Camera;
}

namespace gearoenix::render::model {
    class Dynamic;
}

namespace gearoenix::render::scene {
    class Game;
}

namespace gearoenix::demo::wwr {
class TerrainManager {
public:
    constexpr static std::size_t ground_pieces_count = 2;
    constexpr static double ground_pieces_breadth = 6.0;
private:
    const std::shared_ptr<render::scene::Game> scene;
    const std::shared_ptr<render::model::Dynamic> asphalt_piece;
    const std::shared_ptr<render::camera::Camera> camera;
    const std::shared_ptr<render::model::Dynamic> asphalt_pieces[ground_pieces_count];
    physics::Transformation *const camera_tran;

    void update() noexcept;

    public:
    explicit TerrainManager(std::shared_ptr<render::scene::Game> scene) noexcept;
    ~TerrainManager() noexcept;

};
}

#endif