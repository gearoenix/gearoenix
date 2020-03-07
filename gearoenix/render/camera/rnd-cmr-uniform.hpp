#ifndef GEAROENIX_RENDER_CAMERA_UNIFORM_HPP
#define GEAROENIX_RENDER_CAMERA_UNIFORM_HPP
#include "../../math/math-matrix-4d.hpp"

namespace gearoenix::render::camera {
struct Uniform {
    math::Vec3<float> position = math::Vec3(0.0f, 0.0f, 0.0f);
    float aspect_ratio = 1.7f;
    /// far is negative
    float far = -100.0f;
    /// near is negative
    float near = -1.0f;
    float clip_width = 0.0f;
    float clip_height = 0.0f;
    float clip_start_x = 0.0f;
    float clip_start_y = 0.0f;
    math::Mat4x4<float> inverted_rotation;
    math::Mat4x4<float> view;
    math::Mat4x4<float> projection;
    math::Mat4x4<float> uniform_projection;
    math::Mat4x4<float> view_projection;
    math::Mat4x4<float> uniform_view_projection;
};
}
#endif
