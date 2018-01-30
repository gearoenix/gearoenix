#ifndef GEAROEMIX_RENDER_CAMERA_ORTHOGRAPHIC_HPP
#define GEAROEMIX_RENDER_CAMERA_ORTHOGRAPHIC_HPP
#include "rnd-cmr-camera.hpp"
namespace gearoenix {
namespace render {
    namespace camera {
        class Orthographic : public Camera {
        private:
            core::Real aspects_size;
            void on_ratio_change();

        protected:
        public:
            Orthographic(system::File* f, system::Application* sysapp);
            bool in_sight(const math::Vec3& location, const core::Real radius) const;
            void on_event(const core::event::Event& e);
			math::Ray3 create_ray3(const core::Real x, const core::Real y) const;
        };
    }
}
}
#endif
