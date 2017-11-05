#ifndef GEAROEMIX_RENDER_MATERIAL_DIRECTIONAL_COLORED_SPECULATED_BAKED_SHADOWLESS_OPAQUE_HPP
#define GEAROEMIX_RENDER_MATERIAL_DIRECTIONAL_COLORED_SPECULATED_BAKED_SHADOWLESS_OPAQUE_HPP
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
        class DirectionalColoredSpeculatedBakedShadowlessOpaque : public Material {
        public:
            const static core::Id SHADER_ID;
            class Resources : public shader::Resources {
            protected:
                texture::Cube* env;

            public:
                Resources(Engine* e, pipeline::Pipeline* pip, buffer::Uniform* u);
                void set_baked_env(texture::Cube* t);
            };
            typedef struct {
                math::Mat4x4 mvp;
                math::Mat4x4 m;
                math::Vec3 sun = math::Vec3(0.0f, 0.0f, -1.0f);
                math::Vec3 sun_color = math::Vec3(1.0f, 1.0f, 1.0f);
                math::Vec3 eye = math::Vec3(0.0f, 0.0f, 0.0f);
                math::Vec3 spec_color = math::Vec3(1.0f, 1.0f, 1.0f);
                math::Vec3 spec_factors = math::Vec3(0.5f, 0.5f, 0.5f);
                math::Vec3 ambl_color = math::Vec3(0.2f, 0.2f, 0.2f);
                math::Vec3 color = math::Vec3(0.3f, 0.02f, 0.02f);
                core::Real rfl_fac = 0.5f;
            } Uniform;

        private:
            Uniform u;
            std::shared_ptr<texture::Cube> env;
            Resources* shdrsc;

        public:
            DirectionalColoredSpeculatedBakedShadowlessOpaque(system::File* f, Engine* e, std::shared_ptr<core::EndCaller> end);
            ~DirectionalColoredSpeculatedBakedShadowlessOpaque();
            unsigned int get_vertex_elements_count() const;
            void update(const scene::Scene* s, const model::Model* m);
            void bind();
        };
    }
}
}
#endif
