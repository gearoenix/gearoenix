#ifndef GEAROEMIX_RENDER_MATERIAL_DIRECTIONAL_COLORED_SPECULATED_BAKED_SHADOWLESS_OPAQUE_HPP
#define GEAROEMIX_RENDER_MATERIAL_DIRECTIONAL_COLORED_SPECULATED_BAKED_SHADOWLESS_OPAQUE_HPP
#include "../../math/math-matrix.hpp"
#include "../../math/math-vector.hpp"
#include "../shader/rnd-shd-resources.hpp"
#include "rnd-mat-material.hpp"
#include <memory>
namespace gearoenix {
namespace render {
    namespace texture {
        class Cube;
    }
    namespace material {
        class DirectionalColoredSpeculatedBakedShadowlessOpaque : public Material {
        public:
            class Resources : public shader::Resources {
            protected:
                texture::Cube* env;

            public:
                Resources(Engine* e, pipeline::Pipeline* pip, buffer::Uniform* u);
                void set_baked_env(texture::Cube* t);
            };
            typedef struct {
                math::Vec4 ambl_color;
                math::Vec3 eye;
                core::Real padding1;
                math::Mat4x4 m;
                core::Real rfl_fac;
                math::Vec3 padding2;
                math::Vec3 spec_color;
                core::Real padding3;
                math::Vec3 spec_factors;
                core::Real padding4;
                math::Vec3 sun;
                core::Real padding5;
                math::Vec4 sun_color;
                math::Mat4x4 vp;
            } Uniform;

        private:
            const core::Id SHADER_ID;
            Uniform u;
            bool color_changed = true;
            math::Vec4 color;
            std::shared_ptr<texture::Cube> env;
            Resources* shdrsc;

        public:
            DirectionalColoredSpeculatedBakedShadowlessOpaque(core::Id shdid, system::file::File* f, Engine* e, core::EndCaller<core::EndCallerIgnore> end);
            ~DirectionalColoredSpeculatedBakedShadowlessOpaque();
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
