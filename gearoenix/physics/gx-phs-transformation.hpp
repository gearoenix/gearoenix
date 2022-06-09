#ifndef GEAROENIX_PHYSICS_TRANSFORMATION_HPP
#define GEAROENIX_PHYSICS_TRANSFORMATION_HPP
#include "../core/ecs/gx-cr-ecs-component.hpp"
#include "../math/gx-math-matrix-4d.hpp"
#include "../math/gx-math-quaternion.hpp"
#include <functional>

namespace gearoenix::physics {
struct Transformation final : public core::ecs::Component {
    GX_GET_CREF_PRV(math::Mat4x4<double>, matrix)
    /// This is useful for caching the calculation
    /// It gets updated in each loop so be careful and check the `changed` variable
    GX_GET_CREF_PRV(math::Mat4x4<double>, inverted_matrix)
    GX_GET_CREF_PRV(math::Vec3<double>, x_axis)
    GX_GET_CREF_PRV(math::Vec3<double>, y_axis)
    GX_GET_CREF_PRV(math::Vec3<double>, z_axis)
    GX_GET_CREF_PRV(math::Vec3<double>, scale)
    GX_GET_VAL_PRV(bool, changed, true)

public:
    Transformation() noexcept;
    ~Transformation() noexcept final = default;
    Transformation(Transformation&&) noexcept = default;
    [[nodiscard]] math::Vec3<double> get_location() const noexcept;
    void get_location(math::Vec3<double>& l) const noexcept;
    void set_location(const math::Vec3<double>& l) noexcept;
    void set_location(double x, double y, double z) noexcept;
    void translate(const math::Vec3<double>& t) noexcept;
    void local_x_translate(double v) noexcept;
    void local_y_translate(double v) noexcept;
    void local_z_translate(double v) noexcept;
    void global_rotate(double d, const math::Vec3<double>& axis, const math::Vec3<double>& location) noexcept;
    void global_rotate(double d, const math::Vec3<double>& axis) noexcept;
    void local_rotate(double d, const math::Vec3<double>& axis) noexcept;
    void local_rotate_quaternion(double x, double y, double z, double w) noexcept;
    void local_x_rotate(double d) noexcept;
    void local_y_rotate(double d) noexcept;
    void local_z_rotate(double d) noexcept;
    void local_scale(double s) noexcept;
    void local_scale(const math::Vec3<double>& s) noexcept;
    void local_x_scale(double s) noexcept;
    void local_y_scale(double s) noexcept;
    void local_z_scale(double s) noexcept;
    void set_orientation(const math::Quat<double>& q) noexcept;
    void look_at(const math::Vec3<double>& location, const math::Vec3<double>& target, const math::Vec3<double>& up) noexcept;
    void look_at(const math::Vec3<double>& target, const math::Vec3<double>& up) noexcept;
    void update() noexcept;
    void clear() noexcept;
};
}
#endif
