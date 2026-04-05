#pragma once
#include "../../core/macro/gx-cr-mcr-assert.hpp"
#include "../../math/gx-math-aabb.hpp"
#include "../../math/gx-math-ray.hpp"

#include <array>
#include <optional>
#include <utility>
#include <vector>

namespace gearoenix::physics::accelerator {
struct Bvh final {
    using index_t = std::uint32_t;

    constexpr static index_t local_data_size = 5u;
    constexpr static index_t bins_count = 32u;
    constexpr static index_t walls_count = bins_count - 1u;
    constexpr static index_t leaf_flag = 0x80000000u;


    static_assert(bins_count > 4u && bins_count % 2u == 0u, "Bins count is unexpected.");

    struct Data final {
        math::Aabb3<core::fp_t> box;
        index_t index;
    };

private:
    /// 32-byte slim node: float AABB (upper+lower) + child offsets.
    /// MSB of left encodes leaf/internal: if set, lower 31 bits = inline data count.
    struct Node final {
        math::Vec3<float> upper;
        math::Vec3<float> lower;
        index_t left;
        index_t right;
    };

    static_assert(sizeof(Node) == 32, "Node must be exactly 32 bytes.");

    struct Bin final {
        math::Aabb3<core::fp_t> volume;
        index_t count = 0;
    };

    std::vector<std::uint8_t> nodes;
    math::Aabb3<core::fp_t> accu_center_volume;
    math::Aabb3<core::fp_t> accu_volume;
    std::array<Bin, bins_count> bins;
    std::vector<Data> data;

    [[nodiscard]] Node& node_at(const index_t ptr) { return *reinterpret_cast<Node*>(&nodes[ptr]); }
    [[nodiscard]] const Node& node_at(const index_t ptr) const { return *reinterpret_cast<const Node*>(&nodes[ptr]); }
    [[nodiscard]] Data& data_at(const index_t ptr) { return *reinterpret_cast<Data*>(&nodes[ptr]); }
    [[nodiscard]] const Data& data_at(const index_t ptr) const { return *reinterpret_cast<const Data*>(&nodes[ptr]); }

    [[nodiscard]] static bool is_leaf(const Node& n) { return (n.left & leaf_flag) != 0; }
    [[nodiscard]] static index_t leaf_count(const Node& n) { return n.left & ~leaf_flag; }

    index_t allocate_bytes(const index_t sz)
    {
        const auto off = static_cast<index_t>(nodes.size());
        nodes.resize(off + sz);
        return off;
    }

    template <typename T>
    std::pair<index_t, T&> allocate()
    {
        const auto off = allocate_bytes(static_cast<index_t>(sizeof(T)));
        auto* p = new (&nodes[off]) T();
        return { off, *p };
    }

    void make_leaf(const math::Aabb3<core::fp_t>& volume, const index_t data_starting_index, const index_t data_ending_index)
    {
        const auto count = data_ending_index - data_starting_index;
        auto [_, node] = allocate<Node>();
        node.upper = volume.get_upper().to<float>();
        node.lower = volume.get_lower().to<float>();
        node.left = leaf_flag | count;
        node.right = 0;
        for (auto data_index = data_starting_index; data_index < data_ending_index; ++data_index) {
            allocate<Data>().second = data[data_index];
        }
    }

