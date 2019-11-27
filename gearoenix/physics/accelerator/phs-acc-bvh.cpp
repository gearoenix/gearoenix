#include "phs-acc-bvh.hpp"
#include "../../system/sys-log.hpp"
#include "../collider/phs-cld-collider.hpp"

constexpr int PER_NODE_COLLIDERS = 3;
constexpr int BINS_COUNT = 8;
constexpr int WALLS_COUNT = BINS_COUNT - 1;

std::string gearoenix::physics::accelerator::Bvh::Node::to_string() const noexcept
{
    std::string r(R"("node_type" : ")");
    switch (node_type) {
    case Type::INTERNAL:
        r += "INTERNAL";
        break;
    case Type::LEAF:
        r += "LEAF";
        break;
    }
    r += "\"";
    return r;
}

std::optional<std::pair<gearoenix::core::Real, gearoenix::physics::collider::Collider*>> gearoenix::physics::accelerator::Bvh::Node::hit(const math::Ray3& r, const core::Real d_min) const noexcept
{
    const auto h = volume.hit(r, d_min);
    if (h.has_value())
        return std::make_pair(*h, nullptr);
    return std::nullopt;
}

std::optional<std::pair<gearoenix::core::Real, gearoenix::physics::collider::Collider*>> gearoenix::physics::accelerator::Bvh::LeafNode::hit(const math::Ray3& r, const core::Real d_min) const noexcept
{
    auto h = Node::hit(r, d_min);
    if (h == std::nullopt)
        return std::nullopt;
    core::Real d_min_local = d_min;
    collider::Collider* hited_c = nullptr;
    for (auto c : colliders) {
        const auto hc = c->hit(r, d_min_local);
        if (hc == std::nullopt)
            continue;
        d_min_local = hc.value();
        hited_c = c;
    }
    if (d_min_local < d_min)
        return std::make_pair(d_min_local, hited_c);
    return std::nullopt;
}

std::string gearoenix::physics::accelerator::Bvh::LeafNode::to_string() const noexcept
{
    std::string r("{ ");
    r += Node::to_string();
    r += ", \"colliders_count\" : ";
    r += std::to_string(colliders.size());
    r += " }";
    return r;
}

void gearoenix::physics::accelerator::Bvh::LeafNode::call_on_intersecting(const collider::Collider* const cld, const std::function<void(collider::Collider* const cld)>& collided) const noexcept
{
    for (collider::Collider* const c : colliders) {
        if (c != nullptr) {
            if (cld->check_intersection(c->get_updated_box())) {
                collided(c);
            }
        }
    }
}

void gearoenix::physics::accelerator::Bvh::LeafNode::map(const std::function<void(collider::Collider* const cld)>& collided) const noexcept
{
    for (collider::Collider* const c : colliders) {
        if (c != nullptr) {
            collided(c);
        }
    }
}

void gearoenix::physics::accelerator::Bvh::InternalNode::init(const std::vector<collider::Collider*>& colliders) noexcept
{
    static_assert(BINS_COUNT > 3);
    /// Sorting dimensions
    const math::Vec3& dims = volume.get_diameter();
    std::pair<int, core::Real> dimi[3];
    for (int i = 0; i < 3; ++i) {
        dimi[i] = std::make_pair(i, dims[i]);
    }
    if (dimi[0].second < dimi[1].second) {
        std::swap(dimi[0], dimi[1]);
    }
    if (dimi[0].second < dimi[2].second) {
        std::swap(dimi[0], dimi[2]);
    }
    if (dimi[1].second < dimi[2].second) {
        std::swap(dimi[1], dimi[2]);
    }
    for (const auto di : dimi) {
        struct Bin {
            std::vector<collider::Collider*> c;
            /// TODO: batch the boxing process
            math::Aabb3 b;
        };
        Bin bins[BINS_COUNT];
        for (Bin& b : bins) {
            b.c.reserve(colliders.size());
        }
        const core::Real split_size = di.second / static_cast<core::Real>(BINS_COUNT);
        core::Real splits[WALLS_COUNT];
        core::Real wall = volume.get_lower()[di.first];
        for (core::Real& s : splits) {
            wall += split_size;
            s = wall;
        }
        for (collider::Collider* const c : colliders) {
            bool not_found = true;
            for (int i = 0; i < WALLS_COUNT; ++i) {
                if (c->get_updated_box().get_center()[di.first] < splits[i]) {
                    bins[i].c.push_back(c);
                    bins[i].b.put_without_update(c->get_updated_box());
                    not_found = false;
                    break;
                }
            }
            if (not_found) {
                bins[WALLS_COUNT].c.push_back(c);
                bins[WALLS_COUNT].b.put_without_update(c->get_updated_box());
            }
        }
        // Overlapping centers check
        bool overlapped = false;
        for (const Bin& b : bins) {
            if (b.c.size() == colliders.size()) {
                overlapped = true;
                break;
            } else if (!b.c.empty()) {
                break;
            }
        }
        if (overlapped) {
            continue;
        }
        core::Real best_wall_cost = std::numeric_limits<core::Real>::max();
        int best_wall_index = -1;
        math::Aabb3 best_wall_left_box;
        math::Aabb3 best_wall_right_box;
        int best_wall_left_count = -1;
        int best_wall_right_count = -1;
        math::Aabb3 left_box;
        int left_count = 0;
        for (int i = 0; i < WALLS_COUNT; ++i) {
            left_box.put(bins[i].b);
            left_count += static_cast<int>(bins[i].c.size());
            const int right_count = static_cast<int>(colliders.size()) - left_count;
            math::Aabb3 right_box = bins[i + 1].b;
            for (int j = i + 2; j < BINS_COUNT; ++j) {
                right_box.put_without_update(bins[j].b);
            }
            right_box.update();
            const core::Real cost = left_box.get_volume() * static_cast<core::Real>(left_count) + right_box.get_volume() * static_cast<core::Real>(right_count);
            if (cost < best_wall_cost) {
                best_wall_cost = cost;
                best_wall_index = i;
                best_wall_left_box = left_box;
                best_wall_right_box = right_box;
                best_wall_left_count = left_count;
                best_wall_right_count = right_count;
            }
        }
        ++best_wall_index;
        std::vector<collider::Collider*> left_colliders(std::move(bins[0].c));
        for (int i = 1; i < best_wall_index; ++i) {
            for (auto c : bins[i].c) {
                left_colliders.push_back(c);
            }
        }
        std::vector<collider::Collider*> right_colliders(std::move(bins[WALLS_COUNT].c));
        for (int i = best_wall_index; i < WALLS_COUNT; ++i) {
            for (auto c : bins[i].c) {
                right_colliders.push_back(c);
            }
        }
        if (best_wall_left_count <= PER_NODE_COLLIDERS) {
            left = std::make_unique<LeafNode>(left_colliders, best_wall_left_box);
        } else {
            left = std::make_unique<InternalNode>(left_colliders, best_wall_left_box);
        }
        if (best_wall_right_count <= PER_NODE_COLLIDERS) {
            right = std::make_unique<LeafNode>(right_colliders, best_wall_right_box);
        } 
        else {
            right = std::make_unique<InternalNode>(right_colliders, best_wall_right_box);
        }
        return;
    }
    left = std::make_unique<LeafNode>(colliders, volume);
}

