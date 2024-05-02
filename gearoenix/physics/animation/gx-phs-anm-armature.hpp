#ifndef GEAROENIX_PHYSICS_ANIMATION_ARMATURE_HPP
#define GEAROENIX_PHYSICS_ANIMATION_ARMATURE_HPP
#include "../../core/ecs/gx-cr-ecs-component.hpp"

namespace gearoenix::physics::animation {
struct Manager;
struct Armature final : core::ecs::Component {
    std::size_t root_bone_index = static_cast<std::size_t>(-1);

    explicit Armature(std::string&& name);
    [[nodiscard]] static std::shared_ptr<Armature> construct(std::string&& name);
    ~Armature() final;
    [[nodiscard]] const boost::container::flat_set<std::type_index>& get_all_the_hierarchy_types_except_component() const final;
};
}
#endif