#ifndef GEAROENIX_PHYSICS_COLLIDER_COLLIDER_HPP
#define GEAROENIX_PHYSICS_COLLIDER_COLLIDER_HPP
#include "../../core/cr-static.hpp"
#include "../../math/math-aabb.hpp"
#include "phs-cld-type.hpp"
#include <optional>

namespace gearoenix::math {
struct Mat4x4;
struct Ray3;
}

namespace gearoenix::system::stream {
class Stream;
}

namespace gearoenix::physics {
class Transformation;
}

namespace gearoenix::physics::collider {
/// Child classes must have a transformation implementation and initialize (and update) box
class Collider {
    GX_GET_CVAL_PRT(Type, collider_type)
    GX_GET_UPTR_PRT(Transformation, transformation)
    GX_GET_REF_PRT(math::Aabb3, box)
protected:
    explicit Collider(Type t) noexcept;

public:
    virtual ~Collider() noexcept;
    static Collider* read(system::stream::Stream* f) noexcept;
    /// It returns minimum distance of collider surface from the ray origin along the ray direction
    ///   if the ray hits the surface and the distance is less than d_min
    virtual std::optional<core::Real> hit(const math::Ray3& r, core::Real d_min) const noexcept; 
    virtual std::optional<core::Real> hit(const math::Ray3& r) const noexcept = 0;
};
}
#endif
