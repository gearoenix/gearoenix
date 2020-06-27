#ifndef GEAROENIX_RENDER_LIGHT_POINT_HPP
#define GEAROENIX_RENDER_LIGHT_POINT_HPP
#include "../../core/sync/cr-sync-end-caller.hpp"
#include "../../math/math-vector-4d.hpp"
#include "rnd-lt-light.hpp"

namespace gearoenix::render::light {
class Point : public Light {
private:
    double min_radius = 0.001f;
    math::Vec4<double> position_max_radius = math::Vec4(1.0, 1.0, 1.0, 5.0);

    void update_influence_area() noexcept;

public:
    Point(core::Id id, std::string name, system::stream::Stream* f, engine::Engine* e) noexcept;
    [[nodiscard]] const math::Vec4<double>& get_position_max_radius() const noexcept;
    [[nodiscard]] double get_min_radius() const noexcept;
    bool is_in_light(const model::Model*) const noexcept override;
};
}
#endif
