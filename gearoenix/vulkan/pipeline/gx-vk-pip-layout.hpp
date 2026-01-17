#pragma once
#include "../../render/gx-rnd-build-configuration.hpp"
#if GX_RENDER_VULKAN_ENABLED
#include "../../core/macro/gx-cr-mcr-getter-setter.hpp"
#include "../gx-vk-loader.hpp"

#include <memory>
#include <vector>

namespace gearoenix::vulkan::descriptor {
struct SetLayout;
}

namespace gearoenix::vulkan::pipeline {
struct Layout final {
    GX_GET_VAL_PRV(VkPipelineLayout, vulkan_data, nullptr);
    GX_GET_CREF_PRV(std::vector<std::shared_ptr<descriptor::SetLayout>>, des_set_layouts);

public:
    explicit Layout(std::vector<std::shared_ptr<descriptor::SetLayout>>&&);
    ~Layout();
};
}
#endif