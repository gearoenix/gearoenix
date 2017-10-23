#ifndef GEAROEMIX_RENDER_SHADER_RESOURCES_HPP
#define GEAROEMIX_RENDER_SHADER_RESOURCES_HPP
namespace gearoenix {
namespace render {
    class Engine;
    namespace buffer {
        class Uniform;
    }
    namespace pipeline {
        class Pipeline;
    }
    namespace texture {
        class Texture2D;
    }
    namespace shader {
        class Resources {
        protected:
            Engine* e = nullptr;
            pipeline::Pipeline* pip = nullptr;
            buffer::Uniform* u = nullptr;
            texture::Texture2D* t = nullptr;

        public:
            Resources(Engine* e, pipeline::Pipeline* pip, buffer::Uniform* u);
            Resources(Engine* e, pipeline::Pipeline* pip, buffer::Uniform* u, texture::Texture2D* t);
            virtual ~Resources();
            virtual void bind(pipeline::Pipeline* pip) = 0;
        };
    }
}
}
#endif
