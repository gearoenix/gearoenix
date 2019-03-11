#ifndef GEAROEMIX_CORE_STATIC_HPP
#define GEAROEMIX_CORE_STATIC_HPP
#include "rnd-cmr-camera.hpp"
namespace gearoenix {
namespace physics
{
namespace collider {
class Frustum;
}
}
namespace render {
    namespace camera {
        class Perspective : public Camera {
        private:
            core::Real fovx = 1.0f;
            core::Real fovy = 1.0f;
            core::Real tanx = 1.0f;
            core::Real tany = 1.0f;
            core::Real lambda = 0.3f;
            const std::shared_ptr<physics::collider::Frustum> frustum;

            void on_ratio_change();

        public:
            Perspective(
                    const core::Id my_id,
                    const std::shared_ptr<system::stream::Stream> &f,
                    const std::shared_ptr<engine::Engine> &e,
                    const core::sync::EndCaller<core::sync::EndCallerIgnore> &c);
            bool in_sight(const math::Vec3& location, const core::Real radius) const;
            void on_event(const core::event::Event& e);
            math::Ray3 create_ray3(const core::Real x, const core::Real y) const;
            core::Real get_distance(const math::Vec3 model_location) const;
            void set_vertical_field_of_view(const core::Real radian);
        };
    }
}
}
#endif
