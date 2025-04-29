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
//     math::Vec3<double> speed;
//     math::Vec3<double> force;
//     double mass = 1.0;
//     double inverted_mass = 1.0;
//     double air_friction = -0.5; // its nature is to be a negative number
//     double rotational_speed = 0.0;
//     math::Vec3<double> rotational_speed_axis;
//     double rotational_acceleration = 0.0;
//     math::Vec3<double> rotational_acceleration_axis;
//     double rotational_latency = 0.5; // its nature is to be a number less than 1
//     double rotational_air_friction = 0.0; // its nature is to be a negative number
//     double interaction_friction = 0.0; // its nature is to be a negative number
//     double interaction_bounciness = 1.0; // its nature is to be a number less than 1
//     Behavior rigid_body_behavior = Behavior::Active;
//
// public:
//     Rigid(core::Id id, std::string name, std::shared_ptr<render::model::Dynamic> model);
//     ~Rigid() override;
//     void apply_force_on_origin(const math::Vec3<double>& force);
//     void apply_force(const math::Vec3<double>& force, const math::Vec3<double>& point);
//     void apply_time(double delta_time);
// };
// }