#ifndef GEAROEMIX_RENDER_MATERIAL_WHITE_HPP
#define GEAROEMIX_RENDER_MATERIAL_WHITE_HPP
#include "../../math/math-matrix.hpp"
#include "rnd-mat-material.hpp"
namespace gearoenix {
namespace render {
    namespace material {
        class White : public Material {
        public:
            typedef struct {
                math::Mat4x4 mvp;
            } Uniform;

        private:
            Uniform u;
            void sub_bind();

        public:
            White(Engine* e);
            ~White();
            unsigned int get_vertex_elements_count() const;
            void update(const model::Uniform& muni);
        };
    }
}
}
#endif
