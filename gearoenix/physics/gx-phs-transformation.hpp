#ifndef GEAROENIX_PHYSICS_TRANSFORMATION_HPP
#define GEAROENIX_PHYSICS_TRANSFORMATION_HPP
#include "../core/ecs/gx-cr-ecs-component.hpp"
#include "../math/gx-math-matrix-4d.hpp"
#include "../math/gx-math-quaternion.hpp"
#include <functional>

namespace gearoenix::physics {
struct Transformation final : public core::ecs::Component {
    GX_GET_CREF_PRV(math::Mat4x4<double>, local_matrix);
    GX_GET_CREF_PRV(math::Mat4x4<double>, global_matrix);
    /// This is useful for caching the calculation
    /// It gets updated in each loop so be careful and check the `changed` variable
    GX_GET_CREF_PRV(math::Mat4x4<double>, inverted_global_matrix);
    GX_GET_CREF_PRV(math::Vec3<double>, x_axis);
    GX_GET_CREF_PRV(math::Vec3<double>, y_axis);
    GX_GET_CREF_PRV(math::Vec3<double>, z_axis);
    GX_GET_CREF_PRV(math::Vec3<double>, scale);
    GX_GET_VAL_PRV(bool, changed, true);

public:
    explicit Transformation(std::string&& name) noexcept;
    ~Transformation() noexcept final = default;
    Transformation(Transformation&&) noexcept = default;
    [[nodiscard]] math::Vec3<double> get_global_location() const noexcept;
    void get_global_location(math::Vec3<double>& l) const noexcept;
    [[nodiscard]] math::Vec3<double> get_local_location() const noexcept;
    void get_local_location(math::Vec3<double>& l) const noexcept;
    void set_local_location(const math::Vec3<double>& l) noexcept;
    void local_translate(const math::Vec3<double>& t) noexcept;
    void local_x_translate(double v) noexcept;
    void local_y_translate(double v) noexcept;
    void local_z_translate(double v) noexcept;
    void local_outer_rotate(double d, const math::Vec3<double>& axis, const math::Vec3<double>& location) noexcept;
    void local_outer_rotate(double d, const math::Vec3<double>& axis) noexcept;
    void local_rotate(double d, const math::Vec3<double>& axis) noexcept;
    void local_rotate(const math::Quat<double>& q) noexcept;
    void local_x_rotate(double d) noexcept;
    void local_y_rotate(double d) noexcept;
    void local_z_rotate(double d) noexcept;
    void local_scale(double s) noexcept;
    void local_scale(const math::Vec3<double>& s) noexcept;
    void local_x_scale(double s) noexcept;
    void local_y_scale(double s) noexcept;
    void local_z_scale(double s) noexcept;
    void set_local_orientation(const math::Quat<double>& q) noexcept;
    [[nodiscard]] math::Quat<double> get_local_orientation() const noexcept;
    void local_look_at(const math::Vec3<double>& location, const math::Vec3<double>& target, const math::Vec3<double>& up) noexcept;
    void local_look_at(const math::Vec3<double>& target, const math::Vec3<double>& up) noexcept;
    void local_update() noexcept;
    void update(const Transformation& parent) noexcept;
    void local_update_without_inverse() noexcept;
    void update_without_inverse(const Transformation& parent) noexcept;
    void clear_change() noexcept;
    void reset() noexcept;
    void reset(const math::Vec3<double>& scale, const math::Quat<double>& rotation, const math::Vec3<double>& location) noexcept;
    void reset(
        const math::Vec3<double>& scale,
        const math::Vec3<double>& x_axis,
        const math::Vec3<double>& y_axis,
        const math::Vec3<double>& z_axis,
        const math::Vec3<double>& location) noexcept;
    void show_gui() noexcept;
};
}
#endif
