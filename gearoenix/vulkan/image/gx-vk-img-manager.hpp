#ifndef GEAROENIX_VULKAN_IMAGE_MANAGER_HPP
#define GEAROENIX_VULKAN_IMAGE_MANAGER_HPP
#include "../../render/gx-rnd-build-configuration.hpp"
#ifdef GX_RENDER_VULKAN_ENABLED
#include "../../core/macro/gx-cr-mcr-getter-setter.hpp"
#include "../../core/sync/gx-cr-sync-end-caller.hpp"
#include <tuple>
#include <vector>

namespace gearoenix::vulkan::buffer {
struct Buffer;
}

namespace gearoenix::vulkan::command {
struct Buffer;
}

namespace gearoenix::vulkan::engine {
struct Engine;
}

namespace gearoenix::vulkan::image {
struct Image;
struct Manager final {
private:
    engine::Engine& e;

public:
    explicit Manager(engine::Engine& e) noexcept;
    ~Manager() noexcept;
};
}

#endif
#endif