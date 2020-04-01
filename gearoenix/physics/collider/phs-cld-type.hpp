#ifndef GEAROENIX_PHYSICS_COLLIDER_TYPE_HPP
#define GEAROENIX_PHYSICS_COLLIDER_TYPE_HPP
#include "../../core/cr-types.hpp"
#include <ostream>
namespace gearoenix::physics::collider {
enum struct Type : core::TypeId {
    Ghost = 1,
    Mesh = 2,
    Aabb = 3,
    Sphere = 4,
    Frustum = 5,
    Transform = 6,
};
std::ostream& operator<<(std::ostream& os, Type t) noexcept;
}
#endif
