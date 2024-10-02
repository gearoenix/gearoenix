#ifndef GEAROENIX_RENDER_LIGHT_POINT_HPP
#define GEAROENIX_RENDER_LIGHT_POINT_HPP
#include "gx-rnd-lt-light.hpp"

namespace gearoenix::render::light {
struct Point final : Light {
    math::Vec3<float> position = { 0.0f, 0.0f, 0.0f };

private:
    [[nodiscard]] const HierarchyTypes& get_hierarchy_types() const override;

public:
    Point(std::string&& name, core::ecs::entity_id_t entity_id);
    [[nodiscard]] static std::shared_ptr<Point> construct(std::string&& name, core::ecs::entity_id_t entity_id);
    ~Point() override;
};
}
#endif
