#pragma once
#include "../../render/gx-rnd-build-configuration.hpp"
#if GX_RENDER_VULKAN_ENABLED
#include "../gx-vk-loader.hpp"

#include <cstdint>
#include <span>

namespace gearoenix::vulkan::shader {
struct Module final {
private:
    vk::raii::ShaderModule vulkan_data;

public:
    [[nodiscard]] vk::ShaderModule get_vulkan_data() const { return *vulkan_data; }
    Module(Module&&) = delete;
    Module(const Module&) = delete;
    Module& operator=(Module&&) = delete;
    Module& operator=(const Module&) = delete;
    explicit Module(std::span<const std::uint8_t> file_content);
    ~Module();
};
}
#endif
