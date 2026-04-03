#pragma once
#include "../../math/gx-math-frustum.hpp"
#include "../../math/gx-math-ray.hpp"
#include "gx-phs-acc-bvh.hpp"

BOOST_AUTO_TEST_CASE(gearoenix_accelerator_bvh_frustum)
{
    using namespace gearoenix;

    struct Data final {
        int id = 0;
    };
    using Box = math::Aabb3<core::fp_t>;
    using V2 = math::Vec2<core::fp_t>;
    using V3 = math::Vec3<core::fp_t>;
    using Frustum = math::Frustum<core::fp_t>;

    physics::accelerator::Bvh<Data> bvh;

    bvh.add({ Box(V3(1.0), V3(-1.0)), Data { 0 } });

    bvh.create_nodes();

    bvh.reset();

    for (int i = 0; i < 11; ++i) {
        for (int j = 0; j < 11; ++j) {
            const V3 p(V2(static_cast<core::fp_t>(i), static_cast<core::fp_t>(j)) * 3.0 - 15.0, 0.0);
            bvh.add({ Box(p + 1.0, p - 1.0), Data { (i << 8) | j } });
        }
    }

    bvh.create_nodes();

    Frustum frustum(std::array<V3, 8> {
        V3(-15, 15.0, -1.0), V3(15, 15.0, -1.0), V3(-15, -15.0, -1.0), V3(15.0, -15.0, -1.0), // 0 --------
        V3(-15, 15.0, 1.0), V3(15, 15.0, 1.0), V3(-15, -15.0, 1.0), V3(15.0, -15.0, 1.0) // 1 -------------
    });

    BOOST_TEST(frustum.check_intersection(Box(V3(1.0, 1.0, 0.5), V3(-1.0, -1.0, -0.5))));
    BOOST_TEST(frustum.check_intersection(Box(V3(20.0, 20.0, 2.5), V3(-20.0, -20.0, -2.5))));

    int count = 0;
    bvh.call_on_intersecting(frustum, [&](auto&) { ++count; });
    BOOST_TEST(count == 11 * 11);

    frustum = Frustum(std::array<V3, 8> {
        V3(-10, 10.0, -0.5), V3(10, 10.0, -0.5), V3(-10.0, -10.0, -0.5), V3(10.0, -10.0, -0.5), // 0 --------
        V3(-10, 10.0, 0.5), V3(10, 10.0, 0.5), V3(-10.0, -10.0, 0.5), V3(10.0, -10.0, 0.5) // 1 -------------
    });

    BOOST_TEST(!frustum.check_intersection(Box(V3(-11.0, -11.0, 1.0), V3(-13.0, -13.0, -1.0))));

    count = 0;
    bvh.call_on_intersecting(frustum, [&](auto& d) {
        ++count;
        const int i = d.user_data.id & 255;
        const int j = (d.user_data.id >> 8) & 255;
        const bool b = i > 1 && i < 9 && j > 1 && j < 9;
        BOOST_TEST(b);
    });
    BOOST_TEST(count == 7 * 7);
}

