#pragma once
#include "../../render/gx-rnd-build-configuration.hpp"
#if GX_RENDER_VULKAN_ENABLED
#include "../../core/macro/gx-cr-mcr-getter-setter.hpp"
#include "../gx-vk-loader.hpp"

#include <memory>
#include <vector>

namespace gearoenix::vulkan::sync {
struct Fence {
    GX_GET_VAL_PRV(VkFence, vulkan_data, nullptr);

public:
    Fence(Fence&&) = delete;
    Fence(const Fence&) = delete;
    Fence& operator=(Fence&&) = delete;
    Fence& operator=(const Fence&) = delete;

    explicit Fence(bool signaled = false);
    ~Fence();
    void wait();
    void reset();
    [[nodiscard]] static std::vector<std::shared_ptr<Fence>> create_frame_based(bool signaled = false);
};
}
#endif