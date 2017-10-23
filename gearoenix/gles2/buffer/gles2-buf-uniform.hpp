#ifndef GEAROENIX_GLES2_BUFFER_UNIFORM_HPP
#define GEAROENIX_GLES2_BUFFER_UNIFORM_HPP
#include "../../core/cr-build-configuration.hpp"
#ifdef USE_OPENGL_ES2
#include "../../render/buffer/rnd-buf-uniform.hpp"
namespace gearoenix {
namespace gles2 {
    class Engine;
    namespace buffer {
        class Uniform : public render::buffer::Uniform {
        private:
        public:
            Uniform(Engine*);
            ~Uniform();
            void update(const void* data, unsigned int data_size);
        };
    } // namespace buffer
} // namespace gles2
} // namespace gearoenix
#endif
#endif // GEAROENIX_GLES2_BUFFER_UNIFORM_HPP
