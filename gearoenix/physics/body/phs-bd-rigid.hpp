#ifndef GEAROENIX_PHYSICS_BODY_RIGID_HPP
#define GEAROENIX_PHYSICS_BODY_RIGID_HPP
#include "../../math/math-vector.hpp"
#include "phs-bd-body.hpp"
namespace gearoenix {
namespace physics {
    namespace body {
        class Rigid : public Body {
		public:
			class RigidBodyBehavior {
			public:
				typedef enum {
					ACTIVE,
					PASSIVE,
				} Type;
			};
        private:
        protected:
            math::Vec3 speed;
            math::Vec3 force;
            core::Real mass = 1.0f;
			core::Real inversed_mass = 1.0f;
            core::Real air_friction = -0.5f; // its nature is to be a negative number
            core::Real rotational_speed;
			math::Vec3 rotational_speed_axis;
			core::Real rotational_acceleration;
			math::Vec3 rotational_acceleration_axis;
			core::Real rotational_latency = 0.5f; // its nature is to be a number less than 1
			core::Real rotational_air_friction = 0.0f;  // its nature is to be a negative number
            core::Real interaction_friction = 0.0f; // its nature is to be a negative number
            core::Real interaction_bounciness = 1.0f; // its nature is to be a number less than 1
            RigidBodyBehavior::Type rigid_body_behavior = RigidBodyBehavior::ACTIVE;

        public:
            Rigid(const core::Id my_id, const std::shared_ptr<Transferable>& body_obj);
            virtual ~Rigid();
            void apply_force_on_origin(const math::Vec3& force);
            void apply_force(const math::Vec3& force, const math::Vec3& point);
			void apply_time(const core::Real delta_time);
        };
    }
}
}
#endif
