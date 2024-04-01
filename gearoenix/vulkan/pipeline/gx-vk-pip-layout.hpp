#ifndef GEAROENIX_RENDER_PIPELINE_LAYOUT_HPP
#define GEAROENIX_RENDER_PIPELINE_LAYOUT_HPP
#include "../../render/gx-rnd-build-configuration.hpp"
#ifdef GX_RENDER_VULKAN_ENABLED
#include "../../core/macro/gx-cr-mcr-getter-setter.hpp"
#include "../gx-vk-loader.hpp"
#include <memory>

namespace gearoenix::vulkan::descriptor {
struct SetLayout;
}

namespace gearoenix::vulkan::pipeline {
struct Layout final {
    GX_GET_VAL_PRV(VkPipelineLayout, vulkan_data, nullptr);
    GX_GET_CREF_PRV(std::shared_ptr<descriptor::SetLayout>, des_set_layout);

public:
    explicit Layout(std::shared_ptr<descriptor::SetLayout> des_set_layout);
    ~Layout();
};
}
#endif
#endif