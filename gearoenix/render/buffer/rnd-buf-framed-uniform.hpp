#ifndef GEAROENIX_RENDER_BUFFER_FRAMED_UNIFORM_HPP
#define GEAROENIX_RENDER_BUFFER_FRAMED_UNIFORM_HPP
namespace gearoenix::render {
namespace engine {
    class Engine;
}
namespace buffer {
    class Uniform;
    class FramedUniform {
    protected:
        const engine::Engine* const e;
        Uniform** uniforms;

    public:
        FramedUniform(unsigned int s, engine::Engine* e) noexcept;
        ~FramedUniform() noexcept;
        // Update current frame uniform buffer
        void update(const void* data) noexcept;
        // return current frame uniform buffer
        const Uniform* get_buffer() const noexcept;
        Uniform* get_buffer() noexcept;
    };
}
}
#endif