BOOST_AUTO_TEST_CASE(gearoenix_accelerator_bvh_ray_hit)
{
    using namespace gearoenix;

    struct Data final {
        int id = 0;
    };
    using Box = math::Aabb3<core::fp_t>;
    using V3 = math::Vec3<core::fp_t>;
    using Ray = math::Ray3<core::fp_t>;

    physics::accelerator::Bvh<Data> bvh;

    /// Place 5 boxes along the +Z axis at z = 2, 4, 6, 8, 10
    for (int i = 0; i < 5; ++i) {
        const core::fp_t z = 2.0 + 2.0 * static_cast<core::fp_t>(i);
        bvh.add({ Box(V3(1.0, 1.0, z + 0.5), V3(-1.0, -1.0, z - 0.5)), Data { i } });
    }
    bvh.create_nodes();

    /// Ray from origin along +Z should hit the nearest box (id=0 at z=1.5)
    {
        const Ray ray(V3(0.0), V3(0.0, 0.0, 1.0));
        const auto result = bvh.hit(ray, 1000.0);
        BOOST_TEST(result.has_value());
        BOOST_TEST(result->second->user_data.id == 0);
        BOOST_TEST(result->first >= 1.4);
        BOOST_TEST(result->first <= 1.6);
    }

    /// Ray from far +Z along -Z should hit the farthest box (id=4 at z=10.5)
    {
        const Ray ray(V3(0.0, 0.0, 100.0), V3(0.0, 0.0, -1.0));
        const auto result = bvh.hit(ray, 1000.0);
        BOOST_TEST(result.has_value());
        BOOST_TEST(result->second->user_data.id == 4);
    }

    /// Ray along +X at y=0, z=0 misses all boxes (boxes are at z >= 1.5)
    {
        const Ray ray(V3(-100.0, 0.0, 0.0), V3(1.0, 0.0, 0.0));
        const auto result = bvh.hit(ray, 1000.0);
        BOOST_TEST(!result.has_value());
    }

    /// Ray with limited distance: should only hit if first box is within range
    {
        const Ray ray(V3(0.0), V3(0.0, 0.0, 1.0));
        const auto result_short = bvh.hit(ray, 1.0);
        BOOST_TEST(!result_short.has_value());
        const auto result_long = bvh.hit(ray, 2.0);
        BOOST_TEST(result_long.has_value());
    }

    /// Empty BVH hit returns nullopt
    {
        physics::accelerator::Bvh<Data> empty_bvh;
        const Ray ray(V3(0.0), V3(0.0, 0.0, 1.0));
        BOOST_TEST(!empty_bvh.hit(ray, 1000.0).has_value());
    }
}

BOOST_AUTO_TEST_CASE(gearoenix_accelerator_bvh_ray_nearest)
{
    using namespace gearoenix;

    struct Data final {
        int id = 0;
    };
    using Box = math::Aabb3<core::fp_t>;
    using V3 = math::Vec3<core::fp_t>;
    using Ray = math::Ray3<core::fp_t>;

    /// Place two overlapping boxes at different depths — verify we always get the nearest
    physics::accelerator::Bvh<Data> bvh;
    bvh.add({ Box(V3(1.0, 1.0, 3.0), V3(-1.0, -1.0, 2.0)), Data { 10 } });
    bvh.add({ Box(V3(1.0, 1.0, 6.0), V3(-1.0, -1.0, 5.0)), Data { 20 } });
    bvh.create_nodes();

    const Ray ray(V3(0.0), V3(0.0, 0.0, 1.0));
    const auto result = bvh.hit(ray, 1000.0);
    BOOST_TEST(result.has_value());
    BOOST_TEST(result->second->user_data.id == 10);
    BOOST_TEST(result->first >= 1.9);
    BOOST_TEST(result->first <= 2.1);

    /// Reverse direction: nearest is id=20
    const Ray reverse_ray(V3(0.0, 0.0, 100.0), V3(0.0, 0.0, -1.0));
    const auto reverse_result = bvh.hit(reverse_ray, 1000.0);
    BOOST_TEST(reverse_result.has_value());
    BOOST_TEST(reverse_result->second->user_data.id == 20);
}

BOOST_AUTO_TEST_CASE(gearoenix_accelerator_bvh_call_on_all)
{
    using namespace gearoenix;

    struct Data final {
        int id = 0;
    };
    using Box = math::Aabb3<core::fp_t>;
    using V3 = math::Vec3<core::fp_t>;

    physics::accelerator::Bvh<Data> bvh;
    constexpr int n = 100;
    std::array<bool, n> visited = { };

    for (int i = 0; i < n; ++i) {
        const core::fp_t x = static_cast<core::fp_t>(i) * 3.0;
        bvh.add({ Box(V3(x + 1.0, 1.0, 1.0), V3(x - 1.0, -1.0, -1.0)), Data { i } });
    }
    bvh.create_nodes();

    int count = 0;
    bvh.call_on_all([&](const auto& d) {
        BOOST_TEST(d.user_data.id >= 0);
        BOOST_TEST(d.user_data.id < n);
        visited[d.user_data.id] = true;
        ++count;
    });

    BOOST_TEST(count == n);
    for (int i = 0; i < n; ++i) {
        BOOST_TEST(visited[i]);
    }
}