gearoenix::physics::accelerator::Bvh::InternalNode::InternalNode(const std::vector<collider::Collider*>& colliders) noexcept
    : Node(Type::INTERNAL)
{
    for (const collider::Collider* const c : colliders) {
        volume.put_without_update(c->get_updated_box());
    }
    volume.update();
    if (colliders.size() <= PER_NODE_COLLIDERS) {
        left = std::make_unique<LeafNode>(colliders, volume);
        return;
    }
    init(colliders);
}

gearoenix::physics::accelerator::Bvh::InternalNode::InternalNode(const std::vector<collider::Collider*>& colliders, const math::Aabb3& volume) noexcept
    : Node(Type::INTERNAL, volume)
{
    init(colliders);
}

std::optional<std::pair<gearoenix::core::Real, gearoenix::physics::collider::Collider*>> gearoenix::physics::accelerator::Bvh::InternalNode::hit(const math::Ray3& r, const core::Real d_min) const noexcept
{
    const auto h = Node::hit(r, d_min);
    if (h == std::nullopt)
        return std::nullopt;
    const auto hl = (left == nullptr ? std::nullopt : left->hit(r, d_min));
    const auto [d_min_left, hited_c] = (hl == std::nullopt ? std::make_pair(d_min, nullptr) : *hl);
    (void)hited_c;
    const auto hr = (right == nullptr ? std::nullopt : right->hit(r, d_min_left));
    if (hr == std::nullopt)
        return hl;
    return hr;
}

std::string gearoenix::physics::accelerator::Bvh::InternalNode::to_string() const noexcept
{
    std::string r("{ ");
    r += Node::to_string();
    if (nullptr != left)
        r += ", \"left\" : " + left->to_string();
    if (nullptr != right)
        r += ", \"right\" : " + right->to_string();
    r += " }";
    return r;
}

void gearoenix::physics::accelerator::Bvh::InternalNode::call_on_intersecting(const collider::Collider* const cld, const std::function<void(collider::Collider* const)>& collided) const noexcept
{
#define GX_HELPER(n)                                                     \
    if (n != nullptr) {                                                  \
        const auto is = cld->check_intersection_status(n->get_volume()); \
        if (math::IntersectionStatus::Cut == is) {                       \
            n->call_on_intersecting(cld, collided);                      \
        } else if (math::IntersectionStatus::In == is) {                 \
            n->map(collided);                                            \
        }                                                                \
    }
    GX_HELPER(left)
    GX_HELPER(right)
#undef GX_HELPER
}

void gearoenix::physics::accelerator::Bvh::InternalNode::map(const std::function<void(collider::Collider* const)>& collided) const noexcept
{
#define GX_HELPER(n)      \
    if (nullptr != n) {   \
        n->map(collided); \
    }
    GX_HELPER(left)
    GX_HELPER(right)
#undef GX_HELPER
}

void gearoenix::physics::accelerator::Bvh::reset(const std::vector<collider::Collider*>& colliders) noexcept
{
    root = std::make_unique<InternalNode>(colliders);
}

std::optional<std::pair<gearoenix::core::Real, gearoenix::physics::collider::Collider*>> gearoenix::physics::accelerator::Bvh::hit(const math::Ray3& r, const core::Real d_min) const noexcept
{
    if (root != nullptr)
        return root->hit(r, d_min);
    return std::nullopt;
}

std::string gearoenix::physics::accelerator::Bvh::to_string() const noexcept
{
    std::string r("{ \"root\" : ");
    if (nullptr != root)
        r += root->to_string();
    else
        r += "\"\"";
    r += " }";
    return r;
}

void gearoenix::physics::accelerator::Bvh::call_on_intersecting(const collider::Collider* const cld, const std::function<void(collider::Collider* cld)>& collided) const noexcept
{
    root->call_on_intersecting(cld, collided);
    //root->map(collided);
}
