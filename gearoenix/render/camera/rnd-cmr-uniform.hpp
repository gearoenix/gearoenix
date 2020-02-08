#ifndef GEAROENIX_RENDER_CAMERA_UNIFORM_HPP
#define GEAROENIX_RENDER_CAMERA_UNIFORM_HPP
#include "../../math/math-matrix.hpp"
#include "../../math/math-vector.hpp"

namespace gearoenix::render::camera {
struct Uniform {
    math::Vec3 position = math::Vec3(0.0f, 0.0f, 0.0f);
    core::Real aspect_ratio = 1.7f;
    /// far is negative
    core::Real far = -100.0f;
    /// near is negative
    core::Real near = -1.0f;
    core::Real clip_width = 0.0f;
    core::Real clip_height = 0.0f;
    core::Real clip_start_x = 0.0f;
    core::Real clip_start_y = 0.0f;
    math::Mat4x4 inversed_rotation;
    math::Mat4x4 view;
    math::Mat4x4 projection;
    math::Mat4x4 uniform_projection;
    math::Mat4x4 view_projection;
    math::Mat4x4 uniform_view_projection;
};
}
#endif