BOOST_AUTO_TEST_CASE(gearoenix_accelerator_bvh_intersecting_with_miss)
{
    using namespace gearoenix;

    struct Data final {
        int id = 0;
    };
    using Box = math::Aabb3<core::fp_t>;
    using V3 = math::Vec3<core::fp_t>;
    using Frustum = math::Frustum<core::fp_t>;

    physics::accelerator::Bvh<Data> bvh;

    /// Create a 1D row of boxes along X: 10 boxes, spaced at x = 0, 5, 10, ..., 45
    for (int i = 0; i < 10; ++i) {
        const core::fp_t x = static_cast<core::fp_t>(i) * 5.0;
        bvh.add({ Box(V3(x + 1.0, 1.0, 1.0), V3(x - 1.0, -1.0, -1.0)), Data { i } });
    }
    bvh.create_nodes();

    /// Frustum that covers only x in [-2, 12], should hit boxes 0, 1, 2 and miss 3-9
    const Frustum frustum(std::array<V3, 8> {
        V3(-2.0, 2.0, -2.0), V3(12.0, 2.0, -2.0), V3(-2.0, -2.0, -2.0), V3(12.0, -2.0, -2.0),
        V3(-2.0, 2.0, 2.0), V3(12.0, 2.0, 2.0), V3(-2.0, -2.0, 2.0), V3(12.0, -2.0, 2.0) });

    int hit_count = 0;
    int miss_count = 0;
    bvh.call_on_intersecting(
        frustum,
        [&](auto& d) {
            ++hit_count;
            BOOST_TEST(d.user_data.id <= 2);
        },
        [&](auto& d) {
            ++miss_count;
            BOOST_TEST(d.user_data.id >= 3);
        });

    BOOST_TEST(hit_count == 3);
    BOOST_TEST(miss_count == 7);
}

BOOST_AUTO_TEST_CASE(gearoenix_accelerator_bvh_sah_quality)
{
    using namespace gearoenix;

    /// Validate that SAH produces a reasonable tree by checking that ray queries
    /// against a grid return correct results regardless of insertion order.
    struct Data final {
        int id = 0;
    };
    using Box = math::Aabb3<core::fp_t>;
    using V3 = math::Vec3<core::fp_t>;
    using Ray = math::Ray3<core::fp_t>;

    /// Build a 10x10 grid of boxes on the XY plane
    physics::accelerator::Bvh<Data> bvh;
    constexpr int grid = 10;
    for (int i = 0; i < grid; ++i) {
        for (int j = 0; j < grid; ++j) {
            const core::fp_t x = static_cast<core::fp_t>(i) * 3.0;
            const core::fp_t y = static_cast<core::fp_t>(j) * 3.0;
            bvh.add({ Box(V3(x + 0.5, y + 0.5, 0.5), V3(x - 0.5, y - 0.5, -0.5)), Data { i * grid + j } });
        }
    }
    bvh.create_nodes();

    /// Cast a ray down onto each grid cell and verify it hits the correct box
    for (int i = 0; i < grid; ++i) {
        for (int j = 0; j < grid; ++j) {
            const core::fp_t x = static_cast<core::fp_t>(i) * 3.0;
            const core::fp_t y = static_cast<core::fp_t>(j) * 3.0;
            const Ray ray(V3(x, y, 100.0), V3(0.0, 0.0, -1.0));
            const auto result = bvh.hit(ray, 1000.0);
            BOOST_TEST(result.has_value());
            if (result.has_value()) {
                BOOST_TEST(result->second->user_data.id == i * grid + j);
            }
        }
    }

    /// Cast rays that miss between grid cells
    for (int i = 0; i < grid - 1; ++i) {
        for (int j = 0; j < grid - 1; ++j) {
            const core::fp_t x = static_cast<core::fp_t>(i) * 3.0 + 1.5;
            const core::fp_t y = static_cast<core::fp_t>(j) * 3.0 + 1.5;
            const Ray ray(V3(x, y, 100.0), V3(0.0, 0.0, -1.0));
            const auto result = bvh.hit(ray, 1000.0);
            BOOST_TEST(!result.has_value());
        }
    }
}

