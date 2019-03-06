//#ifndef GEAROEMIX_RENDER_MATERIAL_FONT_COLORED_HPP
//#define GEAROEMIX_RENDER_MATERIAL_FONT_COLORED_HPP
//#include "../../math/math-matrix.hpp"
//#include "../../math/math-vector.hpp"
//#include "../shader/rnd-shd-resources.hpp"
//#include "rnd-mat-material.hpp"
//#include <memory>
//
//namespace gearoenix {
//namespace render {
//    namespace texture {
//        class Texture2D;
//    }
//    namespace material {
//        class FontColored : public Material {
//        public:
//            const core::Id SHADER_ID;
//            class Resources : public shader::Resources {
//            protected:
//                texture::Texture2D* txt2d;
//
//            public:
//                Resources(Engine* e, pipeline::Pipeline* pip, buffer::Uniform* u);
//                void set_texture_2d(texture::Texture2D* t2d);
//            };
//            typedef struct {
//                math::Mat4x4 mvp;
//                math::Vec4 color = math::Vec4(1.0f);
//            } Uniform;
//
//        private:
//            Uniform u;
//            Resources* shdrsc;
//            std::shared_ptr<texture::Texture2D> txt2d;
//
//        public:
//            FontColored(core::Id sid, system::stream::Stream* f, Engine* e, core::sync::EndCaller<core::sync::EndCallerIgnore> end);
//            FontColored(core::Id sid, std::shared_ptr<texture::Texture2D> txt2d, Engine* e, core::sync::EndCaller<core::sync::EndCallerIgnore> end);
//            ~FontColored();
//            unsigned int get_vertex_elements_count() const;
//            core::Id get_shader_id() const;
//            bool needs_mvp() const;
//            bool needs_dbm() const;
//            void update(const scene::Scene* s, const model::Model* m);
//            void bind(texture::Texture2D* shadow_texture);
//            void set_color(const math::Vec4& color);
//        };
//    }
//}
//}
//#endif
