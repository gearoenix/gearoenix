#ifndef GEAROENIX_PHYSICS_ANIMATION_BONE_HPP
#define GEAROENIX_PHYSICS_ANIMATION_BONE_HPP
#include "../gx-phs-transformation.hpp"
#include <vector>
#include <string>

namespace gearoenix::render::engine {
    struct Engine;
}

namespace gearoenix::physics::animation {
    struct Manager;
    struct Bone final {
        typedef std::uint32_t Index;

        Transformation transform;
        std::string name;
        Index parent_index = static_cast<Index>(-1);
        Index children_count = static_cast<Index>(-1);

        Bone() noexcept = default;
        [[nodiscard]] static Index construct(std::vector<std::uint8_t>& memory, const std::string& name, Index children_count) noexcept;
        Bone(const Bone&) = delete;
        Bone(Bone&&) noexcept = default;
        static void set_children(std::vector<std::uint8_t>& memory, Index this_index, const std::vector<Index>& children) noexcept;
        [[nodiscard]] static Transformation& get_transformation(std::vector<std::uint8_t>& memory, Index this_index) noexcept;
        void delete_children(Manager& manager) noexcept;
        ~Bone() noexcept;
        void operator=(const Bone& o) = delete;
        void operator=(Bone&& o) = delete;
        void update() noexcept;
    };
}
#endif