BOOST_AUTO_TEST_CASE(gearoenix_accelerator_bvh_sah_depth_layers)
{
    using namespace gearoenix;

    /// Build boxes at different Z depths with X/Y overlap to verify SAH splits
    /// correctly along the axis with the most spread, and that ray queries
    /// still find the nearest hit through multiple BVH levels.
    struct Data final {
        int id = 0;
    };
    using Box = math::Aabb3<core::fp_t>;
    using V3 = math::Vec3<core::fp_t>;
    using Ray = math::Ray3<core::fp_t>;

    physics::accelerator::Bvh<Data> bvh;

    /// 4 layers of 4 boxes each, all overlapping in XY but at different Z
    for (int layer = 0; layer < 4; ++layer) {
        for (int i = 0; i < 4; ++i) {
            const core::fp_t x = static_cast<core::fp_t>(i) * 3.0;
            const core::fp_t z = static_cast<core::fp_t>(layer) * 5.0;
            bvh.add({ Box(V3(x + 1.0, 1.0, z + 1.0), V3(x - 1.0, -1.0, z - 1.0)), Data { layer * 4 + i } });
        }
    }
    bvh.create_nodes();

    /// Ray going along +Z from below should hit layer 0 first
    for (int i = 0; i < 4; ++i) {
        const core::fp_t x = static_cast<core::fp_t>(i) * 3.0;
        const Ray ray(V3(x, 0.0, -100.0), V3(0.0, 0.0, 1.0));
        const auto result = bvh.hit(ray, 1000.0);
        BOOST_TEST(result.has_value());
        if (result.has_value()) {
            BOOST_TEST(result->second->user_data.id == i);
        }
    }

    /// Ray going along -Z from above should hit layer 3 first
    for (int i = 0; i < 4; ++i) {
        const core::fp_t x = static_cast<core::fp_t>(i) * 3.0;
        const Ray ray(V3(x, 0.0, 100.0), V3(0.0, 0.0, -1.0));
        const auto result = bvh.hit(ray, 1000.0);
        BOOST_TEST(result.has_value());
        if (result.has_value()) {
            BOOST_TEST(result->second->user_data.id == 3 * 4 + i);
        }
    }
}

BOOST_AUTO_TEST_CASE(gearoenix_accelerator_bvh_single_element)
{
    using namespace gearoenix;

    /// Edge case: BVH with exactly 1 element
    struct Data final {
        int id = 0;
    };
    using Box = math::Aabb3<core::fp_t>;
    using V3 = math::Vec3<core::fp_t>;
    using Ray = math::Ray3<core::fp_t>;

    physics::accelerator::Bvh<Data> bvh;
    bvh.add({ Box(V3(1.0), V3(-1.0)), Data { 42 } });
    bvh.create_nodes();

    /// Ray that hits the single box
    {
        const Ray ray(V3(0.0, 0.0, -10.0), V3(0.0, 0.0, 1.0));
        const auto result = bvh.hit(ray, 1000.0);
        BOOST_TEST(result.has_value());
        if (result.has_value()) {
            BOOST_TEST(result->second->user_data.id == 42);
        }
    }

    /// Ray that misses
    {
        const Ray ray(V3(5.0, 0.0, -10.0), V3(0.0, 0.0, 1.0));
        const auto result = bvh.hit(ray, 1000.0);
        BOOST_TEST(!result.has_value());
    }

    int count = 0;
    bvh.call_on_all([&](const auto&) { ++count; });
    BOOST_TEST(count == 1);
}

