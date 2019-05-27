#ifndef GEAROENIX_PHYSICS_COLLIDER_MESH_HPP
#define GEAROENIX_PHYSICS_COLLIDER_MESH_HPP
#include "../../core/cr-types.hpp"
#include "../../math/math-aabb.hpp"
#include "../../math/math-triangle.hpp"
#include "../../math/math-vector.hpp"
#include "phs-collider.hpp"
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
        void update(const math::Mat4x4& m) noexcept final;
        bool hit(const math::Ray3& r, core::Real& distance_from_origin) const  noexcept final;
        bool hit(const math::Ray3& r, core::Real& distance_from_origin, math::Vec2& factors) const noexcept;

        friend std::ostream& operator<<(std::ostream& os, const Mesh& m) noexcept
        {
            os << "MeshCollider { triangles: [";
            for (const math::Triangle3& t : m.ts) {
                os << " " << t << ",";
            }
            os << " ],";
            os << "}";
            return os;
        }
    };
}
}
#endif
