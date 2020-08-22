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
        /// The
        double load;
    };
    /// sorted (starting engine_rpm, engine_torque_function)
    using TorqueRpmDiagramPoints = std::array<std::pair<double, std::function<double(double)>>, 4>;

    GX_GET_REFC_PRV(std::shared_ptr<render::model::Dynamic>, car_body)
    GX_GET_REFC_PRV(std::shared_ptr<render::model::Dynamic>, car_left_front_wheel)
    GX_GET_REFC_PRV(std::shared_ptr<render::model::Dynamic>, car_right_front_wheel)
    GX_GET_REFC_PRV(std::shared_ptr<render::model::Dynamic>, car_left_back_wheel)
    GX_GET_REFC_PRV(std::shared_ptr<render::model::Dynamic>, car_right_back_wheel)
    GX_GETSET_VAL_PRV(double, max_velocity, 150.0)
    GX_GETSET_VAL_PRV(double, max_reverse_velocity, 20.0)
    GX_GETSET_VAL_PRV(double, acceleration_pedal_percentage, 0.0)
    GX_GETSET_VAL_PRV(double, break_pedal_percentage, 0.0)
    GX_GETSET_VAL_PRV(double, break_factor, 1.0)
    GX_GETSET_VAL_PRV(double, steering_angle, 0.0)
    GX_GETSET_VAL_PRV(double, steering_angle_max, 0.6)
    GX_GETSET_VAL_PRV(double, steering_angle_min, -0.6)
    GX_GETSET_VAL_PRV(double, mass, 1000.0)
    GX_GETSET_VAL_PRV(double, back_spring_k, 1.0)
    GX_GETSET_VAL_PRV(double, back_spring_max_length, 1.0)
    GX_GETSET_VAL_PRV(double, back_spring_min_length, 1.0)
    GX_GETSET_VAL_PRV(double, front_spring_k, 1.0)
    GX_GETSET_VAL_PRV(double, front_spring_max_length, 1.0)
    GX_GETSET_VAL_PRV(double, front_spring_min_length, 1.0)
    GX_GETSET_VAL_PRV(double, aerodynamic_drag_factor, 1.0)
    GX_GETSET_VAL_PRV(double, front_axle_center_of_gravity_distance, 1.0)
    GX_GETSET_VAL_PRV(double, rear_axle_center_of_gravity_distance, 1.0)
    GX_GETSET_VAL_PRV(double, cornering_stiffness, 1.0)
    GX_GETSET_VAL_PRV(double, tire_static_friction_coefficient, 1.0)
    GX_GETSET_VAL_PRV(double, tire_kinetic_friction_coefficient, 1.0)
    GX_GETSET_VAL_PRV(double, tire_radius, 0.3)
    /// F_drag = C_drag * V * V
    GX_GET_VAL_PRV(double, aerodynamic_drag_force, 0.0)
    GX_GETSET_VAL_PRV(double, rolling_resistance_factor, 1.0)
    /// F_rr = C_rr * V
    GX_GET_VAL_PRV(double, rolling_resistance_force, 1.0)
    /// F_g_pull = m * g * sin(θ slope angle)
    GX_GET_VAL_PRV(double, gravity_pull_force, 0.0)
    /// F_g_ground = m * g * cos(θ slope angle)
    GX_GET_VAL_PRV(double, gravity_ground_force, 0.0)
    GX_GET_CREF_PRV(math::Vec3<double>, velocity)
    GX_GET_VAL_PRV(double, velocity_attitude, 0.0)
    GX_GET_CREF_PRV(math::Vec3<double>, lateral_velocity)
    GX_GET_CREF_PRV(math::Vec3<double>, front_slip_angle)
    GX_GET_CREF_PRV(math::Vec3<double>, rear_slip_angle)
    /// F_traction = Torque_wheel / Radius_wheel
    GX_GET_CREF_PRV(math::Vec3<double>, traction_force)
    /// F long = F traction - F brake - F drag - F rr +/- F g
    GX_GET_CREF_PRV(math::Vec3<double>, long_force)
    GX_GET_CREF_PRV(math::Vec3<double>, front_lateral_force)
    GX_GET_CREF_PRV(math::Vec3<double>, rear_lateral_force)
    GX_GET_VAL_PRV(double, front_friction_force, 1000.0)
    GX_GET_VAL_PRV(double, rear_friction_force, 1000.0)
    GX_GET_CREF_PRV(math::Vec3<double>, front_capped_lateral_force)
    GX_GET_CREF_PRV(math::Vec3<double>, rear_capped_lateral_force)
    GX_GETSET_VAL_PRV(double, engine_angular_velocity_max_rpm, 0.0)
    /// ω e = 2π Ω e (rpm) / 60
    GX_GET_VAL_PRV(double, engine_angular_velocity, 0.0)
    GX_GET_VAL_PRV(double, engine_angular_velocity_rpm, 0.0)
    GX_GET_VAL_PRV(double, wheels_angular_velocity, 0.0)
    GX_GET_REF_PRV(TorqueRpmDiagramPoints, torque_rpm_diagram_points)
    GX_GET_VAL_PRV(double, engine_torque, 0.0)
    /// T_wheels = T_engine * gear_ratio * G (final drive ratio)
    GX_GET_VAL_PRV(double, wheels_torque, 0.0)
    GX_GET_VAL_PRV(bool, is_sliding, false)
    GX_GET_VAL_PRV(bool, is_front_sliding, false)
    GX_GET_VAL_PRV(bool, is_rear_sliding, false)

    GX_GET_REF_PRV(std::vector<double>, gear_ratios)
    GX_GETSET_VAL_PRV(std::size_t, current_gear, 0)
    GX_GETSET_VAL_PRV(double, final_drive_ratio, 0.0)
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
        std::shared_ptr<render::model::Dynamic> car_left_back_wheel,
        std::shared_ptr<render::model::Dynamic> car_right_back_wheel) noexcept;
    ~Car() noexcept override;
};
}
#endif
