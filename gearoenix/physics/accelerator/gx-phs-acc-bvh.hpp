#ifndef GEAROENIX_PHYSICS_ACCELERATOR_BVH_HPP
#define GEAROENIX_PHYSICS_ACCELERATOR_BVH_HPP
#include "../../core/macro/gx-cr-mcr-assert.hpp"
#include "../../math/gx-math-aabb.hpp"
#include "../../math/gx-math-ray.hpp"
#include <array>
#include <boost/container/static_vector.hpp>
#include <functional>
#include <optional>
#include <string>
#include <utility>
#include <vector>

namespace gearoenix::physics::accelerator {
template <typename UserData>
struct Bvh final {
    constexpr static const std::size_t local_data_size = 5;
    constexpr static const std::size_t bins_count = 32;
    constexpr static const std::size_t walls_count = bins_count - 1;

    static_assert(bins_count > 4 && bins_count % 2 == 0, "Bins count is unexpected.");
    static_assert(std::is_trivially_destructible_v<UserData>, "Only trivially destructable data is accepted.");

    struct Data final {
        math::Aabb3<double> box;
        UserData user_data;
    };

private:
    struct Bin final {
        math::Aabb3<double> volume;
        std::vector<Data> data;
    };

    struct Leaf final {
        const bool leaf = true;
        math::Aabb3<double> volume;
        std::size_t data_count; // there is an array of data after this
    };

    struct Node final {
        const bool leaf = false;
        math::Aabb3<double> volume;
        std::size_t left = static_cast<std::size_t>(-1);
        std::size_t right = static_cast<std::size_t>(-1);
    };

    std::vector<std::uint8_t> nodes;
    math::Aabb3<double> accu_center_volume;
    math::Aabb3<double> accu_volume;
    std::array<Bin, bins_count> bins;
    std::vector<Data> data;

    template <typename N>
    N& allocate() noexcept
    {
        const auto i = nodes.size();
        nodes.resize(i + sizeof(N));
        auto* n = new (reinterpret_cast<N*>(&nodes[i])) N();
        return *n;
    }

