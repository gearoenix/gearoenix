#ifndef GEAROEMIX_RENDER_LIGHT_DIRECTIONAL_HPP
#define GEAROEMIX_RENDER_LIGHT_DIRECTIONAL_HPP
#include "../../core/cr-build-configuration.hpp"
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
        struct DirectionalUniform {
            math::Vec4 color_strength;
            math::Vec4 direction;
            math::Vec4 vps[GX_SHADOW_CASCADES_COUNT];
            math::Vec4 vpbs[GX_SHADOW_CASCADES_COUNT];
        };

        class Directional : public Light {
        protected:
            math::Vec4 direction;

        public:
            Directional(
                const core::Id my_id,
                const std::shared_ptr<system::stream::Stream>& f,
                const std::shared_ptr<engine::Engine>& e);
            const math::Vec4& get_direction() const;
            math::Vec4& get_direction();
        };
    }
}
}
#endif
