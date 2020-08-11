#include "gx-phs-bd-car-smooth-road.hpp"
#include "../../core/asset/cr-asset-manager.hpp"
#include "../../render/model/rnd-mdl-dynamic.hpp"
#include "../gx-phs-constants.hpp"

void gearoenix::physics::body::Car::update_steering_angle() noexcept
{
    if (steering_angle > steering_angle_max)
        steering_angle = steering_angle_max;
    else if (steering_angle < steering_angle_min)
        steering_angle = steering_angle_min;
}

void gearoenix::physics::body::Car::update_axles() noexcept
{
    front_slip_angle = (lateral_velocity + (wheels_angular_velocity * front_axle_center_of_gravity_distance)).atan() - (velocity.sign() * steering_angle);
    rear_slip_angle = (lateral_velocity - (wheels_angular_velocity * rear_axle_center_of_gravity_distance)).atan();
    front_lateral_force = front_slip_angle * cornering_stiffness;
    rear_lateral_force = rear_slip_angle * cornering_stiffness;
    // TODO: compute & check that all tires all on ground
    front_friction_force = mass * gravitational_acceleration * (is_front_sliding ? tire_kinetic_friction_coefficient : tire_static_friction_coefficient) * 0.5;
    front_capped_lateral_force = front_lateral_force - front_friction_force;
    rear_friction_force = mass * gravitational_acceleration * (is_rear_sliding ? tire_kinetic_friction_coefficient : tire_static_friction_coefficient) * 0.5;
    rear_capped_lateral_force = rear_lateral_force - rear_friction_force;
}

void gearoenix::physics::body::Car::update_engine() noexcept
{
    velocity_attitude = velocity.length();
    wheels_angular_velocity = velocity_attitude / (2.0 * GX_PI * tire_radius);
    engine_angular_velocity = wheels_angular_velocity * gear_ratios[current_gear] * final_drive_ratio;
    engine_angular_velocity_rpm = engine_angular_velocity * 60.0;
    // check for transmission change base on engine_angular_velocity_rpm
    //
    std::size_t torque_index = 0;
#ifdef GX_DEBUG_MODE
    if (engine_angular_velocity_rpm > torque_rpm_diagram_points.back().first || engine_angular_velocity_rpm < 0.0) {
        GXLOGF("Unexpected engine_angular_velocity_rpm (" << engine_angular_velocity_rpm << ") value.")
    }
#endif
    while (engine_angular_velocity_rpm > torque_rpm_diagram_points[torque_index++].first)
        ;
    engine_torque = torque_rpm_diagram_points[torque_index].second(engine_angular_velocity_rpm);
}

void gearoenix::physics::body::Car::update() noexcept
{
    update_steering_angle();
    update_axles();
    update_engine();
}

gearoenix::physics::body::Car::Car(
    std::string name,
    std::shared_ptr<render::model::Dynamic> car_body,
    std::shared_ptr<render::model::Dynamic> car_left_front_wheel,
    std::shared_ptr<render::model::Dynamic> car_right_front_wheel,
    std::shared_ptr<render::model::Dynamic> car_left_back_wheel,
    std::shared_ptr<render::model::Dynamic> car_right_back_wheel) noexcept
    : Body(core::asset::Manager::create_id(), std::move(name), Type::SmoothRoadCar, car_body)
    , car_body(std::move(car_body))
    , car_left_front_wheel(std::move(car_left_front_wheel))
    , car_right_front_wheel(std::move(car_right_front_wheel))
    , car_left_back_wheel(std::move(car_left_back_wheel))
    , car_right_back_wheel(std::move(car_right_back_wheel))
{
}

gearoenix::physics::body::Car::~Car() noexcept = default;
