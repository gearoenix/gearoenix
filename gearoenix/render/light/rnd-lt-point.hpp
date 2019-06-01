#ifndef GEAROENIX_RENDER_LIGHT_POINT_HPP
#define GEAROENIX_RENDER_LIGHT_POINT_HPP
#include "../../core/sync/cr-sync-end-caller.hpp"
#include "../../math/math-vector.hpp"
#include "rnd-lt-light.hpp"
namespace gearoenix::render::light {
struct PointUniform {
    math::Vec4 color_min_radius;
    math::Vec4 position_max_radius;
    math::Vec4 vps[6];
    math::Vec4 vpbs[6];
};
class Point : public Light {
private:
    math::Vec4 position_radius = math::Vec4(1.0, 1.0, 1.0, 1.0);

public:
    Point(core::Id my_id, system::stream::Stream* f, engine::Engine* e) noexcept;
    const math::Vec4& get_position_radius() const noexcept;
};
}
#endif