BOOST_AUTO_TEST_CASE(gearoenix_accelerator_bvh_leaf_boundary)
{
    using namespace gearoenix;

    /// Test with exactly local_data_size elements (should create a single leaf)
    /// and local_data_size + 1 (should force a split)
    struct Data final {
        int id = 0;
    };
    using Box = math::Aabb3<core::fp_t>;
    using V3 = math::Vec3<core::fp_t>;
    using Ray = math::Ray3<core::fp_t>;

    /// Exactly local_data_size = 5 items → single leaf node
    {
        physics::accelerator::Bvh<Data> bvh;
        for (int i = 0; i < 5; ++i) {
            const core::fp_t x = static_cast<core::fp_t>(i) * 4.0;
            bvh.add({ Box(V3(x + 1.0, 1.0, 1.0), V3(x - 1.0, -1.0, -1.0)), Data { i } });
        }
        bvh.create_nodes();

        for (int i = 0; i < 5; ++i) {
            const core::fp_t x = static_cast<core::fp_t>(i) * 4.0;
            const Ray ray(V3(x, 0.0, 10.0), V3(0.0, 0.0, -1.0));
            const auto result = bvh.hit(ray, 1000.0);
            BOOST_TEST(result.has_value());
            if (result.has_value()) {
                BOOST_TEST(result->second->user_data.id == i);
            }
        }
    }

    /// local_data_size + 1 = 6 items → must split
    {
        physics::accelerator::Bvh<Data> bvh;
        for (int i = 0; i < 6; ++i) {
            const core::fp_t x = static_cast<core::fp_t>(i) * 4.0;
            bvh.add({ Box(V3(x + 1.0, 1.0, 1.0), V3(x - 1.0, -1.0, -1.0)), Data { i } });
        }
        bvh.create_nodes();

        for (int i = 0; i < 6; ++i) {
            const core::fp_t x = static_cast<core::fp_t>(i) * 4.0;
            const Ray ray(V3(x, 0.0, 10.0), V3(0.0, 0.0, -1.0));
            const auto result = bvh.hit(ray, 1000.0);
            BOOST_TEST(result.has_value());
            if (result.has_value()) {
                BOOST_TEST(result->second->user_data.id == i);
            }
        }

        int count = 0;
        bvh.call_on_all([&](const auto&) { ++count; });
        BOOST_TEST(count == 6);
    }
}

BOOST_AUTO_TEST_CASE(gearoenix_accelerator_bvh_coincident_centers)
{
    using namespace gearoenix;

    /// All boxes have the same center — SAH cannot split by center spread,
    /// so the BVH should fall back to creating a leaf.
    struct Data final {
        int id = 0;
    };
    using Box = math::Aabb3<core::fp_t>;
    using V3 = math::Vec3<core::fp_t>;
    using Ray = math::Ray3<core::fp_t>;

    physics::accelerator::Bvh<Data> bvh;
    for (int i = 0; i < 20; ++i) {
        const core::fp_t half = 1.0 + static_cast<core::fp_t>(i) * 0.1;
        bvh.add({ Box(V3(half), V3(-half)), Data { i } });
    }
    bvh.create_nodes();

    /// Ray through the common center should hit something
    {
        const Ray ray(V3(0.0, 0.0, -100.0), V3(0.0, 0.0, 1.0));
        const auto result = bvh.hit(ray, 1000.0);
        BOOST_TEST(result.has_value());
    }

    /// All items should be visitable
    int count = 0;
    bvh.call_on_all([&](const auto&) { ++count; });
    BOOST_TEST(count == 20);
}

