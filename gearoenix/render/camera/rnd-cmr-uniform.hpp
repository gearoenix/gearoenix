#ifndef GEAROENIX_RENDER_CAMERA_UNIFORM_HPP
#define GEAROENIX_RENDER_CAMERA_UNIFORM_HPP
#include "../../math/math-vector.hpp"
#include "../../math/math-matrix.hpp"

namespace gearoenix {
namespace render {
namespace camera {
struct Uniform
{
    math::Vec4 x_reserved;
    math::Vec4 y_reserved;
    math::Vec4 z_reserved;
    /// far is negative
    math::Vec4 position_far;
    /// near is negative
    math::Vec4 near_aspect_ratio_reserved;
    math::Mat4x4 inversed_rotation;
    math::Mat4x4 view;
    math::Mat4x4 projection;
    math::Mat4x4 uniform_projection;
    math::Mat4x4 view_projection;
    math::Mat4x4 uniform_view_projection;
};
}
}
}
#endif
