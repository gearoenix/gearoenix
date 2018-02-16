#ifndef GEAROEMIX_RENDER_MATERIAL_DIRECTIONAL_COLORED_SPECULATED_NONREFLECTIVE_SHADOWLESS_OPAQUE_HPP
#define GEAROEMIX_RENDER_MATERIAL_DIRECTIONAL_COLORED_SPECULATED_NONREFLECTIVE_SHADOWLESS_OPAQUE_HPP
#include "../../core/cr-end-caller.hpp"
#include "../../math/math-matrix.hpp"
#include "../../math/math-vector.hpp"
#include "../shader/rnd-shd-resources.hpp"
#include "rnd-mat-material.hpp"
#include <memory>
namespace gearoenix {
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
                math::Vec4 ambl_color;
                math::Vec3 eye;
                core::Real padding1;
                math::Mat4x4 m;
                math::Vec3 spec_color;
                core::Real padding2;
                math::Vec3 spec_factors;
                core::Real padding3;
                math::Vec3 sun;
                core::Real padding4;
                math::Vec4 sun_color;
                math::Mat4x4 vp;
            } Uniform;

        private:
            Uniform u;
            bool color_changed = true;
            math::Vec4 color;
            Resources* shdrsc;

        public:
            DirectionalColoredSpeculatedNonreflectiveShadowlessOpaque(system::stream::Stream* f, Engine* e, core::EndCaller<core::EndCallerIgnore> end);
            ~DirectionalColoredSpeculatedNonreflectiveShadowlessOpaque();
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
