#ifndef GEAROEMIX_RENDER_MATERIAL_DEPTH_HPP
#define GEAROEMIX_RENDER_MATERIAL_DEPTH_HPP
#include "../../math/math-matrix.hpp"
#include "rnd-mat-material.hpp"
#include "../shader/rnd-shd-resources.hpp"
namespace gearoenix {
namespace render {
    namespace material {
        class Depth : public Material {
        public:
            class Resources : public shader::Resources {
            public:
                Resources(Engine* e, pipeline::Pipeline* pip, buffer::Uniform* u);
            };
            typedef struct {
                math::Mat4x4 mvp;
            } Uniform;

        private:
            const core::Id SHADER_ID;
            Uniform u;
            Resources* shdrsc;
            void sub_bind();

        public:
            Depth(core::Id shdid, Engine* e, std::shared_ptr<core::EndCaller> end);
            ~Depth();
            unsigned int get_vertex_elements_count() const;
            core::Id get_shader_id() const;
            void update(const scene::Scene* s, const model::Model* m);
            void bind(texture::Texture2D* shadow_texture);
        };
    }
}
}
#endif
