#ifndef GEAROENIX_RENDER_CAMERA_PERSPECTIVE_HPP
#define GEAROENIX_RENDER_CAMERA_PERSPECTIVE_HPP
#include "rnd-cmr-camera.hpp"
namespace gearoenix {
namespace physics::collider {
    class Frustum;
}
namespace render::camera {
    class Perspective : public Camera {
    private:
        core::Real fovx = 1.0f;
        core::Real fovy = 1.0f;
        core::Real tanx = 1.0f;
        core::Real tany = 1.0f;
        core::Real lambda = 0.3f;
        const std::shared_ptr<physics::collider::Frustum> frustum;

        void update_fovy() noexcept;
        void update_cascades() noexcept;
        void update_projection() noexcept;

    public:
        Perspective(core::Id my_id, system::stream::Stream* f, engine::Engine* e) noexcept;
        Perspective(core::Id my_id, engine::Engine* e) noexcept;
        void set_aspect_ratio(core::Real ratio) noexcept final;
        void set_field_of_view(core::Real radian) noexcept;
        [[nodiscard]] math::Ray3 create_ray3(core::Real x, core::Real y) const noexcept final;
        [[nodiscard]] core::Real get_distance(const math::Vec3& model_location) const noexcept final;
    };
}
}
#endif
