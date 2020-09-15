#ifndef GEAROENIX_RENDER_CAMERA_TRANSFORMATION_HPP
#define GEAROENIX_RENDER_CAMERA_TRANSFORMATION_HPP
#include "../../physics/gx-phs-transformation.hpp"
#include <functional>
#include <vector>

namespace gearoenix::physics::collider {
class Collider;
class Frustum;
}

namespace gearoenix::render::camera {
struct Uniform;
class Transformation : public physics::Transformation {
private:
    Uniform* const uniform;
    physics::collider::Frustum* const frustum_collider;
    std::vector<std::array<math::Vec3<double>, 4>>* const cascaded_shadow_frustum_partitions;
    std::function<void()> on_frustum_update = [] {};
    std::function<void()> on_projection_update = [] {};

public:
    Transformation(
        Uniform* uniform,
        physics::collider::Frustum* frustum_collider,
        std::vector<std::array<math::Vec3<double>, 4>>* cascade) noexcept;
    void update_location() noexcept;
    void update_projection() noexcept;
    void update_view_projection() noexcept;
    void update_frustum() noexcept;
    void set_on_frustum_update(std::function<void()> f) noexcept;
    void set_on_projection_update(std::function<void()> f) noexcept;
    void look_at(const math::Vec3<double>& target, const math::Vec3<double>& up) noexcept;
    void look_at(const math::Vec3<double>& origin, const math::Vec3<double>& target, const math::Vec3<double>& up) noexcept;
    // physics::Transformation----------------------------------------------------------------------------
    [[nodiscard]] math::Vec3<double> get_location() const noexcept final;
    void get_location(math::Vec3<double>& l) const noexcept final;
    void set_location(const math::Vec3<double>& l) noexcept final;
    void translate(const math::Vec3<double>& t) noexcept final;
    void local_x_translate(double t) noexcept final;
    void local_y_translate(double t) noexcept final;
    void local_z_translate(double t) noexcept final;
    void global_rotate(double d, const math::Vec3<double>& axis, const math::Vec3<double>& location) noexcept final;
    void local_rotate(double d, const math::Vec3<double>& axis) noexcept final;
    void local_x_rotate(double d) noexcept final;
    void local_y_rotate(double d) noexcept final;
    void local_z_rotate(double d) noexcept final;
    void set_orientation(const math::Quat<double>& q) noexcept final;
};
}
#endif
