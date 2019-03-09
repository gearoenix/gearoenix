#ifndef GEAROEMIX_RENDER_LIGHT_DIRECTIONAL_HPP
#define GEAROEMIX_RENDER_LIGHT_DIRECTIONAL_HPP
#include "../../math/math-matrix.hpp"
#include "../../math/math-vector.hpp"
#include "rnd-lt-light.hpp"
namespace gearoenix {
namespace render {
    namespace camera {
        class Orthographic;
    }
    namespace light {
        class Directional : public Light {
        protected:
			math::Mat4x4 view;
			math::Mat4x4 projection;
			math::Mat4x4 view_projection;
			math::Mat4x4 view_projection_biases;
        public:
            const math::Mat4x4& get_view_projection_biases() const;
        };
    }
}
}
#endif
