#ifndef GEAROENIX_PHYSICS_CONSTRAINT_TYPE_HPP
#define GEAROENIX_PHYSICS_CONSTRAINT_TYPE_HPP
#include "../../core/cr-types.hpp"
namespace gearoenix {
namespace physics {
    namespace constraint {
        class Type {
        public:
            typedef enum : core::Id {
                PLACER = 1,
                TRACKER_SPRING_JOINT_SPRING = 2,
                LOOKER = 3,
            } Id;
        };
    }
}
}
#endif
