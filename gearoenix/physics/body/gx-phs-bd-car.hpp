// #pragma once
// #include "../../math/gx-math-vector-3d.hpp"
// #include "gx-phs-bd-body.hpp"
// #include <array>
// #include <functional>
// #include <memory>
// #include <utility>
// #include <vector>
//
// namespace gearoenix::render::model {
// struct Dynamic;
// }
//
// namespace gearoenix::physics::body {
// struct Car : public Body {
// public:
//     struct Wheel {
//         /// The amount of weight on the wheel
//         core::fp_t load;
//         /// Force_rr = -C_rr * Velocity
//         math::Vec3<core::fp_t> rolling_resistance_force;
//         bool is_sliding = false;
//         bool is_on_surface = true;
//         std::shared_ptr<render::model::Dynamic> model;
//         /// It is based on the surface and tire-to-surface-stickness-factor
//         core::fp_t static_friction_coefficient = 1.0;
//         core::fp_t kinetic_friction_coefficient = 1.0;
//     };
//
//     struct Axle {
//         Wheel left_wheel;
//         Wheel right_wheel;
//         core::fp_t center_of_gravity_distance = 1.0;
//         core::fp_t spring_k = 1.0;
//         core::fp_t spring_max_length = 1.0;
//         core::fp_t spring_min_length = 1.0;
//         core::fp_t tire_radius = 0.3;
//         core::fp_t wheels_angular_velocity = 0.0;
//         math::Vec3<core::fp_t> slip_angle;
//         math::Vec3<core::fp_t> lateral_force;
//         math::Vec3<core::fp_t> capped_lateral_force;
//         bool is_active = true;
//     };
//     /// sorted (starting engine_rpm, engine_torque_function)
//     using TorqueRpmDiagramPoints = std::array<std::pair<core::fp_t, std::function<core::fp_t(core::fp_t)>>, 4>;
//     /// It depends of tier (tunnable)
//     GX_GETSET_VAL_PRV(core::fp_t, rolling_resistance_factor, 13.0)
//     /// It depends of tier (tunnable)
//     GX_GETSET_VAL_PRV(core::fp_t, tire_to_surface_stickness_factor, 1.0)
//     /// Player input
//     GX_GETSET_VAL_PRV(core::fp_t, acceleration_pedal_percentage, 0.0)
//     /// Player input
//     GX_GETSET_VAL_PRV(core::fp_t, break_pedal_percentage, 0.0)
//     GX_GETSET_VAL_PRV(core::fp_t, break_factor, 1.0)
//     /// Player input
//     GX_GETSET_VAL_PRV(core::fp_t, steering_angle, 0.0)
//     /// Player input or it can be set by automatic transmission system
//     GX_GETSET_VAL_PRV(std::uint32_t, current_gear, 0)
//     /// Car settings (tunable)
//     GX_GETSET_VAL_PRV(core::fp_t, steering_angle_max, 0.6)
//     /// Car settings (tunable)
//     GX_GETSET_VAL_PRV(core::fp_t, mass, 1000.0)
//     // It is based on the aerodynamism of the car (tunnable)
//     GX_GETSET_VAL_PRV(core::fp_t, aerodynamic_drag_factor, 0.5)
//     GX_GETSET_VAL_PRV(core::fp_t, cornering_stiffness, 1.0)
//     /// Car settings (tunable)
//     GX_GETSET_VAL_PRV(core::fp_t, engine_angular_velocity_max_rpm, 0.0)
//     /// Car settings (tunable)
//     GX_GET_REF_PRV(TorqueRpmDiagramPoints, torque_rpm_diagram_points)
//     /// Car settings (tunable)
//     GX_GET_REF_PRV(std::vector<core::fp_t>, gear_ratios)
//     /// Car settings (tunable)
//     GX_GETSET_VAL_PRV(core::fp_t, final_drive_ratio, 0.0)
//     /// F_drag = C_drag * V * V
//     GX_GET_VAL_PRV(core::fp_t, aerodynamic_drag_force, 0.0)
//     /// F_g_pull = m * g * sin(θ slope angle)
//     GX_GET_VAL_PRV(core::fp_t, gravity_pull_force, 0.0)
//     /// F_g_ground = m * g * cos(θ slope angle)
//     GX_GET_VAL_PRV(core::fp_t, gravity_ground_force, 0.0)
//     GX_GET_CREF_PRV(math::Vec3<core::fp_t>, velocity)
//     GX_GET_VAL_PRV(core::fp_t, velocity_attitude, 0.0)
//     GX_GET_CREF_PRV(math::Vec3<core::fp_t>, lateral_velocity)
//     /// F_traction = Torque_wheel / Radius_wheel
//     GX_GET_CREF_PRV(math::Vec3<core::fp_t>, traction_force)
//     /// F long = F traction - F brake - F drag - F rr +/- F g
//     GX_GET_CREF_PRV(math::Vec3<core::fp_t>, long_force)
//     /// ω e = 2π Ω e (rpm) / 60
//     GX_GET_VAL_PRV(core::fp_t, engine_angular_velocity, 0.0)
//     GX_GET_VAL_PRV(core::fp_t, engine_angular_velocity_rpm, 0.0)
//     GX_GET_VAL_PRV(core::fp_t, engine_torque, 0.0)
//     /// T_wheels = T_engine * gear_ratio * G (final drive ratio)
//     GX_GET_VAL_PRV(core::fp_t, wheels_torque, 0.0)
//     GX_GET_REF_PRV(Axle, front_axle)
//     GX_GET_REF_PRV(Axle, rear_axle)
// private:
//     void update_steering_angle();
//     void update_axles();
//     void update_engine();
//     void update();
//
// public:
//     Car(
//         std::string name,
//         std::shared_ptr<render::model::Dynamic> car_body,
//         std::shared_ptr<render::model::Dynamic> car_left_front_wheel,
//         std::shared_ptr<render::model::Dynamic> car_right_front_wheel,
//         std::shared_ptr<render::model::Dynamic> car_left_rear_wheel,
//         std::shared_ptr<render::model::Dynamic> car_right_rear_wheel);
//     ~Car() override;
// };
// }