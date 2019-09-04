#ifndef GEAROENIX_PHYSICS_COLLIDER_MESH_HPP
#define GEAROENIX_PHYSICS_COLLIDER_MESH_HPP
#include "../../core/cr-types.hpp"
#include "../../math/math-aabb.hpp"
#include "../../math/math-triangle.hpp"
#include "../../math/math-vector.hpp"
#include "phs-cld-collider.hpp"
#include <memory>
#include <vector>
namespace gearoenix {
namespace system {
    class File;
}
namespace physics::collider {
    class Mesh : public Collider {
    private:
        std::vector<std::uint32_t> indices;
        std::vector<math::Vec3> vertices;
        std::vector<math::Triangle3> ts;
        math::Aabb3 box;

    public:
        explicit Mesh(system::stream::Stream* f) noexcept;
        std::optional<core::Real> hit(const math::Ray3& ray, core::Real d_min) const noexcept final;

        friend std::ostream& operator<<(std::ostream& os, const Mesh& m) noexcept
        {
            os << "{ 'object_class_name' : 'gearoenix::physics::collider::Mesh', 'triangles' : [";
            for (const math::Triangle3& t : m.ts) {
                os << " " << t << ",";
            }
            os << " ] }";
            return os;
        }
    };
}
}
#endif
