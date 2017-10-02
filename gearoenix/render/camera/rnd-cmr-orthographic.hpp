#ifndef GEAROEMIX_RENDER_CAMERA_ORTHOGRAPHIC_HPP
#define GEAROEMIX_RENDER_CAMERA_ORTHOGRAPHIC_HPP
#include "rnd-cmr-camera.hpp"
namespace gearoenix {
namespace render {
    namespace camera {
        class Orthographic : public Camera {
        private:
            core::Real aspects_size;

        protected:
        public:
            Orthographic(system::File* f, system::Application* sysapp);
            bool in_sight(const math::Vec3& location, const core::Real radius);
            void window_size_changed();
        };
    }
}
}
#endif
