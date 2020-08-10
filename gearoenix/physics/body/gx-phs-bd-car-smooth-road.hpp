#ifndef GEAROENIX_PHYSICS_BODY_CAR_SMOOTH_ROAD_HPP
#define GEAROENIX_PHYSICS_BODY_CAR_SMOOTH_ROAD_HPP
#include "../../math/math-vector-3d.hpp"
#include "gx-phs-bd-body.hpp"
#include <vector>

namespace gearoenix::render::model {
class Dynamic;
}
namespace gearoenix::physics::body {
class SmoothRoadCar : public Body {
public:
    using TorqueRpmDiagramPoints = std::array<std::pair<double, double>, 4>;

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
    /// F_traction = Torque_wheel / Radius_wheel
    GX_GET_VAL_PRV(double, traction_force, 0.0)
    /// F long = F traction - F brake - F drag - F rr +/- F g
    GX_GET_VAL_PRV(double, long_force, 0.0)
    /// ω e = 2π Ω e (rpm) / 60
    GX_GET_VAL_PRV(double, engine_angular_velocity, 0.0)
    GX_GET_REF_PRV(TorqueRpmDiagramPoints, torque_rpm_diagram_points)
    /// T_wheels = T_engine * gear_ratio * G (final drive ratio)
    GX_GET_VAL_PRV(double, wheels_torque, 0.0)
    GX_GET_REF_PRV(std::vector<double>, gear_ratios)
private:
    void update() noexcept;

public:
    SmoothRoadCar(
        std::string name,
        std::shared_ptr<render::model::Dynamic> car_body,
        std::shared_ptr<render::model::Dynamic> car_left_front_wheel,
        std::shared_ptr<render::model::Dynamic> car_right_front_wheel,
        std::shared_ptr<render::model::Dynamic> car_left_back_wheel,
        std::shared_ptr<render::model::Dynamic> car_right_back_wheel) noexcept;
    ~SmoothRoadCar() noexcept override;
};
}
#endif
