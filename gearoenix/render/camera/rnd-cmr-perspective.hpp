#ifndef GEAROEMIX_CORE_STATIC_HPP
#define GEAROEMIX_CORE_STATIC_HPP
#include "rnd-cmr-camera.hpp"
namespace gearoenix {
namespace render {
    namespace camera {
        class Perspective : public Camera {
        private:
            core::Real h_angle;
            core::Real v_angle;

        public:
            Perspective(system::File* f, system::Application* app);
            bool in_sight(const math::Vec3& location, const core::Real radius);
            void window_size_changed();
        };
    }
}
}
#endif
