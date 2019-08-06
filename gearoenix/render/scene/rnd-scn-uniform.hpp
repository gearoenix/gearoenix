#ifndef GEAROENIX_RENDER_SCENE_UNIFORM_HPP
#define GEAROENIX_RENDER_SCENE_UNIFORM_HPP
#include "../../core/cr-build-configuration.hpp"
#include "../../math/math-vector.hpp"

namespace gearoenix {
namespace render {
    namespace scene {
        struct Uniform {
            math::Vec4 ambient_light = math::Vec4(0.3f, 0.3f, 0.3f, 1.0f);
            math::Vec4 directional_lights_color[GX_MAX_DIRECTIONAL_LIGHTS];
            math::Vec4 directional_lights_direction[GX_MAX_DIRECTIONAL_LIGHTS];
            /// directional, reserved
            math::Vec4 lights_count;
            /// samples-count, radius, z-tolerance, reserved
            math::Vec4 ssao_config;
        };
    }
}
}
#endif
