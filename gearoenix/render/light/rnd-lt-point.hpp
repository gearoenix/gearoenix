#ifndef GEAROEMIX_RENDER_LIGHT_POINT_HPP
#define GEAROEMIX_RENDER_LIGHT_POINT_HPP
#include "../../core/sync/cr-sync-end-caller.hpp"
#include "../../math/math-vector.hpp"
#include "rnd-lt-light.hpp"
namespace gearoenix {
namespace render {
    namespace light {
        class Point : public Light {
        private:
			math::Vec4 position_radius = math::Vec4(1.0, 1.0, 1.0, 1.0);

        public:
			Point(
                const core::Id my_id,
                const std::shared_ptr<system::stream::Stream>& f,
                const std::shared_ptr<engine::Engine>& e);
            ~Point();
            const math::Vec4& get_position_radius() const;
        };
    }
}
}
#endif
