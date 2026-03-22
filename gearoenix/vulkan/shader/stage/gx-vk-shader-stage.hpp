#pragma once
#include "../../../render/gx-rnd-build-configuration.hpp"
#if GX_RENDER_VULKAN_ENABLED
#include "../../gx-vk-loader.hpp"

#include <cstdint>
#include <span>

namespace gearoenix::vulkan::shader::stage {
struct Stage {
protected:
    vk::raii::ShaderModule vulkan_data;

public:
    [[nodiscard]] vk::ShaderModule get_vulkan_data() const { return *vulkan_data; }

    explicit Stage(std::span<std::uint8_t> data);
    virtual ~Stage();
};
}
#endif