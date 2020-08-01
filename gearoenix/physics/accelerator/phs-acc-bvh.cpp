#include "phs-acc-bvh.hpp"
#include "../collider/phs-cld-collider.hpp"

void gearoenix::physics::accelerator::Bvh::Node::init(
    std::vector<Node>& nodes,
    std::size_t& nodes_current_index,
    std::size_t colliders_splits_starting_index,
    std::size_t colliders_splits_ending_index,
    std::vector<collider::Collider*>& colliders_splits,
    Bin (&bins)[bins_count],
    const gearoenix::math::Aabb3& v) noexcept
{

    static_assert(bins_count > 4 && bins_count % 2 == 0, "Bins count is unexpected.");

    volume = v;
    colliders.clear();
    left_index = static_cast<std::size_t>(-1);
    left = nullptr;
    right_index = static_cast<std::size_t>(-1);
    right = nullptr;

    const auto colliders_splits_size = colliders_splits_ending_index - colliders_splits_starting_index;

    if (colliders_splits_size <= per_node_colliders_count) {
        for (auto i = colliders_splits_starting_index; i < colliders_splits_ending_index; ++i)
            colliders.push_back(colliders_splits[i]);
        return;
    }
    /// Sorting dimensions
    const math::Vec3<double>& dims = volume.get_diameter();
    std::pair<int, double> dim_i[3];
    for (int i = 0; i < 3; ++i) {
        dim_i[i] = std::make_pair(i, dims[i]);
    }
    if (dim_i[0].second < dim_i[1].second) {
        std::swap(dim_i[0], dim_i[1]);
    }
    if (dim_i[0].second < dim_i[2].second) {
        std::swap(dim_i[0], dim_i[2]);
    }
    if (dim_i[1].second < dim_i[2].second) {
        std::swap(dim_i[1], dim_i[2]);
    }
    /// Clearing bins
    for (auto& bin : bins) {
        bin.volume.reset();
        bin.colliders.clear();
    }
    /// Searching for dimension with a good split
    for (const auto& di : dim_i) {
        const auto split_size = di.second / static_cast<double>(bins_count);
        double splits[walls_count] = { 0.0 };
        double wall = volume.get_lower()[di.first];
        for (double& s : splits) {
            wall += split_size;
            s = wall;
        }
        for (auto ci = colliders_splits_starting_index; ci < colliders_splits_ending_index; ++ci) {
            collider::Collider* const c = colliders_splits[ci];
            bool not_found = true;
            for (std::size_t wi = 0; wi < walls_count; ++wi) {
                if (c->get_updated_box().get_center()[di.first] < splits[wi]) {
                    auto& bin = bins[wi];
                    bin.colliders.push_back(c);
                    bin.volume.put_without_update(c->get_updated_box());
                    not_found = false;
                    break;
                }
            }
            if (not_found) {
                auto& bin = bins[walls_count];
                bin.colliders.push_back(c);
                bin.volume.put_without_update(c->get_updated_box());
            }
        }
        /// Overlapping centers check
        bool overlapped = false;
        for (const auto& b : bins) {
            if (b.colliders.size() == colliders_splits_size) {
                overlapped = true;
                break;
            } else if (!b.colliders.empty()) {
                break;
            }
        }
        if (overlapped) {
            continue;
        }
        auto best_wall_cost = std::numeric_limits<double>::max();
        auto best_wall_index = static_cast<std::size_t>(-1);
        math::Aabb3 best_wall_left_box {};
        math::Aabb3 best_wall_right_box {};
        math::Aabb3 left_box {};
        std::size_t left_count = 0;
        for (std::size_t i = 0; i < walls_count; ++i) {
            left_box.put(bins[i].volume);
            left_count += bins[i].colliders.size();
            const auto right_count = colliders_splits_size - left_count;
            math::Aabb3 right_box = bins[i + 1].volume;
            for (auto j = i + 2; j < bins_count; ++j) {
                right_box.put_without_update(bins[j].volume);
            }
            right_box.update();
            const auto cost = left_box.get_volume() * static_cast<double>(left_count) + right_box.get_volume() * static_cast<double>(right_count);
            if (cost < best_wall_cost) {
                best_wall_cost = cost;
                best_wall_index = i;
                best_wall_left_box = left_box;
                best_wall_right_box = right_box;
            }
        }
        ++best_wall_index;

        std::size_t left_colliders_splits_starting_index = colliders_splits.size();
        for (std::size_t i = 0; i < best_wall_index; ++i) {
            for (auto c : bins[i].colliders) {
                colliders_splits.push_back(c);
            }
        }
        std::size_t left_colliders_splits_ending_index = colliders_splits.size();

        std::size_t right_colliders_splits_starting_index = colliders_splits.size();
        for (auto i = best_wall_index; i < walls_count; ++i) {
            for (auto c : bins[i].colliders) {
                colliders_splits.push_back(c);
            }
        }
        std::size_t right_colliders_splits_ending_index = colliders_splits.size();

        ++nodes_current_index;
        left_index = nodes_current_index;
        ++nodes_current_index;
        right_index = nodes_current_index;

        while (nodes_current_index >= nodes.size())
            nodes.emplace_back();

        nodes[left_index].init(
            nodes,
            nodes_current_index,
            left_colliders_splits_starting_index,
            left_colliders_splits_ending_index,
            colliders_splits,
            bins,
            best_wall_left_box);

        nodes[right_index].init(
            nodes,
            nodes_current_index,
            right_colliders_splits_starting_index,
            right_colliders_splits_ending_index,
            colliders_splits,
            bins,
            best_wall_right_box);

        return;
    }
    for (auto i = colliders_splits_starting_index; i < colliders_splits_ending_index; ++i)
        colliders.push_back(colliders_splits[i]);
}

