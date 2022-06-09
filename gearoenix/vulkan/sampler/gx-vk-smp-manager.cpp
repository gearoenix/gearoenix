#include "gx-vk-smp-manager.hpp"
#ifdef GX_RENDER_VULKAN_ENABLED
#include "gx-vk-smp-sampler.hpp"

gearoenix::vulkan::sampler::Manager::Manager(std::shared_ptr<device::Logical> logical_device) noexcept
    : logical_device(std::move(logical_device))
{
}

gearoenix::vulkan::sampler::Manager::~Manager() noexcept = default;

const std::shared_ptr<gearoenix::vulkan::sampler::Sampler>& gearoenix::vulkan::sampler::Manager::get(
    const gearoenix::render::texture::SamplerInfo& info) noexcept
{
    GX_GUARD_LOCK(samplers)
    auto& smp = samplers[info];
    //    if (nullptr == smp)
    // smp = std::make_shared<Sampler>(logical_device, info);
    return smp;
}

#endif
