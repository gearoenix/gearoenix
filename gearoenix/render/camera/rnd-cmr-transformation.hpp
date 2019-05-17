#ifndef GEAROENIX_RENDER_CAMERA_TRANSFORMATON_HPP
#define GEAROENIX_RENDER_CAMERA_TRANSFORMATON_HPP
#include "../../physics/phs-transformation.hpp"

namespace gearoenix {
namespace math {
    struct ProjectorFrustum;
}
namespace render::camera {
    struct Uniform;
    class Transformation : public physics::Transformation {
    private:
        const std::shared_ptr<Uniform> uniform;
        const std::shared_ptr<math::ProjectorFrustum> frustum;

    public:
        Transformation(std::shared_ptr<Uniform> uniform, std::shared_ptr<math::ProjectorFrustum> frustum) noexcept;
        void update_location() noexcept;
        void update_view_projection() noexcept;
        void look_at(const math::Vec3& target, const math::Vec3& up) noexcept;
        void look_at(const math::Vec3& origin, const math::Vec3& target, const math::Vec3& up) noexcept;
        // physics::Transformation----------------------------------------------------------------------------
        const math::Vec3& get_x_axis() const noexcept final;
        const math::Vec3& get_y_axis() const noexcept final;
        const math::Vec3& get_z_axis() const noexcept final;
        const math::Vec3& get_location() const noexcept final;
        void get_location(math::Vec3& l) const noexcept final;
        void set_location(const math::Vec3& l) noexcept final;
        void translate(const math::Vec3& t) noexcept final;
        void local_x_translate(core::Real t) noexcept final;
        void local_y_translate(core::Real t) noexcept final;
        void local_z_translate(core::Real t) noexcept final;
        void global_rotate(core::Real d, const math::Vec3& axis, const math::Vec3& location) noexcept final;
        void global_rotate(core::Real d, const math::Vec3& axis) noexcept final;
        void local_rotate(core::Real d, const math::Vec3& axis) noexcept final;
        void local_x_rotate(core::Real d) noexcept final;
        void local_y_rotate(core::Real d) noexcept final;
        void local_z_rotate(core::Real d) noexcept final;
        void set_orientation(const math::Quat& q) noexcept final;
    };
}
}
#endif
