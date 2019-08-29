#ifndef GEAROENIX_PHYSICS_CONSTRAINT_TRACKER_SPRING_JOINT_SPRING_HPP
#define GEAROENIX_PHYSICS_CONSTRAINT_TRACKER_SPRING_JOINT_SPRING_HPP

#include "../../core/cr-types.hpp"
#include "../../core/sync/cr-sync-end-caller.hpp"
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
            std::shared_ptr<body::Rigid> active;
            std::shared_ptr<body::Rigid> passive;
            core::Real k;
            math::Vec3 angle;
            core::Real joint_k;
            core::Real length;

        public:
            TrackerSpringJointSpring(
                const core::Id my_id,
                system::stream::Stream* f,
                render::engine::Engine* e,
                const core::sync::EndCaller<core::sync::EndCallerIgnore> c);
            TrackerSpringJointSpring(
                const core::Id my_id,
                const std::shared_ptr<body::Rigid>& active,
                const std::shared_ptr<body::Rigid>& passive,
                const core::Real k,
                const math::Vec3& angle,
                const core::Real joint_k,
                const core::Real length);
            ~TrackerSpringJointSpring();
            void on_event(const core::event::Event& e);
            const std::vector<std::pair<core::Id, std::shared_ptr<render::model::Dynamic>>> get_all_models() const;
            const std::vector<std::shared_ptr<body::Body>> get_all_bodies() const;
            void apply(core::Real delta_time) noexcept final;
        };
    }
}
}
#endif
