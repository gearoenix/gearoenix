#ifndef GEAROENIX_PHYSICS_COLLIDER_COLLIDER_HPP
#define GEAROENIX_PHYSICS_COLLIDER_COLLIDER_HPP
#include "../../core/cr-static.hpp"
#include "../../math/math-aabb.hpp"
#include "../phs-transformation.hpp"
#include "phs-cld-type.hpp"
#include <optional>

namespace gearoenix::math {
struct Ray3;
}

namespace gearoenix::system::stream {
class Stream;
}

namespace gearoenix::render::model {
class Model;
}

namespace gearoenix::physics::collider {
/// Child classes must have a transformation implementation and initialize (and update) box
class Collider : public Transformation {
    GX_GET_CVAL_PRT(Type, collider_type)
    GX_GET_CREF_PRT(math::Aabb3, box)
    GX_GETSET_PTR_PRT(render::model::Model, parent)
protected:
    explicit Collider(const Type t) noexcept
        : collider_type(t)
    {
    }

public:
    virtual ~Collider() noexcept = default;
    [[nodiscard]] static Collider* read(system::stream::Stream* f) noexcept;
    /// It returns minimum distance of collider surface from the ray origin along the ray direction
    ///   if the ray hits the surface and the distance is less than d_min
    [[nodiscard]] virtual std::optional<core::Real> hit(const math::Ray3& r, core::Real d_min) const noexcept;

    virtual void set_location(const math::Vec3&) noexcept override;
    virtual void local_scale(core::Real s) noexcept override;
    virtual void local_x_scale(core::Real s) noexcept override;
};
}
#endif
