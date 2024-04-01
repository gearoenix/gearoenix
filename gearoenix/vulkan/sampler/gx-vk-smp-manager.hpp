#ifndef GEAROENIX_VULKAN_SAMPLER_MANAGER_HPP
#define GEAROENIX_VULKAN_SAMPLER_MANAGER_HPP
#include "../../render/gx-rnd-build-configuration.hpp"
#ifdef GX_RENDER_VULKAN_ENABLED
#include "../../core/macro/gx-cr-mcr-getter-setter.hpp"
#include "../../platform/macro/gx-plt-mcr-lock.hpp"
#include "../../render/texture/gx-rnd-txt-sampler.hpp"
#include "../gx-vk-loader.hpp"
#include <map>

namespace gearoenix::vulkan::device {
struct Logical;
}

namespace gearoenix::vulkan::sampler {
struct Sampler;
struct Manager final {
    const std::shared_ptr<device::Logical> logical_device;
    GX_CREATE_GUARD(samplers)
    std::map<render::texture::SamplerInfo, std::shared_ptr<Sampler>> samplers;

public:
    explicit Manager(std::shared_ptr<device::Logical> logical_device);
    ~Manager();
    [[nodiscard]] const std::shared_ptr<Sampler>& get(const render::texture::SamplerInfo& info);
};
}
#endif
#endif
