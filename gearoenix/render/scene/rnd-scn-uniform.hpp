#ifndef GEAROENIX_RENDER_SCENE_UNIFORM_HPP
#define GEAROENIX_RENDER_SCENE_UNIFORM_HPP
#include "../../core/cr-build-configuration.hpp"
#include "../../math/math-vector-4d.hpp"

namespace gearoenix::render::scene {
struct Uniform {
    math::Vec4<float> ambient_light = math::Vec4(0.05f, 0.05f, 0.05f, 1.0f);
    math::Vec4<float> directional_lights_color[GX_MAX_DIRECTIONAL_LIGHTS];
    math::Vec4<float> directional_lights_direction[GX_MAX_DIRECTIONAL_LIGHTS];
    /// directional, reserved
    math::Vec4<float> lights_count;
    /// samples-count, radius, z-tolerance, reserved
    math::Vec4<float> ssao_config;
};
}
#endif
