#ifndef GEAROENIX_PHYSICS_BODY_TYPE_HPP
#define GEAROENIX_PHYSICS_BODY_TYPE_HPP

#include "../../core/gx-cr-types.hpp"

namespace gearoenix::physics::body {
enum struct Type : core::TypeId {
    Static = 1,
    Rigid = 2,
    Car = 3,
};
}

#endif