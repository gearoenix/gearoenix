#ifndef GEAROENIX_PHYSICS_CONSTRAINT_TYPE_HPP
#define GEAROENIX_PHYSICS_CONSTRAINT_TYPE_HPP
#include "../../core/gx-cr-types.hpp"
namespace gearoenix::physics::constraint {
enum struct Type : core::Id {
    WindowPlacer = 1,
    WindowScaler = 2,
    Tracker = 3,
    Joint = 4,
};
}
#endif
