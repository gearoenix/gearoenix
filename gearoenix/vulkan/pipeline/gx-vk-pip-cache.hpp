#pragma once
#include "../../render/gx-rnd-build-configuration.hpp"
#if GX_RENDER_VULKAN_ENABLED
#include "../gx-vk-loader.hpp"

namespace gearoenix::vulkan::pipeline {
struct Cache final {
private:
    vk::raii::PipelineCache vulkan_data { nullptr };

public:
    [[nodiscard]] vk::PipelineCache get_vulkan_data() const { return *vulkan_data; }
    [[nodiscard]] const vk::raii::PipelineCache& get_pipeline_cache() const { return vulkan_data; }
    Cache();
    Cache(Cache&&) = delete;
    Cache(const Cache&) = delete;
    ~Cache();
};
}
#endif