    void create_node(
        const index_t data_starting_index,
        const index_t data_ending_index,
        const math::Aabb3<core::fp_t>& center_volume,
        const math::Aabb3<core::fp_t>& volume)
    {
        const auto data_size = data_ending_index - data_starting_index;
        GX_ASSERT_D(data_size > 0);
        if (data_size <= local_data_size) {
            make_leaf(volume, data_starting_index, data_ending_index);
            return;
        }

        const auto& dimensions = center_volume.get_diameter();
        std::array<std::pair<index_t, core::fp_t>, 3> dimensions_indices;
        for (index_t dimension_index = 0; dimension_index < 3; ++dimension_index) {
            dimensions_indices[dimension_index] = { dimension_index, dimensions[dimension_index] };
        }
        if (dimensions_indices[0].second < dimensions_indices[1].second) {
            std::swap(dimensions_indices[0], dimensions_indices[1]);
        }
        if (dimensions_indices[0].second < dimensions_indices[2].second) {
            std::swap(dimensions_indices[0], dimensions_indices[2]);
        }
        if (dimensions_indices[1].second < dimensions_indices[2].second) {
            std::swap(dimensions_indices[1], dimensions_indices[2]);
        }

        for (const auto& dimension_index : dimensions_indices) {
            if (0.0 >= dimension_index.second) {
                continue;
            }

            for (auto& bin : bins) {
                bin.volume.reset();
                bin.count = 0;
            }

            /// O(1) bin assignment: count + accumulate volumes only (no data copies)
            const auto inv_split = static_cast<core::fp_t>(bins_count) / dimension_index.second;
            const auto lower_val = volume.get_lower()[dimension_index.first];

            for (auto data_index = data_starting_index; data_index < data_ending_index; ++data_index) {
                const auto& d = data[data_index];
                auto bin_idx = static_cast<index_t>((d.box.get_center()[dimension_index.first] - lower_val) * inv_split);
                if (bin_idx >= bins_count) {
                    bin_idx = bins_count - 1;
                }
                ++bins[bin_idx].count;
                bins[bin_idx].volume.put_without_update(d.box);
            }

            bool overlapped = false;
            for (const auto& b : bins) {
                if (b.count == data_size) {
                    overlapped = true;
                    break;
                }
                if (b.count > 0) {
                    break;
                }
            }
            if (overlapped) {
                continue;
            }

            for (auto& bin : bins) {
                bin.volume.update();
            }

            /// Prefix-scan SAH: O(bins) reverse prefix for right boxes
            std::array<math::Aabb3<core::fp_t>, walls_count> right_boxes;
            std::array<index_t, walls_count> right_counts;
            right_boxes[walls_count - 1] = bins[bins_count - 1].volume;
            right_counts[walls_count - 1] = bins[bins_count - 1].count;
            for (index_t ri = walls_count - 1; ri > 0;) {
                const auto ri_prev = ri - 1;
                right_boxes[ri_prev] = right_boxes[ri];
                right_boxes[ri_prev].put(bins[ri].volume);
                right_counts[ri_prev] = right_counts[ri] + bins[ri].count;
                ri = ri_prev;
            }

            /// Single left-to-right sweep for SAH evaluation
            auto best_wall_cost = std::numeric_limits<core::fp_t>::max();
            auto best_wall_index = static_cast<index_t>(-1);
            math::Aabb3<core::fp_t> best_wall_left_box;
            math::Aabb3<core::fp_t> best_wall_right_box;
            math::Aabb3<core::fp_t> left_box;
            index_t left_count = 0;

            for (index_t wall_index = 0; wall_index < walls_count; ++wall_index) {
                left_count += bins[wall_index].count;
                if (left_count == 0) {
                    continue;
                }
                left_box.put(bins[wall_index].volume);
                if (right_counts[wall_index] == 0) {
                    break;
                }
                const auto cost = left_box.get_volume() * static_cast<core::fp_t>(left_count)
                    + right_boxes[wall_index].get_volume() * static_cast<core::fp_t>(right_counts[wall_index]);
                if (cost < best_wall_cost) {
                    best_wall_cost = cost;
                    best_wall_index = wall_index;
                    best_wall_left_box = left_box;
                    best_wall_right_box = right_boxes[wall_index];
                }
            }

            if (best_wall_index == static_cast<index_t>(-1)) {
                continue;
            }

            ++best_wall_index;

            /// In-place partition: rearrange data[data_starting_index..data_ending_index) so that
            /// items in bins [0..best_wall_index) come first, no extra copies.
            auto mid = data_starting_index;
            for (auto data_index = data_starting_index; data_index < data_ending_index; ++data_index) {
                auto bin_idx = static_cast<index_t>((data[data_index].box.get_center()[dimension_index.first] - lower_val) * inv_split);
                if (bin_idx >= bins_count) {
                    bin_idx = bins_count - 1;
                }
                if (bin_idx < best_wall_index) {
                    if (data_index != mid) {
                        std::swap(data[data_index], data[mid]);
                    }
                    ++mid;
                }
            }

            GX_ASSERT_D(mid > data_starting_index && mid < data_ending_index);

            math::Aabb3<core::fp_t> left_center_volume;
            for (auto data_index = data_starting_index; data_index < mid; ++data_index) {
                left_center_volume.put_without_update(data[data_index].box.get_center());
            }
            left_center_volume.update();

            math::Aabb3<core::fp_t> right_center_volume;
            for (auto data_index = mid; data_index < data_ending_index; ++data_index) {
                right_center_volume.put_without_update(data[data_index].box.get_center());
            }
            right_center_volume.update();

            auto [current_node_index, current_node] = allocate<Node>();
            current_node.upper = math::Vec3<float>(volume.get_upper());
            current_node.lower = math::Vec3<float>(volume.get_lower());

            current_node.left = static_cast<index_t>(nodes.size());
            create_node(data_starting_index, mid, left_center_volume, best_wall_left_box);

            node_at(current_node_index).right = static_cast<index_t>(nodes.size());
            create_node(mid, data_ending_index, right_center_volume, best_wall_right_box);

            return;
        }

        make_leaf(volume, data_starting_index, data_ending_index);
    }

