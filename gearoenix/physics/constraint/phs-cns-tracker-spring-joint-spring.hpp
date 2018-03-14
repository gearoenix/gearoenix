#ifndef GEAROENIX_PHYSICS_CONSTRAINT_TRACKER_SPRING_JOINT_SPRING_HPP
#define GEAROENIX_PHYSICS_CONSTRAINT_TRACKER_SPRING_JOINT_SPRING_HPP

#include "../../core/cr-end-caller.hpp"
#include "../../core/cr-types.hpp"
#include "../../math/math-vector.hpp"
#include "phs-cns-constraint.hpp"
#include <memory>

namespace gearoenix {
namespace physics {
    namespace body {
        class Rigid;
    }
    namespace constraint {
        class TrackerSpringJointSpring : public Constraint {
        private:
            std::weak_ptr<body::Rigid> active;
            std::weak_ptr<body::Rigid> passive;
            core::Real k;
            core::Real length;
            core::Real active_joint_k;
            core::Real passive_joint_k;

        public:
            TrackerSpringJointSpring(system::stream::Stream* f, render::Engine* render_engine, core::EndCaller<core::EndCallerIgnore> c);
            virtual ~TrackerSpringJointSpring();
            virtual void apply();
        };
    }
}
}
#endif
