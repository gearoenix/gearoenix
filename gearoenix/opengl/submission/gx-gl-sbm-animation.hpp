#pragma once
#include "../../render/gx-rnd-build-configuration.hpp"
#ifdef GX_RENDER_OPENGL_ENABLED
#include "../../math/gx-math-matrix-4d.hpp"

namespace gearoenix::gl::submission {
struct Bone final {
    math::Mat4x4<float> m;
    math::Mat4x4<float> inv_m;
};
}

#endif