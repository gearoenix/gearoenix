#ifndef GEAROENIX_PHYSICS_BODY_RIGID_HPP
#define GEAROENIX_PHYSICS_BODY_RIGID_HPP
#include "../../math/math-vector.hpp"
#include "phs-bd-body.hpp"
namespace gearoenix {
namespace physics {
    namespace body {
        class Rigid : public Body {
        private:
        protected:
            math::Vec3 speed;
            math::Vec3 acceleration;
            math::Vec3 rotational_speed;
            math::Vec3 rotational_acceleration;
            core::Real mass;
            core::Real air_friction;
            core::Real friction;
            core::Real bounciness;
            bool active = false;

        public:
            Rigid(const std::shared_ptr<Transferable>& body_obj);
            virtual ~Rigid();
            void apply_force_on_origin(const math::Vec3& force);
            void apply_force(const math::Vec3& force, const math::Vec3& point);
        };
    }
}
}
#endif
