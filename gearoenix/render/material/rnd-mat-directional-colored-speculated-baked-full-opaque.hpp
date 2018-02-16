#ifndef GEAROEMIX_RENDER_MATERIAL_DIRECTIONAL_COLORED_SPECULATED_BAKED_FULL_OPAQUE_HPP
#define GEAROEMIX_RENDER_MATERIAL_DIRECTIONAL_COLORED_SPECULATED_BAKED_FULL_OPAQUE_HPP
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
        class DirectionalColoredSpeculatedBakedFullOpaque : public Material {
        public:
            const static core::Id SHADER_ID;
            class Resources : public shader::Resources {
            protected:
                texture::Cube* env;
                texture::Texture2D* shdtxt;

            public:
                Resources(Engine* e, pipeline::Pipeline* pip, buffer::Uniform* u);
                void set_baked_env(texture::Cube* t);
                void set_shadow_texture(texture::Texture2D* t);
            };
            typedef struct {
                math::Vec4 ambl_color;
                math::Mat4x4 db;
                math::Vec3 eye;
                core::Real padding2;
                math::Mat4x4 m;
                core::Real rfl_fac;
                math::Vec3 padding3;
                math::Vec3 spec_color;
                core::Real spec_alpha = 1.0f; // this is temporary in future this must be feed by blender part
                math::Vec3 spec_factors;
                core::Real padding5;
                math::Vec3 sun;
                core::Real padding6;
                math::Vec4 sun_color;
                math::Mat4x4 vp;
            } Uniform;

        private:
            Uniform u;
            bool color_changed = true;
            math::Vec4 color;
            std::shared_ptr<texture::Cube> env;
            Resources* shdrsc;

        public:
            DirectionalColoredSpeculatedBakedFullOpaque(system::file::File* f, Engine* e, core::EndCaller<core::EndCallerIgnore> end);
            ~DirectionalColoredSpeculatedBakedFullOpaque();
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
