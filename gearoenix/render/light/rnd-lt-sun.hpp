#ifndef GEAROEMIX_RENDER_LIGHT_SUN_HPP
#define GEAROEMIX_RENDER_LIGHT_SUN_HPP
#include "../../math/math-vector.hpp"
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
            math::Vec3 color;

        public:
            Sun(system::File* f);
            ~Sun();
            const math::Vec3& get_direction() const;
            const math::Vec3& get_color() const;
            const camera::Orthographic* get_camera() const;
            camera::Orthographic* get_camera();
        };
    }
}
}
#endif