    /// Float AABB-ray intersection for slim traversal nodes
    [[nodiscard]] static std::optional<float> node_hit(
        const Node& n, const math::Vec3<float>& ro, const math::Vec3<float>& rrd, const float d_min)
    {
        const auto t0 = (n.lower - ro) * rrd;
        const auto t1 = (n.upper - ro) * rrd;
        const auto t_min = t0.minimum(t1).maximum();
        const auto t_max = t0.maximum(t1).minimum();
        if (t_max >= 0.0f && t_min <= t_max && t_min < d_min) {
            return t_min;
        }
        return std::nullopt;
    }

    /// Ray hit traversal — caller has already tested this node's box
    [[nodiscard]] std::optional<std::pair<core::fp_t, const Data*>> hit(
        const index_t ptr,
        const math::Ray3<core::fp_t>& ray,
        const math::Vec3<float>& ro,
        const math::Vec3<float>& rrd,
        const core::fp_t minimum_distance) const
    {
        const auto& node = node_at(ptr);

        if (is_leaf(node)) {
            const auto data_size = leaf_count(node);
            auto min_dis = minimum_distance;
            const Data* min_data = nullptr;
            auto data_ptr = ptr + static_cast<index_t>(sizeof(Node));
            for (index_t data_index = 0; data_index < data_size; ++data_index, data_ptr += static_cast<index_t>(sizeof(Data))) {
                const auto& d = data_at(data_ptr);
                if (const auto h = d.box.hit(ray, min_dis); h.has_value()) {
                    min_dis = *h;
                    min_data = &d;
                }
            }
            if (nullptr == min_data) {
                return std::nullopt;
            }
            return std::make_pair(min_dis, min_data);
        }

        /// Near-first traversal: test both children, visit nearer first
        const auto left_t = node_hit(node_at(node.left), ro, rrd, static_cast<float>(minimum_distance));
        const auto right_t = node_hit(node_at(node.right), ro, rrd, static_cast<float>(minimum_distance));

        auto near_ptr = node.left;
        auto far_ptr = node.right;
        auto near_t = left_t;
        auto far_t = right_t;
        if (!near_t.has_value() || (far_t.has_value() && *far_t < *near_t)) {
            std::swap(near_ptr, far_ptr);
            std::swap(near_t, far_t);
        }

        std::optional<std::pair<core::fp_t, const Data*>> hit_result;
        if (near_t.has_value()) {
            hit_result = hit(near_ptr, ray, ro, rrd, minimum_distance);
        }

        const auto far_min = hit_result ? hit_result->first : minimum_distance;
        if (far_t.has_value() && *far_t < static_cast<float>(far_min)) {
            if (auto fr = hit(far_ptr, ray, ro, rrd, far_min); fr.has_value()) {
                hit_result = fr;
            }
        }

        return hit_result;
    }

    template <typename Function>
    void call_on_all(const index_t ptr, Function&& function)
    {
        const auto& node = node_at(ptr);
        if (is_leaf(node)) {
            const auto data_size = leaf_count(node);
            auto data_ptr = ptr + static_cast<index_t>(sizeof(Node));
            for (index_t data_index = 0; data_index < data_size; ++data_index, data_ptr += static_cast<index_t>(sizeof(Data))) {
                function(data_at(data_ptr));
            }
        } else {
            call_on_all(node.left, function);
            call_on_all(node.right, function);
        }
    }

