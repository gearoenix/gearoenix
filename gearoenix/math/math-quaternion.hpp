#ifndef GEAROENIX_MATH_QUATERNION_HPP
#define GEAROENIX_MATH_QUATERNION_HPP
#include "../core/cr-types.hpp"
#include "math-matrix.hpp"

namespace gearoenix {
namespace math {
struct Quat
{
    core::Real x = 0.0f;
    core::Real y = 0.0f;
    core::Real z = 0.0f;
    core::Real w = 0.0f;

    Quat();
    Quat(const core::Real x, const core::Real y, const core::Real z, const core::Real w);
    Mat4x4 to_mat() const;
};
} // namespace math
} // namespace gearoenix
#endif
