#ifndef GEAROENIX_GLES2_BUFFER_UNIFORM_HPP
#define GEAROENIX_GLES2_BUFFER_UNIFORM_HPP
#include "../../core/cr-build-configuration.hpp"
#ifdef GX_USE_OPENGL_ES2
#include "../../render/buffer/rnd-buf-uniform.hpp"
namespace gearoenix {
namespace gles2 {
    class Engine;
    namespace buffer {
        class Uniform : public render::buffer::Uniform {
        private:
            unsigned char* data = nullptr;

        public:
            Uniform(unsigned int s, Engine*);
            ~Uniform();
            void update(const void* src, unsigned int data_size);
            void* get_data();
        };
    } // namespace buffer
} // namespace gles2
} // namespace gearoenix
#endif
#endif // GEAROENIX_GLES2_BUFFER_UNIFORM_HPP
