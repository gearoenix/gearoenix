#ifndef GEAROENIX_CORE_STATIC_HPP
#define GEAROENIX_CORE_STATIC_HPP
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

        void update_fovy();

    public:
        Perspective(
            core::Id my_id,
            const std::shared_ptr<system::stream::Stream>& f,
            const std::shared_ptr<engine::Engine>& e);
        Perspective(
            core::Id my_id,
            const std::shared_ptr<engine::Engine>& e);
        void set_aspect_ratio(core::Real ratio) final;
        void set_field_of_view(core::Real radian);
        math::Ray3 create_ray3(core::Real x, core::Real y) const final;
        core::Real get_distance(const math::Vec3& model_location) const final;
        std::vector<math::Vec3[4]> get_cascaded_shadow_frustum_partitions() const final;
    };
}
}
#endif
