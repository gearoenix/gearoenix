#ifndef GEAROEMIX_RENDER_MATERIAL_SHADELESS_D2_MATTE_NONREFLECTIVE_SHADOWLESS_OPAQUE_HPP
#define GEAROEMIX_RENDER_MATERIAL_SHADELESS_D2_MATTE_NONREFLECTIVE_SHADOWLESS_OPAQUE_HPP
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
        class Texture2D;
    }
    namespace material {
        class ShadelessD2MatteNonreflectiveShadowlessOpaque : public Material {
        public:
            class Resources : public shader::Resources {
            protected:
                texture::Texture2D* txt2d;

            public:
                Resources(Engine* e, pipeline::Pipeline* pip, buffer::Uniform* u);
                void set_texture_2d(texture::Texture2D* t2d);
            };
            typedef struct {
                math::Mat4x4 mvp;
            } Uniform;

        private:
            const core::Id SHADER_ID;
            Uniform u;
            Resources* shdrsc;
            std::shared_ptr<texture::Texture2D> txt2d;

        public:
            ShadelessD2MatteNonreflectiveShadowlessOpaque(core::Id sid, system::File* f, Engine* e, std::shared_ptr<core::EndCaller> end);
            ~ShadelessD2MatteNonreflectiveShadowlessOpaque();
            unsigned int get_vertex_elements_count() const;
            core::Id get_shader_id() const;
            bool needs_mvp() const;
            bool needs_dbm() const;
            void update(const scene::Scene* s, const model::Model* m);
            void bind(texture::Texture2D* shadow_texture);
        };
    }
}
}
#endif
