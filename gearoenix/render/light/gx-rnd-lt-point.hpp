#ifndef GEAROENIX_RENDER_LIGHT_POINT_HPP
#define GEAROENIX_RENDER_LIGHT_POINT_HPP
#include "gx-rnd-lt-light.hpp"

namespace gearoenix::render::light {
struct Point final : Light {
    math::Vec3<float> position = { 0.0f, 0.0f, 0.0f };

private:
    [[nodiscard]] const boost::container::flat_set<std::type_index>& get_all_the_hierarchy_types_except_component() const override;

public:
    explicit Point(std::string&& name);
    [[nodiscard]] static std::shared_ptr<Point> construct(std::string&& name);
    ~Point() override;
};
}
#endif
