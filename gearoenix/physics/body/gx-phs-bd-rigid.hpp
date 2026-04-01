// #pragma once
// #include "../../math/gx-math-vector-3d.hpp"
// #include "gx-phs-bd-body.hpp"
//
// namespace gearoenix::physics::body {
// struct Rigid : public Body {
// public:
//     enum struct Behavior {
//         Active = 1,
//         Passive = 2,
//     };
//
// private:
// protected:
//     math::Vec3<core::fp_t> speed;
//     math::Vec3<core::fp_t> force;
//     core::fp_t mass = 1.0;
//     core::fp_t inverted_mass = 1.0;
//     core::fp_t air_friction = -0.5; // its nature is to be a negative number
//     core::fp_t rotational_speed = 0.0;
//     math::Vec3<core::fp_t> rotational_speed_axis;
//     core::fp_t rotational_acceleration = 0.0;
//     math::Vec3<core::fp_t> rotational_acceleration_axis;
//     core::fp_t rotational_latency = 0.5; // its nature is to be a number less than 1
//     core::fp_t rotational_air_friction = 0.0; // its nature is to be a negative number
//     core::fp_t interaction_friction = 0.0; // its nature is to be a negative number
//     core::fp_t interaction_bounciness = 1.0; // its nature is to be a number less than 1
//     Behavior rigid_body_behavior = Behavior::Active;
//
// public:
//     Rigid(core::Id id, std::string name, std::shared_ptr<render::model::Dynamic> model);
//     ~Rigid() override;
//     void apply_force_on_origin(const math::Vec3<core::fp_t>& force);
//     void apply_force(const math::Vec3<core::fp_t>& force, const math::Vec3<core::fp_t>& point);
//     void apply_time(core::fp_t delta_time);
// };
// }