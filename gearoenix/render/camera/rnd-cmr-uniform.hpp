#ifndef GEAROENIX_RENDER_CAMERA_UNIFORM_HPP
#define GEAROENIX_RENDER_CAMERA_UNIFORM_HPP
#include "../../math/math-matrix.hpp"
#include "../../math/math-vector.hpp"

namespace gearoenix {
namespace render {
    namespace camera {
        struct Uniform {
            math::Vec4 x_reserved = math::Vec4(1.0f, 0.0f, 0.0f, 0.0f);
            math::Vec4 y_reserved = math::Vec4(0.0f, 1.0f, 0.0f, 0.0f);
            math::Vec4 z_reserved = math::Vec4(0.0f, 0.0f, 1.0f, 0.0f);
            /// far is negative
            math::Vec4 position_far = math::Vec4(0.0f, 0.0f, 0.0f, -100.0f);
            /// near is negative
            math::Vec4 near_aspect_ratio_reserved = math::Vec4(-1.0f, 1.7f, 0.0f, 0.0f);
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
