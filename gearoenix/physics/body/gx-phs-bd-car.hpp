#ifndef GEAROENIX_PHYSICS_BODY_CAR_HPP
#define GEAROENIX_PHYSICS_BODY_CAR_HPP
#include "../../math/math-vector-3d.hpp"
#include "gx-phs-bd-body.hpp"
#include <array>
#include <functional>
#include <memory>
#include <utility>
#include <vector>

namespace gearoenix::render::model {
class Dynamic;
}

namespace gearoenix::physics::body {
class Car : public Body {
public:
    struct Wheel {
        /// The amount of weight on the wheel
        double load;
        /// Force_rr = -C_rr * Velocity
        math::Vec3<double> rolling_resistance_force;
        bool is_sliding = false;
        bool is_on_surface = true;
        std::shared_ptr<render::model::Dynamic> model;
        /// It is based on the surface and tire-to-surface-stickness-factor
        double static_friction_coefficient = 1.0;
        double kinetic_friction_coefficient = 1.0;
    };

    struct Axle {
        Wheel left_wheel;
        Wheel right_wheel;
        double center_of_gravity_distance = 1.0;
        double spring_k = 1.0;
        double spring_max_length = 1.0;
        double spring_min_length = 1.0;
        double tire_radius = 0.3;
        double wheels_angular_velocity = 0.0;
        math::Vec3<double> slip_angle;
        math::Vec3<double> lateral_force;
        math::Vec3<double> capped_lateral_force;
        bool is_active = true;
    };
    /// sorted (starting engine_rpm, engine_torque_function)
    using TorqueRpmDiagramPoints = std::array<std::pair<double, std::function<double(double)>>, 4>;
    /// It depends of tier (tunnable)
    GX_GETSET_VAL_PRV(double, rolling_resistance_factor, 13.0)
    /// It depends of tier (tunnable)
    GX_GETSET_VAL_PRV(double, tire_to_surface_stickness_factor, 1.0)
    /// Player input
    GX_GETSET_VAL_PRV(double, acceleration_pedal_percentage, 0.0)
    /// Player input
    GX_GETSET_VAL_PRV(double, break_pedal_percentage, 0.0)
    GX_GETSET_VAL_PRV(double, break_factor, 1.0)
    /// Player input
    GX_GETSET_VAL_PRV(double, steering_angle, 0.0)
    /// Player input or it can be set by automatic transmission system
    GX_GETSET_VAL_PRV(std::size_t, current_gear, 0)
    /// Car settings (tunable)
    GX_GETSET_VAL_PRV(double, steering_angle_max, 0.6)
    /// Car settings (tunable)
    GX_GETSET_VAL_PRV(double, mass, 1000.0)
    // It is based on the aerodynamism of the car (tunnable)
    GX_GETSET_VAL_PRV(double, aerodynamic_drag_factor, 0.5)
    GX_GETSET_VAL_PRV(double, cornering_stiffness, 1.0)
    /// Car settings (tunable)
    GX_GETSET_VAL_PRV(double, engine_angular_velocity_max_rpm, 0.0)
    /// Car settings (tunable)
    GX_GET_REF_PRV(TorqueRpmDiagramPoints, torque_rpm_diagram_points)
    /// Car settings (tunable)
    GX_GET_REF_PRV(std::vector<double>, gear_ratios)
    /// Car settings (tunable)
    GX_GETSET_VAL_PRV(double, final_drive_ratio, 0.0)
    /// F_drag = C_drag * V * V
    GX_GET_VAL_PRV(double, aerodynamic_drag_force, 0.0)
    /// F_g_pull = m * g * sin(θ slope angle)
    GX_GET_VAL_PRV(double, gravity_pull_force, 0.0)
    /// F_g_ground = m * g * cos(θ slope angle)
    GX_GET_VAL_PRV(double, gravity_ground_force, 0.0)
    GX_GET_CREF_PRV(math::Vec3<double>, velocity)
    GX_GET_VAL_PRV(double, velocity_attitude, 0.0)
    GX_GET_CREF_PRV(math::Vec3<double>, lateral_velocity)
    /// F_traction = Torque_wheel / Radius_wheel
    GX_GET_CREF_PRV(math::Vec3<double>, traction_force)
    /// F long = F traction - F brake - F drag - F rr +/- F g
    GX_GET_CREF_PRV(math::Vec3<double>, long_force)
    /// ω e = 2π Ω e (rpm) / 60
    GX_GET_VAL_PRV(double, engine_angular_velocity, 0.0)
    GX_GET_VAL_PRV(double, engine_angular_velocity_rpm, 0.0)
    GX_GET_VAL_PRV(double, engine_torque, 0.0)
    /// T_wheels = T_engine * gear_ratio * G (final drive ratio)
    GX_GET_VAL_PRV(double, wheels_torque, 0.0)
    GX_GET_REFC_PRV(std::shared_ptr<render::model::Dynamic>, car_body)
    GX_GET_REF_PRV(Axle, front_axle)
    GX_GET_REF_PRV(Axle, rear_axle)
private:
    void update_steering_angle() noexcept;
    void update_axles() noexcept;
    void update_engine() noexcept;
    void update() noexcept;

public:
    Car(
        std::string name,
        std::shared_ptr<render::model::Dynamic> car_body,
        std::shared_ptr<render::model::Dynamic> car_left_front_wheel,
        std::shared_ptr<render::model::Dynamic> car_right_front_wheel,
        std::shared_ptr<render::model::Dynamic> car_left_rear_wheel,
        std::shared_ptr<render::model::Dynamic> car_right_rear_wheel) noexcept;
    ~Car() noexcept override;
};
}
#endif
