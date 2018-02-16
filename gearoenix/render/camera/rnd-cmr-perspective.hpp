#ifndef GEAROEMIX_CORE_STATIC_HPP
#define GEAROEMIX_CORE_STATIC_HPP
#include "rnd-cmr-camera.hpp"
namespace gearoenix {
namespace render {
    namespace camera {
        class Perspective : public Camera {
        private:
            core::Real h_angle;
            core::Real tanhang;
            core::Real tanvang;
            core::Real one_coshang;
            core::Real one_cosvang;

            void on_ratio_change();

        public:
            Perspective(system::stream::Stream* f, system::Application* app);
            bool in_sight(const math::Vec3& location, const core::Real radius) const;
            void on_event(const core::event::Event& e);
            math::Ray3 create_ray3(const core::Real x, const core::Real y) const;
        };
    }
}
}
#endif