BOOST_AUTO_TEST_CASE(gearoenix_accelerator_bvh_diagonal_ray)
{
    using namespace gearoenix;

    /// Test diagonal rays hitting a 3D grid to exercise multi-axis SAH splits
    struct Data final {
        int id = 0;
    };
    using Box = math::Aabb3<core::fp_t>;
    using V3 = math::Vec3<core::fp_t>;
    using Ray = math::Ray3<core::fp_t>;

    physics::accelerator::Bvh<Data> bvh;
    int id = 0;
    constexpr int grid = 5;
    constexpr core::fp_t spacing = 4.0;

    for (int x = 0; x < grid; ++x) {
        for (int y = 0; y < grid; ++y) {
            for (int z = 0; z < grid; ++z) {
                const V3 center(
                    static_cast<core::fp_t>(x) * spacing,
                    static_cast<core::fp_t>(y) * spacing,
                    static_cast<core::fp_t>(z) * spacing);
                bvh.add({ Box(center + 0.5, center - 0.5), Data { id++ } });
            }
        }
    }
    bvh.create_nodes();

    BOOST_TEST(id == grid * grid * grid);

    /// Diagonal ray from corner towards the center of the grid
    const V3 grid_center(
        static_cast<core::fp_t>(grid - 1) * spacing * 0.5,
        static_cast<core::fp_t>(grid - 1) * spacing * 0.5,
        static_cast<core::fp_t>(grid - 1) * spacing * 0.5);
    const V3 origin = grid_center - V3(100.0);
    const V3 direction = (grid_center - origin).normalised();
    const Ray ray(origin, direction);

    const auto result = bvh.hit(ray, 10000.0);
    BOOST_TEST(result.has_value());

    /// The hit box center should be the corner box (0,0,0) since it's nearest along the diagonal
    if (result.has_value()) {
        const auto& hit_box = result->second->box;
        const auto hit_center = hit_box.get_center();
        BOOST_TEST(hit_center.x >= -0.6);
        BOOST_TEST(hit_center.x <= 0.6);
        BOOST_TEST(hit_center.y >= -0.6);
        BOOST_TEST(hit_center.y <= 0.6);
        BOOST_TEST(hit_center.z >= -0.6);
        BOOST_TEST(hit_center.z <= 0.6);
    }

    /// Verify total count
    int count = 0;
    bvh.call_on_all([&](const auto&) { ++count; });
    BOOST_TEST(count == grid * grid * grid);
}

BOOST_AUTO_TEST_CASE(gearoenix_accelerator_bvh_reset_and_rebuild)
{
    using namespace gearoenix;

    /// Verify reset + rebuild works correctly
    struct Data final {
        int id = 0;
    };
    using Box = math::Aabb3<core::fp_t>;
    using V3 = math::Vec3<core::fp_t>;
    using Ray = math::Ray3<core::fp_t>;

    physics::accelerator::Bvh<Data> bvh;

    /// First build: 3 boxes along X
    for (int i = 0; i < 3; ++i) {
        const core::fp_t x = static_cast<core::fp_t>(i) * 5.0;
        bvh.add({ Box(V3(x + 1.0, 1.0, 1.0), V3(x - 1.0, -1.0, -1.0)), Data { i } });
    }
    bvh.create_nodes();

    {
        const Ray ray(V3(0.0, 0.0, 10.0), V3(0.0, 0.0, -1.0));
        const auto result = bvh.hit(ray, 1000.0);
        BOOST_TEST(result.has_value());
        BOOST_TEST(result->second->user_data.id == 0);
    }

    /// Reset and rebuild with different data
    bvh.reset();

    for (int i = 0; i < 3; ++i) {
        const core::fp_t y = static_cast<core::fp_t>(i) * 5.0;
        bvh.add({ Box(V3(1.0, y + 1.0, 1.0), V3(-1.0, y - 1.0, -1.0)), Data { 100 + i } });
    }
    bvh.create_nodes();

    {
        const Ray ray(V3(0.0, 0.0, 10.0), V3(0.0, 0.0, -1.0));
        const auto result = bvh.hit(ray, 1000.0);
        BOOST_TEST(result.has_value());
        BOOST_TEST(result->second->user_data.id == 100);
    }

    /// Old data should not be accessible
    {
        const Ray ray(V3(10.0, 0.0, 10.0), V3(0.0, 0.0, -1.0));
        const auto result = bvh.hit(ray, 1000.0);
        BOOST_TEST(!result.has_value());
    }
}
