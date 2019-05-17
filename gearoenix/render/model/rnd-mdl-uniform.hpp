#ifndef GEAROENIX_RENDER_MODEL_UNIFORM_HPP
#define GEAROENIX_RENDER_MODEL_UNIFORM_HPP
#include "../../math/math-matrix.hpp"
namespace gearoenix {
namespace render {
    namespace model {
        struct Uniform {
            math::Mat4x4 m;
            core::Real radius;
        };
    }
}
}
#endif