    void create_node(
        const std::size_t data_starting_index,
        const std::size_t data_ending_index,
        const math::Aabb3<double>& center_volume,
        const math::Aabb3<double>& volume) noexcept
    {
        const auto data_size = data_ending_index - data_starting_index;
        GX_ASSERT_D(data_size > 0);
        if (data_size <= local_data_size) {
            auto& l = allocate<Leaf>();
            l.volume = volume;
            l.data_count = data_size;
            for (auto data_index = data_starting_index; data_index < data_ending_index; ++data_index)
                allocate<Data>() = data[data_index];
            return;
        }
        /// Sorting dimensions
        const auto& dimensions = center_volume.get_diameter();
        std::array<std::pair<std::size_t, double>, 3> dimensions_indices;
        for (std::size_t dimension_index = 0; dimension_index < 3; ++dimension_index) {
            dimensions_indices[dimension_index] = std::make_pair(dimension_index, dimensions[dimension_index]);
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
        /// Searching for dimension with a good split
        for (const auto& dimension_index : dimensions_indices) {
            /// Clearing bins
            for (auto& bin : bins) {
                bin.volume.reset();
                bin.data.clear();
            }
            const auto split_size = dimension_index.second / static_cast<double>(bins_count);
            std::array<double, walls_count> splits = { 0.0 };
            auto wall = volume.get_lower()[dimension_index.first];
            for (auto& s : splits) {
                wall += split_size;
                s = wall;
            }
            for (auto data_index = data_starting_index; data_index < data_ending_index; ++data_index) {
                const auto& d = data[data_index];
                bool not_found = true;
                for (std::size_t wall_index = 0; wall_index < walls_count; ++wall_index) {
                    if (d.box.get_center()[dimension_index.first] < splits[wall_index]) {
                        auto& bin = bins[wall_index];
                        bin.data.push_back(d);
                        bin.volume.put_without_update(d.box);
                        not_found = false;
                        break;
                    }
                }
                if (not_found) {
                    auto& bin = bins[walls_count];
                    bin.data.push_back(d);
                    bin.volume.put_without_update(d.box);
                }
            }
            /// Overlapping centers check
            bool overlapped = false;
            for (const auto& b : bins) {
                if (b.data.size() == data_size) {
                    overlapped = true;
                    break;
                } else if (!b.data.empty()) {
                    break;
                }
            }
            if (overlapped) {
                continue;
            }
            auto best_wall_cost = std::numeric_limits<double>::max();
            auto best_wall_index = static_cast<std::size_t>(-1);
            math::Aabb3<double> best_wall_left_box;
            math::Aabb3<double> best_wall_right_box;
            math::Aabb3<double> left_box;
            std::size_t left_count = 0;
            for (std::size_t wall_index = 0; wall_index < walls_count; ++wall_index) {
                const auto& left_bin = bins[wall_index];
                left_count += left_bin.data.size();
                if (0 >= left_count)
                    continue;
                left_box.put(left_bin.volume);
                const auto right_count = data_size - left_count;
                if (0 >= right_count)
                    break;
                math::Aabb3<double> right_box = bins[wall_index + 1].volume;
                for (auto right_split_indx = wall_index + 2; right_split_indx < bins_count; ++right_split_indx) {
                    right_box.put_without_update(bins[right_split_indx].volume);
                }
                right_box.update();
                const auto cost = left_box.get_volume() * static_cast<double>(left_count) + right_box.get_volume() * static_cast<double>(right_count);
                if (cost < best_wall_cost) {
                    best_wall_cost = cost;
                    best_wall_index = wall_index;
                    best_wall_left_box = left_box;
                    best_wall_right_box = right_box;
                }
            }

            for (auto& bin : bins) {
                bin.volume.update();
            }

            ++best_wall_index;

            const std::size_t left_data_starting_index = data.size();
            math::Aabb3<double> left_center_volume;
            for (std::size_t wall_index = 0; wall_index < best_wall_index; ++wall_index) {
                const auto& bin = bins[wall_index];
                left_center_volume.put_without_update(bin.volume.get_center());
                for (auto& d : bin.data) {
                    data.push_back(d);
                }
            }
            left_center_volume.update();
            const std::size_t left_data_ending_index = data.size();

            const std::size_t right_data_starting_index = data.size();
            math::Aabb3<double> right_center_volume;
            for (auto wall_index = best_wall_index; wall_index < bins_count; ++wall_index) {
                const auto& bin = bins[wall_index];
                right_center_volume.put_without_update(bin.volume.get_center());
                for (auto& d : bin.data) {
                    data.push_back(d);
                }
            }
            right_center_volume.update();
            const std::size_t right_data_ending_index = data.size();

            const auto current_node_index = nodes.size();
            auto& current_node = allocate<Node>();
            current_node.volume = volume;

            current_node.left = nodes.size();

            create_node(
                left_data_starting_index,
                left_data_ending_index,
                left_center_volume,
                best_wall_left_box);

            reinterpret_cast<Node*>(&nodes[current_node_index])->right = nodes.size();

            create_node(
                right_data_starting_index,
                right_data_ending_index,
                right_center_volume,
                best_wall_right_box);

            return;
        }

        auto& l = allocate<Leaf>();
        l.volume = volume;
        l.data_count = data_size;
        for (auto data_index = data_starting_index; data_index < data_ending_index; ++data_index)
            allocate<Data>() = data[data_index];
    }

    [[nodiscard]] std::optional<std::pair<double, const Data*>> hit(
        const std::size_t ptr,
        const math::Ray3<double>& ray,
        const double minimum_distance) const noexcept
    {
        const auto& node = *reinterpret_cast<const Node*>(ptr);
        auto hit_result = node.volume.hit(ray, minimum_distance);
        if (!hit_result.has_value())
            return std::nullopt;
        if (node.leaf) {
            const auto data_size = node.left;
            const auto min_dis = minimum_distance;
            const Data* min_data = nullptr;
            for (
                std::size_t data_index = 0, data_ptr = ptr + sizeof(Leaf);
                data_index < data_size;
                ++data_index, data_ptr += sizeof(Data)) {
                const auto* const d = reinterpret_cast<const Data*>(data_ptr);
                const auto h = d->volume.hit(ray, min_dis);
                if (h.has_value()) {
                    min_dis = *h;
                    min_data = d;
                }
            }
            if (nullptr == min_data)
                return std::nullopt;
            return std::make_pair(min_dis, min_data);
        }
        hit_result = hit(reinterpret_cast<std::size_t>(&nodes[node.left]), ray, minimum_distance);
        const auto min_dis = hit_result.has_value() ? hit_result->first : minimum_distance;
        const auto right_hit_result = hit(reinterpret_cast<std::size_t>(&nodes[node.right]), ray, min_dis);
        return right_hit_result.has_value() ? right_hit_result : hit_result;
    }

    template <typename Function>
    void call_on_all(const std::size_t ptr, Function&& function) noexcept
    {
        const auto& node = *reinterpret_cast<const Node*>(ptr);
        if (node.leaf) {
            const auto data_size = node.left;
            for (std::size_t data_index = 0, data_ptr = ptr + sizeof(Leaf);
                 data_index < data_size;
                 ++data_index, data_ptr += sizeof(Data)) {
                function(*reinterpret_cast<Data*>(data_ptr));
            }
        } else {
            call_on_all(reinterpret_cast<std::size_t>(&nodes[node.left]), function);
            call_on_all(reinterpret_cast<std::size_t>(&nodes[node.right]), function);
        }
    }

    template <typename Collider, typename Function>
    void call_on_intersecting(const std::size_t ptr, const Collider& cld, Function&& on_intersection) noexcept
    {
        const auto& node = *reinterpret_cast<const Node*>(ptr);
        const auto intersection_status = cld.check_intersection_status(node.volume);
        if (math::IntersectionStatus::Out == intersection_status)
            return;
        if (math::IntersectionStatus::In == intersection_status) {
            call_on_all(ptr, on_intersection);
            return;
        }
        if (node.leaf) {
            const auto data_size = node.left;
            for (std::size_t data_index = 0, data_ptr = ptr + sizeof(Leaf);
                 data_index < data_size;
                 ++data_index, data_ptr += sizeof(Data)) {
                auto& d = *reinterpret_cast<Data*>(data_ptr);
                const auto is = cld.check_intersection_status(d.box);
                if (math::IntersectionStatus::Out == is)
                    continue;
                on_intersection(d);
            }
        } else {
            call_on_intersecting(reinterpret_cast<std::size_t>(&nodes[node.left]), cld, on_intersection);
            call_on_intersecting(reinterpret_cast<std::size_t>(&nodes[node.right]), cld, on_intersection);
        }
    }

public:
    void reset() noexcept
    {
        nodes.clear();
        data.clear();
        accu_center_volume.reset();
        accu_volume.reset();
    }

    void add(Data&& d) noexcept
    {
        accu_center_volume.put_without_update(d.box.get_center());
        accu_volume.put_without_update(d.box);
        data.push_back(std::move(d));
    }

    void create_nodes() noexcept
    {
        if (data.empty())
            return;
        accu_volume.update();
        accu_center_volume.update();
        create_node(0, data.size(), accu_center_volume, accu_volume);
    }

    [[nodiscard]] std::optional<std::pair<double, Data*>> hit(const math::Ray3<double>& ray, const double minimum_distance) const noexcept
    {
        if (nodes.empty())
            return std::nullopt;
        return hit(reinterpret_cast<std::size_t>(&nodes[0]), ray, minimum_distance);
    }

    template <typename Collider, typename Function>
    void call_on_intersecting(const Collider& cld, Function&& on_intersection) noexcept
    {
        if (nodes.empty())
            return;
        call_on_intersecting(reinterpret_cast<std::size_t>(&nodes[0]), cld, on_intersection);
    }
};
}
#endif
