#ifndef GEAROENIX_PHYSICS_TRANSFORMATION_HPP
#define GEAROENIX_PHYSICS_TRANSFORMATION_HPP
#include "../core/cr-static.hpp"
#include "../math/math-matrix.hpp"
#include "../math/math-quaternion.hpp"
#include "../math/math-vector.hpp"
#include <functional>

namespace gearoenix::physics {
class Transformation {
    GX_GETSET_CREF_PRT(math::Vec3, x_axis)
    GX_GETSET_CREF_PRT(math::Vec3, y_axis)
    GX_GETSET_CREF_PRT(math::Vec3, z_axis)
    GX_GET_CREF_PRT(math::Vec3, current_local_scale)
    GX_GETSET_CREF_PRT(std::function<void()>, on_scale)
protected:
    Transformation() noexcept;

public:
    virtual ~Transformation() noexcept;
    [[nodiscard]] virtual const math::Vec3& get_location() const noexcept;
    virtual void get_location(math::Vec3& l) const noexcept;
    virtual void set_location(const math::Vec3& l) noexcept;
    virtual void translate(const math::Vec3& t) noexcept;
    virtual void local_x_translate(core::Real t) noexcept;
    virtual void local_y_translate(core::Real t) noexcept;
    virtual void local_z_translate(core::Real t) noexcept;
    virtual void global_rotate(core::Real d, const math::Vec3& axis, const math::Vec3& location) noexcept;
    virtual void local_rotate(core::Real d, const math::Vec3& axis) noexcept;
    virtual void local_x_rotate(core::Real d) noexcept;
    virtual void local_y_rotate(core::Real d) noexcept;
    virtual void local_z_rotate(core::Real d) noexcept;
    virtual void local_scale(core::Real s) noexcept;
    virtual void local_x_scale(core::Real s) noexcept;
    virtual void local_y_scale(core::Real s) noexcept;
    virtual void local_z_scale(core::Real s) noexcept;
    virtual void set_orientation(const math::Quat& q) noexcept;
};
}
#endif
