#pragma once
#include "../../render/gx-rnd-build-configuration.hpp"
#if GX_RENDER_VULKAN_ENABLED
#include "../gx-vk-loader.hpp"

#include <memory>
#include <vector>

namespace gearoenix::vulkan::sync {
struct Fence {
private:
    vk::raii::Fence vulkan_data;

public:
    Fence(Fence&&) = delete;
    Fence(const Fence&) = delete;
    Fence& operator=(Fence&&) = delete;
    Fence& operator=(const Fence&) = delete;

    explicit Fence(bool signaled = false);
    ~Fence();
    void wait();
    void reset();

    [[nodiscard]] const vk::raii::Fence& get_fence() const { return vulkan_data; }
    [[nodiscard]] vk::Fence get_vulkan_data() const { return *vulkan_data; }

    [[nodiscard]] static std::vector<std::shared_ptr<Fence>> create_frame_based(bool signaled = false);
};
}
#endif
