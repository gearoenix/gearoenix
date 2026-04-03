#pragma once
#include "../../math/gx-math-aabb.hpp"
#include "../../math/gx-math-triangle.hpp"
#include "../../math/gx-math-vector-3d.hpp"
#include <memory>
#include <vector>

namespace gearoenix::physics::collider {
struct Mesh final {
private:
    std::vector<std::uint32_t> indices;
    std::vector<math::Vec3<core::fp_t>> vertices;
    std::vector<math::Triangle3<core::fp_t>> ts;
    math::Aabb3<core::fp_t> box;

public:
    [[nodiscard]] std::optional<core::fp_t> hit(const math::Ray3<core::fp_t>& ray, core::fp_t d_min) const;

    friend std::ostream& operator<<(std::ostream& os, const Mesh& m)
    {
        os << "{ 'object_class_name' : 'gearoenix::physics::collider::Mesh', 'triangles' : [";
        for (const math::Triangle3<core::fp_t>& t : m.ts) {
            os << " " << t << ",";
        }
        os << " ] }";
        return os;
    }
};
}