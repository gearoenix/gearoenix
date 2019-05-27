#ifndef GEAROENIX_GLES2_BUFFER_UNIFORM_HPP
#define GEAROENIX_GLES2_BUFFER_UNIFORM_HPP
#include "../../core/cr-build-configuration.hpp"
#ifdef GX_USE_OPENGL_ES2
#include "../../render/buffer/rnd-buf-uniform.hpp"
namespace gearoenix::gles2 {
    namespace engine {
        class Engine;
    }
    namespace buffer {
        class Uniform : public render::buffer::Uniform {
        private:
            unsigned char* data = nullptr;

        public:
            Uniform(unsigned int s, engine::Engine* e) noexcept ;
            ~Uniform() noexcept final;
            void update(const void* src) noexcept final;
            const void* get_data() noexcept final;
        };
    } // namespace buffer
} // namespace gearoenix
#endif
#endif // GEAROENIX_GLES2_BUFFER_UNIFORM_HPP
