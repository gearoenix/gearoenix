#ifndef GEAROENIX_RENDER_LIGHT_POINT_HPP
#define GEAROENIX_RENDER_LIGHT_POINT_HPP
#include "../../core/ecs/gx-cr-ecs-component.hpp"
#include "../../math/gx-math-vector-3d.hpp"

namespace gearoenix::render::light {
struct Point final : public core::ecs::Component {
    math::Vec3<float> position = { 0.0f, 0.0f, 0.0f };

    Point() noexcept;
    Point(Point&&) noexcept;
    ~Point() noexcept;
};
}
#endif
