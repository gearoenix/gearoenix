//#ifndef GEAROEMIX_RENDER_MATERIAL_MATERIAL_HPP
//#define GEAROEMIX_RENDER_MATERIAL_MATERIAL_HPP
//#include "../../core/sync/cr-sync-end-caller.hpp"
//#include "../shader/rnd-shd-shader.hpp"
//#include <memory>
//namespace gearoenix {
//namespace system {
//    class File;
//}
//namespace render {
//    class Engine;
//    namespace buffer {
//        class Uniform;
//    }
//    namespace model {
//        class Model;
//    }
//    namespace pipeline {
//        class Pipeline;
//    }
//    namespace scene {
//        class Scene;
//    }
//    namespace texture {
//        class Texture2D;
//    }
//    namespace material {
//        class Material {
//        private:
//        protected:
//            std::shared_ptr<pipeline::Pipeline> pl;
//            buffer::Uniform* ub;
//            Engine* e;
//            Material(core::Id sid, unsigned int us, Engine* e, core::sync::EndCaller<core::sync::EndCallerIgnore> end);
//
//        public:
//            virtual ~Material();
//            virtual void update(const scene::Scene* s, const model::Model* m) = 0;
//            virtual void bind(texture::Texture2D* shadow_texture) = 0;
//            virtual unsigned int get_vertex_elements_count() const = 0;
//            virtual core::Id get_shader_id() const = 0;
//            virtual bool needs_mvp() const = 0;
//            virtual bool needs_dbm() const = 0;
//            static Material* read(system::stream::Stream* f, Engine* e, core::sync::EndCaller<core::sync::EndCallerIgnore> end);
//        };
//    }
//}
//}
//#endif
