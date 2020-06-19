#ifndef GX_DEMO_WWR_TERRAIN_MANAGER_HPP
#define GX_DEMO_WWR_TERRAIN_MANAGER_HPP

#include <memory>
#include <vector>

namespace gearoenix::physics {
    class Transformation;
}

namespace gearoenix::render::model {
    class Static;
}

namespace gearoenix::demo::wwr {
    class TerrainManager {
        private:
        const std::shared_ptr<render::model::Static> asphalt_piece;
        std::shared_ptr<render::model::Static> asphalt_piece[2];
        physics::Transformation *const camera_tran;
        
        void update() noexcept;

        public:
        TerrainManager() noexcept;
        ~TerrainManager() noexcept;

    };
}

#endif