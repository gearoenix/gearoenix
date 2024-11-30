#pragma once
#include "../../render/gx-rnd-build-configuration.hpp"
#ifdef GX_RENDER_VULKAN_ENABLED
#include "../../core/macro/gx-cr-mcr-getter-setter.hpp"
#include "../gx-vk-loader.hpp"
#include <string>

namespace gearoenix::platform::stream {
struct Path;
}

namespace gearoenix::vulkan::device {
struct Logical;
}

namespace gearoenix::vulkan::engine {
struct Engine;
}

namespace gearoenix::vulkan::shader {
struct Module final {
    GX_GET_VAL_PRV(VkShaderModule, vulkan_data, nullptr);
    GX_GET_CRRF_PRV(device::Logical, logical_device);

public:
    Module(Module&&) = delete;
    Module(const Module&) = delete;
    Module& operator=(Module&&) = delete;
    Module& operator=(const Module&) = delete;
    Module(const engine::Engine& e, const std::string& file_address);
    ~Module();
};
}
#endif