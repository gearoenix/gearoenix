#ifndef GEAROENIX_MATH_RAY_HPP
#define GEAROENIX_MATH_RAY_HPP
#include "math-vector.hpp"
namespace gearoenix::math {
struct Ray3 {
    Vec3 origin;
    Vec3 normalized_direction;
};
}
#endif