void gearoenix::physics::accelerator::Bvh::Node::set_pointers(std::vector<Node>& nodes) noexcept
{
    if (left_index == static_cast<std::size_t>(-1))
        return;
    left = &nodes[left_index];
    right = &nodes[right_index];
    left->set_pointers(nodes);
    right->set_pointers(nodes);
}

std::optional<std::pair<double, gearoenix::physics::collider::Collider*>> gearoenix::physics::accelerator::Bvh::Node::hit(const math::Ray3& r, const double d_min) const noexcept
{
    auto min_dis = d_min;
    std::optional<std::pair<double, collider::Collider*>> result = std::nullopt;
    if (nullptr == left) {
        for (auto* const c : colliders) {
            if (auto d = c->hit(r, min_dis)) {
                min_dis = *d;
                result = std::make_pair(min_dis, c);
            }
        }
        return result;
    }
    if (auto left_dis = left->volume.hit(r, min_dis)) {
        if (auto right_dis = right->volume.hit(r, min_dis)) {
            if (*left_dis < *right_dis) {
                result = left->hit(r, min_dis);
                if (result) {
                    min_dis = result->first;
                }
                if (auto right_result = right->hit(r, min_dis))
                    result = right_result;
                return result;
            }
            result = right->hit(r, min_dis);
            if (result) {
                min_dis = result->first;
            }
            if (auto left_result = left->hit(r, min_dis))
                result = left_result;
            return result;
        }
        return left->hit(r, min_dis);
    }
    if (right->volume.hit(r, min_dis)) {
        return right->hit(r, min_dis);
    }
    return result;
}

void gearoenix::physics::accelerator::Bvh::Node::call_on_intersecting(
    const collider::Collider* const cld,
    const std::function<void(collider::Collider* const)>& collided) const noexcept
{
#define GX_HELPER(n)                                               \
    {                                                              \
        const auto is = cld->check_intersection_status(n->volume); \
        if (math::IntersectionStatus::Cut == is) {                 \
            n->call_on_intersecting(cld, collided);                \
        } else if (math::IntersectionStatus::In == is) {           \
            n->map(collided);                                      \
        }                                                          \
    }
    if (nullptr == left) {
        for (collider::Collider* const c : colliders) {
            if (cld->check_intersection(c->get_updated_box())) {
                collided(c);
            }
        }
    } else {
        GX_HELPER(left)
        GX_HELPER(right)
    }
#undef GX_HELPER
}

void gearoenix::physics::accelerator::Bvh::Node::map(
    const std::function<void(collider::Collider* const)>& collided) const noexcept
{
    if (nullptr == left) {
        for (collider::Collider* const c : colliders) {
            collided(c);
        }
    } else {
        left->map(collided);
        right->map(collided);
    }
}

void gearoenix::physics::accelerator::Bvh::reset(const std::vector<collider::Collider*>& colliders) noexcept
{
    math::Aabb3 volume;
    collider_splits.clear();
    /// Because of std for loop for vector is not constexpr in Linux
    for (auto c : colliders) {
        volume.put_without_update(c->get_updated_box());
        collider_splits.push_back(c);
    }
    volume.update();
    std::size_t node_current_index = 0;
    nodes_pool[0].init(
        nodes_pool,
        node_current_index,
        0,
        colliders.size(),
        collider_splits,
        bins_pool,
        volume);

    root = &nodes_pool[0];
    root->set_pointers(nodes_pool);
}

std::optional<std::pair<double, gearoenix::physics::collider::Collider*>> gearoenix::physics::accelerator::Bvh::hit(
    const math::Ray3& r, const double d_min) const noexcept
{
    return root->hit(r, d_min);
}

void gearoenix::physics::accelerator::Bvh::call_on_intersecting(
    const collider::Collider* const cld,
    const std::function<void(collider::Collider*)>& collided) const noexcept
{
    root->call_on_intersecting(cld, collided);
}
