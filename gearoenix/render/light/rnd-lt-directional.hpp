#ifndef GEAROEMIX_RENDER_LIGHT_DIRECTIONAL_HPP
#define GEAROEMIX_RENDER_LIGHT_DIRECTIONAL_HPP
#include "../../core/sync/cr-sync-end-caller.hpp"
#include "../../math/math-matrix.hpp"
#include "../../math/math-vector.hpp"
#include "rnd-lt-light.hpp"
namespace gearoenix {
namespace render {
    namespace camera {
		class Camera;
        class Orthographic;
    }
	namespace model {
		class Model;
	}
    namespace light {
		class DirectionalModelShadowData;
        class Directional : public Light {
        protected:
			math::Vec4 direction;
        public:
			Directional(
				const core::Id my_id,
				const std::shared_ptr<system::stream::Stream>& f,
				const std::shared_ptr<engine::Engine>& e);
			const math::Vec4 &get_direction() const;
			math::Vec4 &get_direction();
        };
    }
}
}
#endif
