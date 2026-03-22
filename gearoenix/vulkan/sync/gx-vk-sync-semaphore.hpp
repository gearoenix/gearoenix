#pragma once
#include "../../render/gx-rnd-build-configuration.hpp"
#if GX_RENDER_VULKAN_ENABLED
#include "../gx-vk-loader.hpp"

#include <memory>
#include <string>
#include <vector>

namespace gearoenix::vulkan::sync {
struct Semaphore final {
private:
    vk::raii::Semaphore vulkan_data;
    vk::PipelineStageFlags pipeline_stage = vk::PipelineStageFlagBits::eTopOfPipe;

public:
    Semaphore(Semaphore&&) = delete;
    Semaphore(const Semaphore&) = delete;
    Semaphore& operator=(Semaphore&&) = delete;
    Semaphore& operator=(const Semaphore&) = delete;
    explicit Semaphore(const std::string&);
    ~Semaphore();

    [[nodiscard]] const vk::raii::Semaphore& get_semaphore() const { return vulkan_data; }
    [[nodiscard]] vk::Semaphore get_vulkan_data() const { return *vulkan_data; }
    [[nodiscard]] vk::PipelineStageFlags get_pipeline_stage() const { return pipeline_stage; }
    void set_pipeline_stage(vk::PipelineStageFlags stage) { pipeline_stage = stage; }

    [[nodiscard]] static std::vector<std::shared_ptr<Semaphore>> create_frame_based(const std::string& base_name);
};
}
#endif
