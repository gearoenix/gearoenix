#include "phs-acc-bvh.hpp"
#include "../collider/phs-cld-collider.hpp"

constexpr int PER_NODE_COLLIDERS = 3;
constexpr int BINS_COUNT = 8;
constexpr int WALLS_COUNT = BINS_COUNT - 1;

void gearoenix::physics::accelerator::Bvh::InternalNode::init(const std::vector<const collider::Collider*>& colliders) noexcept
{
    static_assert(BINS_COUNT > 3);
    /// Sorting dimensions
    const math::Vec3& dims = volume.get_diameter();
    std::pair<int, core::Real> dimi[3];
    for (int i = 0; i < 3; ++i)
        dimi[i] = std::make_pair(i, dims[i]);
    if (dimi[0].second < dimi[1].second) std::swap(dimi[0], dimi[1]);
    if (dimi[0].second < dimi[2].second) std::swap(dimi[0], dimi[2]);
    if (dimi[1].second < dimi[2].second) std::swap(dimi[1], dimi[2]);
    for (const auto di : dimi) {
        struct Bin {
            std::vector<const collider::Collider*> c;
            /// TODO: batch the boxing process
            math::Aabb3 b;
        };
        Bin bins[BINS_COUNT];
        for (Bin& b : bins) b.c.reserve(colliders.size());
        const core::Real split_size = di.second / static_cast<core::Real>(BINS_COUNT);
        core::Real splits[WALLS_COUNT];
        core::Real wall = volume.get_lower()[di.first];
        for (core::Real& s : splits) {
            wall += split_size;
            s = wall;
        }
        for (const collider::Collider* c : colliders) {
            bool not_found = true;
            for (int i = 0; i < WALLS_COUNT; ++i) {
                if (c->get_box().get_center()[di.first] < splits[i]) {
                    bins[i].c.push_back(c);
                    bins[i].b.put_without_update(c->get_box());
                    not_found = false;
                    break;
                }
            }
            if (not_found) {
                bins[WALLS_COUNT].c.push_back(c);
                bins[WALLS_COUNT].b.put_without_update(c->get_box());
            }
        }
        // Overlapping centers check
        bool overlapped = false;
        for (int i = 0; i < BINS_COUNT; ++i)
            if (bins[i].c.size() == colliders.size()) {
                overlapped = true;
                break;
            }
            else if (bins[i].c.size() != 0) break;
        if (overlapped) continue;
        core::Real best_wall_cost = std::numeric_limits<core::Real>::max();
        int best_wall_index = -1;
        math::Aabb3 best_wall_left_box;
        math::Aabb3 best_wall_right_box;
        int best_wall_left_count;
        int best_wall_right_count;
        math::Aabb3 left_box;
        int left_count = 0;
        for (int i = 0; i < WALLS_COUNT; ++i)
        {
            left_box.put(bins[i].b);
            left_count += static_cast<int>(bins[i].c.size());
            const int right_count = static_cast<int>(colliders.size()) - left_count;
            math::Aabb3 right_box = bins[i + 1].b;
            for (int j = i + 2; j < BINS_COUNT; ++j)
                right_box.put_without_update(bins[j].b);
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
        std::vector<const collider::Collider*> left_colliders(std::move(bins[0].c));
        for (int i = 1; i < best_wall_index; ++i)
            for (auto c : bins[i].c)
                left_colliders.push_back(c);
        std::vector<const collider::Collider*> right_colliders(std::move(bins[WALLS_COUNT].c));
        for (int i = best_wall_index; i < WALLS_COUNT; ++i)
            for (auto c : bins[i].c)
                right_colliders.push_back(c);
        if (best_wall_left_count < PER_NODE_COLLIDERS) left = std::make_unique<LeafNode>(left_colliders, best_wall_left_box);
        else left = std::make_unique<InternalNode>(left_colliders, best_wall_left_box);
        if (best_wall_right_count < PER_NODE_COLLIDERS) right = std::make_unique<LeafNode>(right_colliders, best_wall_right_box);
        else right = std::make_unique<InternalNode>(right_colliders, best_wall_right_box);
        return;
    }
    left = std::make_unique<LeafNode>(colliders, volume);
}

gearoenix::physics::accelerator::Bvh::InternalNode::InternalNode(const std::vector<const collider::Collider*> &colliders) noexcept
    : Node(Type::INTERNAL)
{
    if(colliders.size() <= 3) {
        left = std::make_unique<LeafNode>(colliders);
        return;
    }
    for(const collider::Collider *c: colliders)
        volume.put_without_update(c->get_box());
    volume.update();
    init(colliders);
}

gearoenix::physics::accelerator::Bvh::InternalNode::InternalNode(const std::vector<const collider::Collider*>& colliders, const math::Aabb3& volume) noexcept
    : Node(Type::INTERNAL, volume)
{
    init(colliders);
}

void gearoenix::physics::accelerator::Bvh::reset(const std::vector<const collider::Collider*> &colliders) noexcept 
{
    root = std::make_unique<InternalNode>(colliders);
}