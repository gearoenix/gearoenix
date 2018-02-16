#ifndef GEAROEMIX_RENDER_MATERIAL_DIRECTIONAL_D2_SPECULATED_NONREFLECTIVE_SHADOWLESS_OPAQUE_HPP
#define GEAROEMIX_RENDER_MATERIAL_DIRECTIONAL_D2_SPECULATED_NONREFLECTIVE_SHADOWLESS_OPAQUE_HPP
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
        class DirectionalD2SpeculatedNonreflectiveShadowlessOpaque : public Material {
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
                math::Vec3 ambl_color;
                core::Real padding1;
                math::Vec3 eye;
                core::Real padding2;
                math::Mat4x4 m;
                math::Vec3 spec_color;
                core::Real padding3;
                math::Vec3 spec_factors;
                core::Real padding4;
                math::Vec3 sun;
                core::Real padding5;
                math::Vec3 sun_color;
                core::Real padding6;
                math::Mat4x4 vp;
            } Uniform;

        private:
            Uniform u;
            std::shared_ptr<texture::Texture2D> t;
            Resources* shdrsc;

        public:
            DirectionalD2SpeculatedNonreflectiveShadowlessOpaque(system::stream::Stream* f, Engine* e, core::EndCaller<core::EndCallerIgnore> end);
            ~DirectionalD2SpeculatedNonreflectiveShadowlessOpaque();
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
