#ifndef GEAROENIX_PHYSICS_ACCELERATOR_BVH_HPP
#define GEAROENIX_PHYSICS_ACCELERATOR_BVH_HPP
#include "../../core/cr-static.hpp"
#include "../../math/math-aabb.hpp"
#include <functional>
#include <optional>
#include <string>
#include <utility>
#include <vector>

namespace gearoenix::math {
struct Ray3;
}

namespace gearoenix::system::stream {
class Stream;
}

namespace gearoenix::physics::collider {
class Collider;
}

namespace gearoenix::physics::accelerator {
struct Bvh {
    constexpr static const std::size_t per_node_colliders_count = 5;
    constexpr static const std::size_t bins_count = 32;
    constexpr static const std::size_t walls_count = bins_count - 1;

private:
    struct Bin {
        math::Aabb3 volume;
        std::vector<collider::Collider*> colliders;
    };

    struct Node {
        math::Aabb3 volume;
        std::vector<collider::Collider*> colliders;
        std::unique_ptr<Node> left = nullptr;
        std::unique_ptr<Node> right = nullptr;

        void init(
            std::size_t colliders_splits_starting_index,
            std::size_t colliders_splits_ending_index,
            std::vector<collider::Collider*>& colliders_splits,
            Bin (&bins)[bins_count],
            const math::Aabb3& volume) noexcept;
        [[nodiscard]] std::optional<std::pair<double, collider::Collider*>> hit(const math::Ray3& r, double d_min) const noexcept;
        void call_on_intersecting(const collider::Collider* cld, const std::function<void(collider::Collider* const cld)>& collided) const noexcept;
        void map(const std::function<void(collider::Collider* const cld)>& collided) const noexcept;
    };

    std::unique_ptr<Node> root;
    Bin bins_pool[bins_count];
    std::vector<collider::Collider*> collider_splits;

public:
    void reset(const std::vector<collider::Collider*>& colliders) noexcept;
    [[nodiscard]] std::optional<std::pair<double, collider::Collider*>> hit(const math::Ray3& r, double d_min) const noexcept;
    void call_on_intersecting(const collider::Collider* cld, const std::function<void(collider::Collider* cld)>& collided) const noexcept;
};
}
#endif
