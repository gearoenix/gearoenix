#ifndef GEAROENIX_VULKAN_BUFFER_UNIFORM_HPP
#define GEAROENIX_VULKAN_BUFFER_UNIFORM_HPP
#include "../../render/gx-rnd-build-configuration.hpp"
#ifdef GX_RENDER_VULKAN_ENABLED
#include <cstddef>

namespace gearoenix::vulkan::engine {
struct Engine;
}

namespace gearoenix::vulkan::buffer {
struct Dynamic final {
    engine::Engine* const e;
    const std::size_t size;

public:
    Dynamic(std::size_t size, engine::Engine* e) noexcept;
    ~Dynamic() noexcept;
};
}
#endif
#endif