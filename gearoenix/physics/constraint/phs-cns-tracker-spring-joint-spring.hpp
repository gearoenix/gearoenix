#ifndef GEAROENIX_PHYSICS_CONSTRAINT_TRACKER_SPRING_JOINT_SPRING_HPP
#define GEAROENIX_PHYSICS_CONSTRAINT_TRACKER_SPRING_JOINT_SPRING_HPP

#include "../../core/cr-types.hpp"
#include "../../core/sync/cr-sync-end-caller.hpp"
#include "../../math/math-vector-3d.hpp"
#include "phs-cns-constraint.hpp"
#include <memory>

namespace gearoenix::physics {
namespace body {
    class Rigid;
}
namespace constraint {
    class TrackerSpringJointSpring : public Constraint {
    private:
        std::shared_ptr<body::Rigid> active;
        std::shared_ptr<body::Rigid> passive;
        double k = 0.0;
        math::Vec3<double> angle;
        double joint_k = 0.0;
        double length = 0.0;

    public:
        TrackerSpringJointSpring(
            core::Id my_id,
            system::stream::Stream* f,
            render::engine::Engine* e,
            const core::sync::EndCaller<core::sync::EndCallerIgnore>& c);
        TrackerSpringJointSpring(
            core::Id my_id,
            std::shared_ptr<body::Rigid> active,
            std::shared_ptr<body::Rigid> passive,
            double k,
            const math::Vec3<double>& angle,
            double joint_k,
            double length);
        ~TrackerSpringJointSpring() override;
        void on_event(const core::event::Event& e) override;
        [[nodiscard]] std::vector<std::pair<core::Id, std::shared_ptr<render::model::Dynamic>>> get_all_models() const override;
        [[nodiscard]] std::vector<std::shared_ptr<body::Body>> get_all_bodies() const override;
        void apply(double delta_time) noexcept final;
    };
}
}
#endif
