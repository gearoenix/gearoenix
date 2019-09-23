#ifndef GEAROENIX_RENDER_BUFFER_FRAMED_UNIFORM_HPP
#define GEAROENIX_RENDER_BUFFER_FRAMED_UNIFORM_HPP
#include "../engine/rnd-eng-engine.hpp"
#include "rnd-buf-uniform.hpp"
#include <memory>
#include <vector>
namespace gearoenix::render {
namespace engine {
    class Engine;
}
namespace buffer {
    class Uniform;
    class FramedUniform {
    protected:
        const engine::Engine* const e;
        std::vector<std::unique_ptr<Uniform>> uniforms;

    public:
        FramedUniform(std::size_t s, engine::Engine* e) noexcept;
        ~FramedUniform() noexcept;
        // Update current frame uniform buffer
        template<typename T> void update(const T& data) noexcept;
        // return current frame uniform buffer
        const Uniform* get_buffer() const noexcept;
        Uniform* get_buffer() noexcept;
    };

    template<typename T>
    inline void FramedUniform::update(const T& data) noexcept
    {
        uniforms[e->get_frame_number()]->set_data(data);
    }
}
}
#endif