#ifndef GEAROEMIX_RENDER_MATERIAL_DIRECTIONAL_COLORED_SPECULATED_NONREFLECTIVE_SHADOWLESS_OPAQUE_HPP
#define GEAROEMIX_RENDER_MATERIAL_DIRECTIONAL_COLORED_SPECULATED_NONREFLECTIVE_SHADOWLESS_OPAQUE_HPP
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
        class DirectionalColoredSpeculatedNonreflectiveShadowlessOpaque : public Material {
        public:
            const static core::Id SHADER_ID;
            class Resources : public shader::Resources {
            public:
                Resources(Engine* e, pipeline::Pipeline* pip, buffer::Uniform* u);
            };
            typedef struct {
                math::Mat4x4 mvp;
                math::Mat4x4 m;
                math::Vec3 sun = math::Vec3(0.0f, 0.0f, -1.0f);
                math::Vec3 sun_color = math::Vec3(1.0f, 1.0f, 1.0f);
                math::Vec3 eye = math::Vec3(0.0f, 0.0f, 0.0f);
                math::Vec3 spec_color = math::Vec3(1.0f, 1.0f, 1.0f);
                core::Real spec_factor = 0.5f;
                math::Vec3 ambl_color = math::Vec3(0.2f, 0.2f, 0.2f);
                math::Vec3 color = math::Vec3(0.3f, 0.02f, 0.02f);
            } Uniform;

        private:
            Uniform u;
            Resources* shdrsc;

        public:
            DirectionalColoredSpeculatedNonreflectiveShadowlessOpaque(system::File* f, Engine* e, std::shared_ptr<core::EndCaller> end);
            ~DirectionalColoredSpeculatedNonreflectiveShadowlessOpaque();
            unsigned int get_vertex_elements_count() const;
            void update(const scene::Scene* s, const model::Model* m);
            void bind();
        };
    }
}
}
#endif