    template <typename Collider, typename Function>
    void call_on_intersecting(const index_t ptr, const Collider& cld, Function&& on_intersection)
    {
        const auto& node = node_at(ptr);
        const math::Aabb3 node_box(math::Vec3<core::fp_t>(node.upper), math::Vec3<core::fp_t>(node.lower));
        const auto intersection_status = cld.check_intersection_status(node_box);
        if (math::IntersectionStatus::Out == intersection_status) {
            return;
        }
        if (math::IntersectionStatus::In == intersection_status) {
            call_on_all(ptr, on_intersection);
            return;
        }
        if (is_leaf(node)) {
            const auto data_size = leaf_count(node);
            auto data_ptr = ptr + static_cast<index_t>(sizeof(Node));
            for (index_t data_index = 0; data_index < data_size; ++data_index, data_ptr += static_cast<index_t>(sizeof(Data))) {
                auto& d = data_at(data_ptr);
                if (math::IntersectionStatus::Out != cld.check_intersection_status(d.box)) {
                    on_intersection(d);
                }
            }
        } else {
            call_on_intersecting(node.left, cld, on_intersection);
            call_on_intersecting(node.right, cld, on_intersection);
        }
    }

    template <typename Collider, typename Function, typename ElseFunction>
    void call_on_intersecting(const index_t ptr, const Collider& cld, Function&& on_intersection, ElseFunction&& not_intersection)
    {
        const auto& node = node_at(ptr);
        const math::Aabb3 node_box(node.upper.to<core::fp_t>(), node.lower.to<core::fp_t>());
        const auto intersection_status = cld.check_intersection_status(node_box);
        if (math::IntersectionStatus::Out == intersection_status) {
            call_on_all(ptr, not_intersection);
            return;
        }
        if (math::IntersectionStatus::In == intersection_status) {
            call_on_all(ptr, on_intersection);
            return;
        }
        if (is_leaf(node)) {
            const auto data_size = leaf_count(node);
            auto data_ptr = ptr + static_cast<index_t>(sizeof(Node));
            for (index_t data_index = 0; data_index < data_size; ++data_index, data_ptr += static_cast<index_t>(sizeof(Data))) {
                auto& d = data_at(data_ptr);
                if (math::IntersectionStatus::Out == cld.check_intersection_status(d.box)) {
                    not_intersection(d);
                } else {
                    on_intersection(d);
                }
            }
        } else {
            call_on_intersecting(node.left, cld, on_intersection, not_intersection);
            call_on_intersecting(node.right, cld, on_intersection, not_intersection);
        }
    }

public:
    void reset()
    {
        nodes.clear();
        data.clear();
        accu_center_volume.reset();
        accu_volume.reset();
    }

    void add(Data&& d)
    {
        accu_center_volume.put_without_update(d.box.get_center());
        accu_volume.put_without_update(d.box);
        data.push_back(std::move(d));
    }

    void create_nodes()
    {
        if (data.empty()) {
            return;
        }
        nodes.reserve(data.size() * (sizeof(Node) + sizeof(Data)) * 2);
        accu_volume.update();
        accu_center_volume.update();
        create_node(0, static_cast<index_t>(data.size()), accu_center_volume, accu_volume);
    }

    [[nodiscard]] std::optional<std::pair<core::fp_t, const Data*>> hit(const math::Ray3<core::fp_t>& ray, const core::fp_t minimum_distance) const
    {
        if (nodes.empty()) {
            return std::nullopt;
        }
        const auto ro = ray.get_origin().to<float>();
        const auto rrd = ray.get_reversed_normalized_direction().to<float>();
        if (!node_hit(node_at(0), ro, rrd, static_cast<float>(minimum_distance))) {
            return std::nullopt;
        }
        return hit(0, ray, ro, rrd, minimum_distance);
    }

    template <typename Collider, typename Function>
    void call_on_intersecting(const Collider& cld, Function&& on_intersection)
    {
        if (nodes.empty()) {
            return;
        }
        call_on_intersecting(static_cast<index_t>(0), cld, std::forward<Function>(on_intersection));
    }

    template <typename Collider, typename Function, typename ElseFunction>
    void call_on_intersecting(const Collider& cld, Function&& on_intersection, ElseFunction&& not_intersection)
    {
        if (nodes.empty()) {
            return;
        }
        call_on_intersecting(static_cast<index_t>(0), cld, std::forward<Function>(on_intersection), std::forward<ElseFunction>(not_intersection));
    }

    template <typename Function>
    void call_on_all(Function&& function)
    {
        if (nodes.empty()) {
            return;
        }
        call_on_all(static_cast<index_t>(0), std::forward<Function>(function));
    }
};
}