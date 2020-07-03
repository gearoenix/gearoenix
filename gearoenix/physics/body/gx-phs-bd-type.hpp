#ifndef GEAROENIX_PHYSICS_BODY_TYPE_HPP
#define GEAROENIX_PHYSICS_BODY_TYPE_HPP

#include "../../core/cr-types.hpp"

namespace gearoenix::physics::body {
enum struct Type : core::TypeId {
    Static = 1,
    Rigid = 2,
    SmoothRoadCar = 3,
};
}

#endif