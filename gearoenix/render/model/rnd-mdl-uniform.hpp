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
            math::Mat4x4 mvp;

        public:
            Uniform();
            math::Mat4x4& get_m();
            const math::Mat4x4& get_m() const;
            math::Mat4x4& get_mvp();
            const math::Mat4x4& get_mvp() const;
        };
    }
}
}
#endif
