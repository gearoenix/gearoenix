#ifndef GEAROEMIX_RENDER_LIGHT_SUN_HPP
#define GEAROEMIX_RENDER_LIGHT_SUN_HPP
#include "rnd-lt-light.hpp"
namespace gearoenix {
namespace render {
    namespace camera {
        class Orthographic;
    }
    namespace light {
        class Sun : public Light {
        private:
            camera::Orthographic* cam;

        public:
            Sun(system::File* f);
            ~Sun();
        };
    }
}
}
#endif
