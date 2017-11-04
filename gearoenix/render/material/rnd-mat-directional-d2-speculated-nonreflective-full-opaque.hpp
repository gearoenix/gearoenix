#ifndef GEAROEMIX_RENDER_MATERIAL_DIRECTIONAL_D2_SPECULATED_NONREFLECTIVE_FULL_OPAQUE_HPP
#define GEAROEMIX_RENDER_MATERIAL_DIRECTIONAL_D2_SPECULATED_NONREFLECTIVE_FULL_OPAQUE_HPP
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
        class DirectionalD2SpeculatedNonreflectiveFullOpaque : public Material {
        public:
            const static core::Id SHADER_ID;
            class Resources : public shader::Resources {
            protected:
                texture::Texture2D* txt;

            public:
                Resources(Engine* e, pipeline::Pipeline* pip, buffer::Uniform* u);
                void set_texture(texture::Texture2D* t);
            };
            typedef struct {
                math::Mat4x4 mvp;
                math::Mat4x4 m;
                math::Vec3 sun;
                math::Vec3 sun_color;
                math::Vec3 spec_color;
                math::Vec3 spec_factors;
                math::Vec3 ambl_color;
                math::Vec3 eye;
            } Uniform;

        private:
            Uniform u;
            std::shared_ptr<texture::Texture2D> t;
            Resources* shdrsc;

        public:
            DirectionalD2SpeculatedNonreflectiveFullOpaque(system::File* f, Engine* e, std::shared_ptr<core::EndCaller> end);
            ~DirectionalD2SpeculatedNonreflectiveFullOpaque();
            unsigned int get_vertex_elements_count() const;
            void update(const scene::Scene* s, const model::Model* m);
            void bind();
        };
    }
}
}
#endif
