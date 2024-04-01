#ifndef GEAROENIX_PHYSICS_COLLIDER_MESH_HPP
#define GEAROENIX_PHYSICS_COLLIDER_MESH_HPP
#include "../../core/gx-cr-types.hpp"
#include "../../math/gx-math-aabb.hpp"
#include "../../math/gx-math-triangle.hpp"
#include "../../math/gx-math-vector-3d.hpp"
#include <memory>
#include <vector>

namespace gearoenix::physics::collider {
struct Mesh final {
private:
    std::vector<std::uint32_t> indices;
    std::vector<math::Vec3<double>> vertices;
    std::vector<math::Triangle3<double>> ts;
    math::Aabb3<double> box;

public:
    [[nodiscard]] std::optional<double> hit(const math::Ray3<double>& ray, double d_min) const;

    friend std::ostream& operator<<(std::ostream& os, const Mesh& m)
    {
        os << "{ 'object_class_name' : 'gearoenix::physics::collider::Mesh', 'triangles' : [";
        for (const math::Triangle3<double>& t : m.ts) {
            os << " " << t << ",";
        }
        os << " ] }";
        return os;
    }
};
}
#endif
