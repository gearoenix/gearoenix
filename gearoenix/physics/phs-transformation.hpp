#ifndef GEAROENIX_PHYSICS_TRANSFORMATION_HPP
#define GEAROENIX_PHYSICS_TRANSFORMATION_HPP
#include "../core/cr-static.hpp"
#include "../math/math-matrix-4d.hpp"
#include "../math/math-quaternion.hpp"
#include <functional>

namespace gearoenix::physics {
class Transformation {
    GX_GETSET_CREF_PRT(math::Vec3<double>, x_axis)
    GX_GETSET_CREF_PRT(math::Vec3<double>, y_axis)
    GX_GETSET_CREF_PRT(math::Vec3<double>, z_axis)
    GX_GET_CREF_PRT(math::Vec3<double>, current_local_scale)
    GX_GETSET_CREF_PRT(std::function<void()>, on_scale)
protected:
    Transformation() noexcept;

public:
    virtual ~Transformation() noexcept;
    [[nodiscard]] virtual const math::Vec3<double>& get_location() const noexcept;
    virtual void get_location(math::Vec3<double>& l) const noexcept;
    virtual void set_location(const math::Vec3<double>& l) noexcept;
    virtual void translate(const math::Vec3<double>& t) noexcept;
    virtual void local_x_translate(double t) noexcept;
    virtual void local_y_translate(double t) noexcept;
    virtual void local_z_translate(double t) noexcept;
    virtual void global_rotate(double d, const math::Vec3<double>& axis, const math::Vec3<double>& location) noexcept;
    virtual void local_rotate(double d, const math::Vec3<double>& axis) noexcept;
    virtual void local_x_rotate(double d) noexcept;
    virtual void local_y_rotate(double d) noexcept;
    virtual void local_z_rotate(double d) noexcept;
    virtual void local_scale(double s) noexcept;
    virtual void local_x_scale(double s) noexcept;
    virtual void local_y_scale(double s) noexcept;
    virtual void local_z_scale(double s) noexcept;
    virtual void set_orientation(const math::Quat<double>& q) noexcept;
};
}
#endif
