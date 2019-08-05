#ifndef GEAROENIX_RENDER_LIGHT_POINT_HPP
#define GEAROENIX_RENDER_LIGHT_POINT_HPP
#include "../../core/sync/cr-sync-end-caller.hpp"
#include "../../math/math-vector.hpp"
#include "rnd-lt-light.hpp"
namespace gearoenix::render::light {
class Point : public Light {
private:
    core::Real min_radius = 0.001f;
    math::Vec4 position_max_radius = math::Vec4(1.0f, 1.0f, 1.0f, 5.0f);

    void update_influence_area() noexcept;

public:
    Point(core::Id my_id, system::stream::Stream* f, engine::Engine* e) noexcept;
    const math::Vec4& get_position_max_radius() const noexcept;
    const core::Real get_min_radius() const noexcept;
};
}
#endif
