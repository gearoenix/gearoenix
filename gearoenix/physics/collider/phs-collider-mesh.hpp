#ifndef GEAROENIX_PHYSICS_COLLIDER_MESH_HPP
#define GEAROENIX_PHYSICS_COLLIDER_MESH_HPP
#include "../../core/cr-types.hpp"
#include "../../math/math-aabb.hpp"
#include "../../math/math-triangle.hpp"
#include "../../math/math-vector.hpp"
#include "phs-collider.hpp"
#include <vector>
namespace gearoenix {
namespace system {
    class File;
}
namespace physics {
    namespace collider {
        class Mesh : public Collider {
        private:
            typedef struct {
                math::Vec3 p[3];
            } Triangle;
            std::vector<Triangle> ps;
            std::vector<math::Triangle3> ts;
            math::Aabb3 box;

        public:
            Mesh(system::file::File* in);
            ~Mesh();
            void update(const math::Mat4x4& m);
            bool hit(const math::Ray3& r, core::Real& distance_from_origin) const;
            bool hit(const math::Ray3& r, core::Real& distance_from_origin, math::Vec2& factors) const;
            void print() const;
        };
    }
}
}
#endif
