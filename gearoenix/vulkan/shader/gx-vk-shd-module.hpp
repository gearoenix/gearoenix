#pragma once
#include "../../render/gx-rnd-build-configuration.hpp"
#if GX_RENDER_VULKAN_ENABLED
#include "../../core/macro/gx-cr-mcr-getter-setter.hpp"
#include "../gx-vk-loader.hpp"

#include <cstdint>
#include <span>

namespace gearoenix::vulkan::shader {
struct Module final {
    GX_GET_VAL_PRV(VkShaderModule, vulkan_data, nullptr);

public:
    Module(Module&&) = delete;
    Module(const Module&) = delete;
    Module& operator=(Module&&) = delete;
    Module& operator=(const Module&) = delete;
    explicit Module(std::span<const std::uint8_t> file_content);
    ~Module();
};
}
#endif