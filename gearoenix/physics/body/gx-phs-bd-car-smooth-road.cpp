#include "gx-phs-bd-car-smooth-road.hpp"
#include "../../core/asset/cr-asset-manager.hpp"
#include "../../render/model/rnd-mdl-dynamic.hpp"

void gearoenix::physics::body::SmoothRoadCar::update() noexcept
{
    if (steering_angle > steering_angle_max)
        steering_angle = steering_angle_max;
    else if (steering_angle < steering_angle_min)
        steering_angle = steering_angle_min;
}

gearoenix::physics::body::SmoothRoadCar::SmoothRoadCar(
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

gearoenix::physics::body::SmoothRoadCar::~SmoothRoadCar() noexcept = default;
