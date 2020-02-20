#ifndef GEAROENIX_GLES2_BUFFER_MANAGER_HPP
#define GEAROENIX_GLES2_BUFFER_MANAGER_HPP
#include "../../core/cr-build-configuration.hpp"
#ifdef GX_USE_OPENGL_ES2
#include "../../render/buffer/rnd-buf-manager.hpp"
namespace gearoenix::gles2 {
namespace engine {
    class Engine;
}
namespace buffer {
    class Manager final : public render::buffer::Manager {
    public:
        explicit Manager(engine::Engine* e) noexcept;
        ~Manager() noexcept final = default;
        render::buffer::Uniform* create_uniform(std::size_t size) noexcept final;
        render::buffer::Static* create_static(const std::vector<math::BasicVertex>& vertices, const core::sync::EndCaller<core::sync::EndCallerIgnore>& c) noexcept final;
        render::buffer::Static* create_static(const std::vector<std::uint32_t>& indices, const core::sync::EndCaller<core::sync::EndCallerIgnore>& c) noexcept final;
    };
}
}
#endif
#endif
