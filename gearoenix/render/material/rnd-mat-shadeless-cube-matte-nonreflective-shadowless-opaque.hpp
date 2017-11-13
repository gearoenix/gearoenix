#ifndef GEAROEMIX_RENDER_MATERIAL_SHADELESS_CUBE_MATTE_NONREFLECTIVE_SHADOWLESS_OPAQUE_HPP
#define GEAROEMIX_RENDER_MATERIAL_SHADELESS_CUBE_MATTE_NONREFLECTIVE_SHADOWLESS_OPAQUE_HPP
#include "../../math/math-matrix.hpp"
#include "../../math/math-vector.hpp"
#include "../shader/rnd-shd-resources.hpp"
#include "rnd-mat-material.hpp"
#include <memory>
namespace gearoenix {
namespace core {
    class EndCaller;
}
namespace render {
    namespace texture {
        class Cube;
    }
    namespace material {
        class ShadelessCubeMatteNonreflectiveShadowlessOpaque : public Material {
        public:
            const static core::Id SHADER_ID;
            class Resources : public shader::Resources {
            protected:
                texture::Cube* ctxt;

            public:
                Resources(Engine* e, pipeline::Pipeline* pip, buffer::Uniform* u);
                void set_cube_texture(texture::Cube* ct);
            };
            typedef struct {
                math::Mat4x4 mvp;
                math::Mat4x4 m;
            } Uniform;

        private:
            Uniform u;
            Resources* shdrsc;
            std::shared_ptr<texture::Cube> ctxt;

        public:
            ShadelessCubeMatteNonreflectiveShadowlessOpaque(system::File* f, Engine* e, std::shared_ptr<core::EndCaller> end);
            ~ShadelessCubeMatteNonreflectiveShadowlessOpaque();
            unsigned int get_vertex_elements_count() const;
            core::Id get_shader_id() const;
            void update(const scene::Scene* s, const model::Model* m);
            void bind(texture::Texture2D* shadow_texture);
        };
    }
}
}
#endif
