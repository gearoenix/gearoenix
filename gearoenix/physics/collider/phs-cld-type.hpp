#ifndef GEAROENIX_PHYSICS_COLLIDER_TYPE_HPP
#define GEAROENIX_PHYSICS_COLLIDER_TYPE_HPP
#include "../../core/cr-types.hpp"
#include <ostream>
namespace gearoenix::physics::collider {
enum struct Type : core::Id {
    GHOST = 1,
    MESH = 2,
    AABB = 3,
    SPHERE = 4,
};
std::ostream& operator<<(std::ostream& os, Type t) noexcept;
}
#endif
