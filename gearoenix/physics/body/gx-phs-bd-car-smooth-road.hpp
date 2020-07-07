#ifndef GEAROENIX_PHYSICS_BODY_CAR_SMOOTH_ROAD_HPP
#define GEAROENIX_PHYSICS_BODY_CAR_SMOOTH_ROAD_HPP
#include "gx-phs-bd-body.hpp"
#include <vector>

namespace gearoenix::render::model {
class Dynamic;
}

namespace gearoenix::physics::body {
class SmoothRoadCar : public Body {
    GX_GET_REFC_PRV(std::shared_ptr<render::model::Dynamic>, car_body)
    GX_GET_REFC_PRV(std::shared_ptr<render::model::Dynamic>, car_left_front_wheel)
    GX_GET_REFC_PRV(std::shared_ptr<render::model::Dynamic>, car_right_front_wheel)
    GX_GET_REFC_PRV(std::shared_ptr<render::model::Dynamic>, car_left_back_wheel)
    GX_GET_REFC_PRV(std::shared_ptr<render::model::Dynamic>, car_right_back_wheel)
    GX_GETSET_VAL_PRV(double, max_speed, 0.0)
    GX_GETSET_VAL_PRV(double, max_reverse_speed, 0.0)
    GX_GETSET_VAL_PRV(double, acceleration, 0.0)
    GX_GETSET_VAL_PRV(double, break_power, 1.0)
    GX_GETSET_VAL_PRV(double, break_factor, 0.0)
    GX_GETSET_VAL_PRV(double, handel_divergence, 0.0)
    GX_GET_VAL_PRV(double, speed, 1.0)
    GX_GET_REF_PRV(std::vector<double>, gear_threshold)
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
