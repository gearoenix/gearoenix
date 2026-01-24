#pragma once
#include "../../render/gx-rnd-build-configuration.hpp"
#if GX_RENDER_VULKAN_ENABLED
#include "../../core/macro/gx-cr-mcr-getter-setter.hpp"
#include "../gx-vk-loader.hpp"

#include <memory>

namespace gearoenix::vulkan::command {
struct Pool final {
    GX_GET_VAL_PRV(VkCommandPool, vulkan_data, nullptr);

public:
    Pool();
    Pool(Pool&&) = delete;
    Pool(const Pool&) = delete;
    Pool& operator=(Pool&&) = delete;
    Pool& operator=(const Pool&) = delete;
    ~Pool();
};
using PoolPtr = std::shared_ptr<Pool>;
}
#endif