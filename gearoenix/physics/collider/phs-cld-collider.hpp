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
    GX_GET_CREF_PRT(math::Aabb3, origin_box)
    GX_GET_CREF_PRT(math::Aabb3, updated_box)
    GX_GET_CREF_PRT(math::Mat4x4, model_matrix)
    GX_GETSET_PTR_PRT(render::model::Model, parent)
protected:
    math::Vec3 updated_points[8];

    void update_box() noexcept;

    explicit Collider(const Type t) noexcept
        : collider_type(t)
    {
    }

public:
    ~Collider() noexcept override = default;
    [[nodiscard]] static Collider* read(system::stream::Stream* f) noexcept;
    /// It returns minimum distance of collider surface from the ray origin along the ray direction
    ///   if the ray hits the surface and the distance is less than d_min
    [[nodiscard]] virtual std::optional<core::Real> hit(const math::Ray3& r, core::Real d_min) const noexcept;
    [[nodiscard]] virtual bool check_intersection(const Collider* o) const noexcept;
    [[nodiscard]] virtual math::IntersectionStatus check_intersection_status(const math::Aabb3& box) const noexcept;
    void put_in_box(const math::Aabb3& b) noexcept;

    [[nodiscard]] const math::Vec3& get_location() const noexcept override;
    void set_location(const math::Vec3&) noexcept override;
    void local_scale(core::Real s) noexcept override;
    void local_x_scale(core::Real s) noexcept override;

    // TODO: I have to implement rotations
};
}
#endif
