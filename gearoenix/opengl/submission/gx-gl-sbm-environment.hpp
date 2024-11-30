#pragma once
#include "../../render/gx-rnd-build-configuration.hpp"
#ifdef GX_RENDER_OPENGL_ENABLED
#include "../../math/gx-math-aabb.hpp"
#include "../../math/gx-math-matrix-4d.hpp"
#include "../gx-gl-types.hpp"

namespace gearoenix::gl::submission {
struct Skybox final {
    uint vertex_object = 0;
    uint albedo_txt = 0;
};

struct Reflection final {
    uint irradiance = 0;
    uint radiance = 0;
    math::Aabb3<double> box;
    double size = -std::numeric_limits<double>::max();
    float radiance_mips_count = 0.0f;
};
}

#endif