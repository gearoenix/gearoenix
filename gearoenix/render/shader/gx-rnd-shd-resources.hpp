#ifndef GEAROENIX_RENDER_SHADER_RESOURCES_HPP
#define GEAROENIX_RENDER_SHADER_RESOURCES_HPP
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

        public:
            Resources(Engine* e, pipeline::Pipeline* pip, buffer::Uniform* u);
            virtual ~Resources();
            virtual void bind() = 0;
        };
    }
}
}
#endif
