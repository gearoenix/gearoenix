#ifndef GEAROENIX_PHYSICS_CONSTRAINT_TYPE_HPP
#define GEAROENIX_PHYSICS_CONSTRAINT_TYPE_HPP
#include "../../core/cr-types.hpp"
namespace gearoenix::physics::constraint {
enum struct Type : core::Id {
    WindowPlacer = 1,
    Tracker = 2,
    Joint = 3,
};
}
#endif
