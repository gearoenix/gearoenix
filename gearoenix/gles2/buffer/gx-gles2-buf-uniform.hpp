#ifndef GEAROENIX_GLES2_BUFFER_UNIFORM_HPP
#define GEAROENIX_GLES2_BUFFER_UNIFORM_HPP
#include "../../core/gx-cr-build-configuration.hpp"
#ifdef GX_USE_OPENGL_ES2
#include "../../render/buffer/gx-rnd-buf-uniform.hpp"
namespace gearoenix::gles2 {
namespace engine {
    class Engine;
}
namespace buffer {
    class Uniform final : public render::buffer::Uniform {
    private:
        std::vector<unsigned char> data;

        [[nodiscard]] const void* get_data() const noexcept final;
        [[nodiscard]] void* get_data() noexcept final;
        void update(const void* src) noexcept final;

    public:
        Uniform(std::size_t s, std::size_t frame_number, engine::Engine* e) noexcept;
        ~Uniform() noexcept final;
    };
} // namespace buffer
} // namespace gearoenix
#endif
#endif // GEAROENIX_GLES2_BUFFER_UNIFORM_HPP
