//#ifndef GEAROEMIX_RENDER_MATERIAL_DIRECTIONAL_D2_SPECULATED_BAKED_FULL_OPAQUE_HPP
//#define GEAROEMIX_RENDER_MATERIAL_DIRECTIONAL_D2_SPECULATED_BAKED_FULL_OPAQUE_HPP
//#include "../../math/math-matrix.hpp"
//#include "../../math/math-vector.hpp"
//#include "../shader/rnd-shd-resources.hpp"
//#include "rnd-mat-material.hpp"
//#include <memory>
//namespace gearoenix {
//namespace render {
//    namespace texture {
//        class Texture2D;
//        class Cube;
//    }
//    namespace material {
//        class DirectionalD2SpeculatedBakedFullOpaque : public Material {
//        public:
//            const static core::Id SHADER_ID;
//            class Resources : public shader::Resources {
//            protected:
//                texture::Cube* env;
//                texture::Texture2D* txt;
//                texture::Texture2D* shdtxt;
//
//            public:
//                Resources(Engine* e, pipeline::Pipeline* pip, buffer::Uniform* u);
//                void set_texture(texture::Texture2D* t);
//                void set_baked_env(texture::Cube* t);
//                void set_shadow_texture(texture::Texture2D* t);
//            };
//            typedef struct {
//                math::Vec3 ambl_color;
//                core::Real ambl_alpha = 1.0f; // this is a temporary workaround in future this must be fed by blender.
//                math::Mat4x4 db;
//                math::Vec3 eye;
//                core::Real padding1;
//                math::Mat4x4 m;
//                core::Real rfl_fac = 0.5f;
//                math::Vec3 padding2;
//                math::Vec3 spec_color;
//                core::Real spec_alpha = 1.0f; // this is a temporary workaround in future this must be fed by blender.
//                math::Vec3 spec_factors;
//                core::Real padding3;
//                math::Vec3 sun;
//                core::Real padding4;
//                math::Vec3 sun_color;
//                core::Real padding5 = 1.0f;
//                math::Mat4x4 vp;
//            } Uniform;
//
//        private:
//            Uniform u;
//            std::shared_ptr<texture::Texture2D> txt;
//            std::shared_ptr<texture::Cube> env;
//            Resources* shdrsc;
//
//        public:
//            DirectionalD2SpeculatedBakedFullOpaque(system::stream::Stream* f, Engine* e, core::sync::EndCaller<core::sync::EndCallerIgnore> end);
//            ~DirectionalD2SpeculatedBakedFullOpaque();
//            unsigned int get_vertex_elements_count() const;
//            core::Id get_shader_id() const;
//            bool needs_mvp() const;
//            bool needs_dbm() const;
//            void update(const scene::Scene* s, const model::Model* m);
//            void bind(texture::Texture2D* shadow_texture);
//        };
//    }
//}
//}
//#endif
