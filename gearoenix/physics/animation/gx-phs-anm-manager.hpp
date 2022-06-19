#ifndef GEAROENIX_PHYSICS_ANIMATION_MANAGER_HPP
#define GEAROENIX_PHYSICS_ANIMATION_MANAGER_HPP
#include "../../platform/macro/gx-plt-mcr-lock.hpp"
#include <vector>
#include <string>

namespace gearoenix::core::ecs {
    struct EntityBuilder;
}

namespace gearoenix::render::engine {
struct Engine;
}

namespace gearoenix::physics::animation {
struct Animation;
struct Manager final {
    render::engine::Engine& e;
private:
    GX_CREATE_GUARD(bones);
    std::vector<std::uint8_t> bones;
    std::vector<std::uint32_t> bone_indices;

public:
    explicit Manager(render::engine::Engine& e) noexcept;
    Manager(const Manager&) = delete;
    Manager(Manager&&) = delete;
    ~Manager() noexcept;
    void operator=(const Manager& o) = delete;
    void operator=(Manager&& o) = delete;
    void create_armature(core::ecs::EntityBuilder& builder, const std::string& name, const std::vector<std::uint8_t>& armature_bones) noexcept;
    void update() noexcept;
};
}
#endif