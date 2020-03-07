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
class Bvh {
private:
    class Node {
    public:
        enum struct Type {
            INTERNAL,
            LEAF,
        };
        GX_GET_CVAL_PRT(Type, node_type)
        GX_GET_REF_PRT(math::Aabb3, volume)
    protected:
        explicit Node(const Type t) noexcept
            : node_type(t)
        {
        }
        Node(const Type t, math::Aabb3 volume) noexcept
            : node_type(t)
            , volume(std::move(volume))
        {
        }

    public:
        virtual ~Node() noexcept = default;
        [[nodiscard]] virtual std::string to_string() const noexcept;
        [[nodiscard]] virtual std::optional<std::pair<double, collider::Collider*>> hit(const math::Ray3& r, double d_min) const noexcept;
        virtual void call_on_intersecting(const collider::Collider* cld, const std::function<void(collider::Collider* const cld)>& collided) const noexcept = 0;
        virtual void map(const std::function<void(collider::Collider* const cld)>& collided) const noexcept = 0;
    };

    class LeafNode : public Node {
        GX_GET_REF_PRV(std::vector<collider::Collider*>, colliders)
    public:
        LeafNode(std::vector<collider::Collider*> colliders, const math::Aabb3& volume) noexcept
            : Node(Type::LEAF, volume)
            , colliders(std::move(colliders))
        {
        }
        [[nodiscard]] std::optional<std::pair<double, collider::Collider*>> hit(const math::Ray3& r, double d_min) const noexcept final;
        [[nodiscard]] std::string to_string() const noexcept final;
        void call_on_intersecting(const collider::Collider* cld, const std::function<void(collider::Collider* const cld)>& collided) const noexcept final;
        void map(const std::function<void(collider::Collider* const cld)>& collided) const noexcept final;
    };

    class InternalNode : public Node {
        GX_GET_UPTR_PRV(Node, left)
        GX_GET_UPTR_PRV(Node, right)

    private:
        void init(const std::vector<collider::Collider*>& colliders) noexcept;

    public:
        explicit InternalNode(const std::vector<collider::Collider*>& colliders) noexcept;
        InternalNode(const std::vector<collider::Collider*>& colliders, const math::Aabb3& volume) noexcept;
        [[nodiscard]] std::optional<std::pair<double, collider::Collider*>> hit(const math::Ray3& r, double d_min) const noexcept final;
        [[nodiscard]] std::string to_string() const noexcept final;
        void call_on_intersecting(const collider::Collider* cld, const std::function<void(collider::Collider* const cld)>& collided) const noexcept final;
        void map(const std::function<void(collider::Collider* const cld)>& collided) const noexcept final;
    };

    std::unique_ptr<InternalNode> root;

public:
    Bvh() noexcept = default;
    ~Bvh() noexcept = default;
    void reset(const std::vector<collider::Collider*>& colliders) noexcept;
    [[nodiscard]] std::optional<std::pair<double, collider::Collider*>> hit(const math::Ray3& r, double d_min) const noexcept;
    [[nodiscard]] std::string to_string() const noexcept;
    void call_on_intersecting(const collider::Collider* cld, const std::function<void(collider::Collider* cld)>& collided) const noexcept;
};
}
#endif
