#ifndef GEAROENIX_RENDER_MODEL_UNIFORM_HPP
#define GEAROENIX_RENDER_MODEL_UNIFORM_HPP
#include "../../math/math-matrix.hpp"
#include <memory>
namespace gearoenix {
namespace render {
    namespace model {
        struct Uniform {
        private:
            math::Mat4x4 m;

        public:
            Uniform();
            inline math::Mat4x4& get_m();
            inline const math::Mat4x4& get_m() const;
        };
    }
}
}
#endif
