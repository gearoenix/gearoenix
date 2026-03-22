#pragma once
#include "../../render/gx-rnd-build-configuration.hpp"
#if GX_RENDER_VULKAN_ENABLED
#include "../gx-vk-loader.hpp"

#include <memory>

namespace gearoenix::vulkan::command {
struct Pool final {
private:
    vk::raii::CommandPool vulkan_data;

public:
    Pool();
    Pool(Pool&&) = delete;
    Pool(const Pool&) = delete;
    Pool& operator=(Pool&&) = delete;
    Pool& operator=(const Pool&) = delete;
    ~Pool();

    [[nodiscard]] const vk::raii::CommandPool& get_pool() const { return vulkan_data; }
    [[nodiscard]] vk::CommandPool get_vulkan_data() const { return *vulkan_data; }
};
using PoolPtr = std::shared_ptr<Pool>;
}
#endif
