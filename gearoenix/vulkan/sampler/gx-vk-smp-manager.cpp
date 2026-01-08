#include "gx-vk-smp-manager.hpp"
#if GX_RENDER_VULKAN_ENABLED
#include "gx-vk-smp-sampler.hpp"

gearoenix::vulkan::sampler::Manager::Manager()
    : Singleton(this)
{
}

gearoenix::vulkan::sampler::Manager::~Manager() = default;

const std::shared_ptr<gearoenix::vulkan::sampler::Sampler>& gearoenix::vulkan::sampler::Manager::get(const render::texture::SamplerInfo& info)
{
    const std::lock_guard l(samplers_lock);
    auto& smp = samplers[info];
    if (nullptr == smp) {
        smp = std::make_shared<Sampler>(info);
    }
    return smp;
}

#endif