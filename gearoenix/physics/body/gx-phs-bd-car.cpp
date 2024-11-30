#include "gx-phs-bd-car.hpp"
#include "../../core/asset/gx-cr-asset-manager.hpp"
#include "../../render/model/gx-rnd-mdl-dynamic.hpp"
#include "../gx-phs-constants.hpp"

void gearoenix::physics::body::Car::update_steering_angle()
{
    if (steering_angle > steering_angle_max) {
        steering_angle = steering_angle_max;
        return;
    }
    const auto steering_angle_min = -steering_angle_max;
    if (steering_angle < steering_angle_min)
        steering_angle = steering_angle_min;
}

void gearoenix::physics::body::Car::update_axles()
{
    // front_slip_angle = (lateral_velocity + (wheels_angular_velocity * front_axle_center_of_gravity_distance)).atan() - (velocity.sign() * steering_angle);
    // rear_slip_angle = (lateral_velocity - (wheels_angular_velocity * rear_axle_center_of_gravity_distance)).atan();
    // front_lateral_force = front_slip_angle * cornering_stiffness;
    // rear_lateral_force = rear_slip_angle * cornering_stiffness;
    //// TODO: compute & check that all tires all on ground
    // front_friction_force = mass * gravitational_acceleration * (is_front_sliding ? tire_kinetic_friction_coefficient : tire_static_friction_coefficient) * 0.5;
    // front_capped_lateral_force = front_lateral_force - front_friction_force;
    // rear_friction_force = mass * gravitational_acceleration * (is_rear_sliding ? tire_kinetic_friction_coefficient : tire_static_friction_coefficient) * 0.5;
    // rear_capped_lateral_force = rear_lateral_force - rear_friction_force;
}

void gearoenix::physics::body::Car::update_engine()
{
    velocity_attitude = velocity.length();
    // wheels_angular_velocity = velocity_attitude / (2.0 * std::numbers::pi * tire_radius);
    // engine_angular_velocity = wheels_angular_velocity * gear_ratios[current_gear] * final_drive_ratio;
    engine_angular_velocity_rpm = engine_angular_velocity * 60.0;
    // check for transmission change base on engine_angular_velocity_rpm
    //
    std::uint32_t torque_index = 0;
#ifdef GX_DEBUG_MODE
    if (engine_angular_velocity_rpm > torque_rpm_diagram_points.back().first || engine_angular_velocity_rpm < 0.0) {
        GXLOGF("Unexpected engine_angular_velocity_rpm (" << engine_angular_velocity_rpm << ") value.")
    }
#endif
    while (engine_angular_velocity_rpm > torque_rpm_diagram_points[torque_index++].first)
        ;
    engine_torque = torque_rpm_diagram_points[torque_index].second(engine_angular_velocity_rpm);
}

void gearoenix::physics::body::Car::update()
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
    std::shared_ptr<render::model::Dynamic> car_left_rear_wheel,
    std::shared_ptr<render::model::Dynamic> car_right_rear_wheel)
    : Body(core::asset::Manager::create_id(), std::move(name), Type::Car, std::move(car_body))
    , front_axle {
        .left_wheel { .model = std::move(car_left_front_wheel) },
        .right_wheel { .model = std::move(car_right_front_wheel) },
    }
    , rear_axle {
        .left_wheel { .model = std::move(car_left_rear_wheel) },
        .right_wheel { .model = std::move(car_right_rear_wheel) },
    }
{
    std::vector<core::Id> influence_ids {
        model->get_id(),
        front_axle.left_wheel.model->get_id(),
        front_axle.right_wheel.model->get_id(),
        rear_axle.left_wheel.model->get_id(),
        rear_axle.right_wheel.model->get_id(),
    };
}

gearoenix::physics::body::Car::~Car() = default;
