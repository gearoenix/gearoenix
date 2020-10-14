#ifndef GEAROENIX_PHYSICS_COLLIDER_COLLIDER_HPP
#define GEAROENIX_PHYSICS_COLLIDER_COLLIDER_HPP
#include "../../core/gx-cr-static.hpp"
#include "../../math/gx-math-aabb.hpp"
#include "../../math/gx-math-matrix-4d.hpp"
#include "../gx-phs-transformation.hpp"
#include "gx-phs-cld-type.hpp"
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
    GX_GET_CREF_PRT(math::Mat4x4<double>, model_matrix)
    GX_GETSET_PTR_PRT(render::model::Model, parent)
protected:
    math::Vec3<double> updated_points[8];

    void update_box() noexcept;

    explicit Collider(Type t) noexcept;
    Collider(Type t, const Collider& c) noexcept;
    Collider(const Collider& c) noexcept;

public:
    ~Collider() noexcept override = default;
    [[nodiscard]] static Collider* read(system::stream::Stream* f) noexcept;
    /// It returns minimum distance of collider surface from the ray origin along the ray direction
    ///   if the ray hits the surface and the distance is less than d_min
    [[nodiscard]] virtual std::optional<double> hit(const math::Ray3& r, double d_min) const noexcept;
    [[nodiscard]] virtual bool check_intersection(const math::Aabb3& box) const noexcept;
    [[nodiscard]] virtual math::IntersectionStatus check_intersection_status(const math::Aabb3& box) const noexcept;
    [[nodiscard]] virtual Collider* clone() const noexcept;
    void put_in_box(const math::Aabb3& b) noexcept;

    [[nodiscard]] math::Vec3<double> get_location() const noexcept override;
    void set_location(const math::Vec3<double>&) noexcept override;
    void local_z_rotate(double d) noexcept final;
    void local_scale(double s) noexcept override;
    void local_scale(const math::Vec3<double>& s) noexcept override;
    void local_x_scale(double s) noexcept override;
    void local_y_scale(double s) noexcept override;
    void local_z_scale(double s) noexcept override;

    // TODO: I have to implement rotations

    virtual void set_model_matrix(const math::Mat4x4<double>& m) noexcept;
};
}
#endif
