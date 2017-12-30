#ifndef GEAROENIX_gles3_BUFFER_UNIFORM_HPP
#define GEAROENIX_gles3_BUFFER_UNIFORM_HPP
#include "../../core/cr-build-configuration.hpp"
#ifdef USE_OPENGL_ES3
#include "../../render/buffer/rnd-buf-uniform.hpp"
namespace gearoenix {
namespace gles3 {
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
} // namespace gles3
} // namespace gearoenix
#endif
#endif // GEAROENIX_gles3_BUFFER_UNIFORM_HPP
