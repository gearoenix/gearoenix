#ifndef GEAROENIX_GLC3_BUFFER_MANAGER_HPP
#define GEAROENIX_GLC3_BUFFER_MANAGER_HPP
#include "../../core/gx-cr-build-configuration.hpp"
#ifdef GX_USE_OPENGL_CLASS_3
#include "../../render/buffer/gx-rnd-buf-manager.hpp"

namespace gearoenix::glc3::engine {
class Engine;
}

namespace gearoenix::glc3::buffer {
class Manager final : public render::buffer::Manager {
public:
    explicit Manager(engine::Engine* e) noexcept;
    ~Manager() noexcept final = default;
    std::shared_ptr<render::buffer::Uniform> create_uniform(std::size_t size, std::size_t frame_number) noexcept final;
    std::shared_ptr<render::buffer::Static> create_static(const std::vector<math::BasicVertex>& vertices, const core::sync::EndCaller<core::sync::EndCallerIgnore>& c) noexcept final;
    std::shared_ptr<render::buffer::Static> create_static(const std::vector<std::uint32_t>& indices, const core::sync::EndCaller<core::sync::EndCallerIgnore>& c) noexcept final;
};
}
#endif
#endif
