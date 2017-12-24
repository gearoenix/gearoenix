#ifndef GEAROEMIX_RENDER_MATERIAL_SHADELESS_COLORED_MATTE_NONREFLECTIVE_SHADOWLESS_OPAQUE_HPP
#define GEAROEMIX_RENDER_MATERIAL_SHADELESS_COLORED_MATTE_NONREFLECTIVE_SHADOWLESS_OPAQUE_HPP
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
        class ShadelessColoredMatteNonreflectiveShadowlessOpaque : public Material {
        public:
            const core::Id SHADER_ID;
            class Resources : public shader::Resources {
            public:
                Resources(Engine* e, pipeline::Pipeline* pip, buffer::Uniform* u);
            };
            typedef struct {
                math::Mat4x4 mvp;
                math::Vec3 color;
            } Uniform;

        private:
            Uniform u;
            Resources* shdrsc;

        public:
            ShadelessColoredMatteNonreflectiveShadowlessOpaque(core::Id sid, system::File* f, Engine* e, std::shared_ptr<core::EndCaller> end);
            ~ShadelessColoredMatteNonreflectiveShadowlessOpaque();
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
