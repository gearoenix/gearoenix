#ifndef GEAROEMIX_RENDER_MATERIAL_DEPTH_HPP
#define GEAROEMIX_RENDER_MATERIAL_DEPTH_HPP
#include "../../math/math-matrix.hpp"
#include "rnd-mat-material.hpp"
namespace gearoenix {
namespace render {
    namespace shader {
        class Resources;
    }
    namespace material {
        class Depth : public Material {
        public:
            const static core::Id SHADER_ID;
            typedef struct {
                math::Mat4x4 mvp;
            } Uniform;

        private:
            Uniform u;
            shader::Resources* shdrsc;
            void sub_bind();

        public:
            Depth(Engine* e, std::shared_ptr<core::EndCaller> end);
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
