#ifndef GEAROENIX_PHYSICS_ACCELERATOR_BVH_HPP
#define GEAROENIX_PHYSICS_ACCELERATOR_BVH_HPP
#include "../../core/cr-static.hpp"
#include "../../math/math-aabb.hpp"
#include <optional>
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
        Node(const Type t) noexcept : node_type(t) {}
        Node(const Type t, const math::Aabb3 &volume) noexcept : node_type(t), volume(volume) {}
    public:
        virtual ~Node() noexcept = default;
    };

    class LeafNode : public Node {
        GX_GET_REF_PRV(std::vector<const collider::Collider*>, colliders)
    public:
        LeafNode(const std::vector<const collider::Collider*>& colliders, const math::Aabb3& volume) noexcept : 
            Node(Type::LEAF, volume), colliders(colliders) {}
    };

    class InternalNode : public Node {
        GX_GET_UPTR_PRV(Node, left)
        GX_GET_UPTR_PRV(Node, right)
    private:
        void init(const std::vector<const collider::Collider*>& colliders) noexcept;
    public:
        InternalNode(const std::vector<const collider::Collider*> &colliders) noexcept;
        InternalNode(const std::vector<const collider::Collider*> &colliders, const math::Aabb3& volume) noexcept;
    };

    std::unique_ptr<InternalNode> root;
public:
    Bvh() noexcept = default;
    ~Bvh() noexcept = default;
    void reset(const std::vector<const collider::Collider*> &colliders) noexcept;
    virtual std::optional<core::Real> hit(const math::Ray3& r) const noexcept = 0;
};
}
#endif
