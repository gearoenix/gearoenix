#pragma once
#include "../../render/gx-rnd-build-configuration.hpp"
#ifdef GX_RENDER_OPENGL_ENABLED
#include "../../math/gx-math-frustum.hpp"
#include "../../math/gx-math-matrix-4d.hpp"
#include "../gx-gl-types.hpp"

namespace gearoenix::gl::submission {
struct DirectionalShadow final {
    math::Mat4x4<float> normalised_vp;
    math::Vec3<float> direction;
    math::Vec3<float> colour;
    uint shadow_map_texture = static_cast<uint>(-1);
};

struct DirectionalShadowCaster final {
    math::Frustum<double> frustum;
    DirectionalShadow shadow_data;
};
}

#endif