#ifndef GEAROEMIX_RENDER_LIGHT_SUN_HPP
#define GEAROEMIX_RENDER_LIGHT_SUN_HPP
#include "../../math/math-matrix.hpp"
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
            bool color_changed = true;
            math::Vec3 color;
            math::Mat4x4 db;

        public:
            Sun(system::file::File* f);
            ~Sun();
            const math::Vec3& get_direction() const;
            const math::Vec3& get_color() const;
            bool get_color_changed() const;
            const math::Mat4x4& get_bias() const;
            const camera::Orthographic* get_camera() const;
        };
    }
}
}
#endif
