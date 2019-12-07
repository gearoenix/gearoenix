#ifndef GEAROENIX_RENDER_CAMERA_TRANSFORMATON_HPP
#define GEAROENIX_RENDER_CAMERA_TRANSFORMATON_HPP
#include "../../physics/phs-transformation.hpp"
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
    std::vector<std::array<math::Vec3, 4>>* const cascaded_shadow_frustum_partitions;
    std::function<void()> on_frustum_update = [] {};
    std::function<void()> on_projection_update = [] {};

public:
    Transformation(
        Uniform* uniform,
        physics::collider::Frustum* frustum_collider,
        std::vector<std::array<math::Vec3, 4>>* cascade) noexcept;
    void update_location() noexcept;
    void update_projection() noexcept;
    void update_view_projection() noexcept;
    void set_on_frustum_update(std::function<void()> f) noexcept;
    void set_on_projection_update(std::function<void()> f) noexcept;
    void look_at(const math::Vec3& target, const math::Vec3& up) noexcept;
    void look_at(const math::Vec3& origin, const math::Vec3& target, const math::Vec3& up) noexcept;
    // physics::Transformation----------------------------------------------------------------------------
    [[nodiscard]] const math::Vec3& get_location() const noexcept final;
    void get_location(math::Vec3& l) const noexcept final;
    void set_location(const math::Vec3& l) noexcept final;
    void translate(const math::Vec3& t) noexcept final;
    void local_x_translate(core::Real t) noexcept final;
    void local_y_translate(core::Real t) noexcept final;
    void local_z_translate(core::Real t) noexcept final;
    void global_rotate(core::Real d, const math::Vec3& axis, const math::Vec3& location) noexcept final;
    void local_rotate(core::Real d, const math::Vec3& axis) noexcept final;
    void local_x_rotate(core::Real d) noexcept final;
    void local_y_rotate(core::Real d) noexcept final;
    void local_z_rotate(core::Real d) noexcept final;
    void set_orientation(const math::Quat& q) noexcept final;
};
}
#endif
