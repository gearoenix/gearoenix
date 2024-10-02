#ifndef GEAROENIX_PHYSICS_ANIMATION_ARMATURE_HPP
#define GEAROENIX_PHYSICS_ANIMATION_ARMATURE_HPP
#include "../../core/ecs/gx-cr-ecs-component.hpp"

namespace gearoenix::physics::animation {
struct Manager;
struct Armature final : core::ecs::Component {
    std::size_t root_bone_index = static_cast<std::size_t>(-1);

    Armature(std::string&& name, core::ecs::entity_id_t entity_id);
    [[nodiscard]] static std::shared_ptr<Armature> construct(std::string&& name, core::ecs::entity_id_t entity_id);
    ~Armature() override;
    [[nodiscard]] const HierarchyTypes& get_hierarchy_types() const override;
};
}
#endif