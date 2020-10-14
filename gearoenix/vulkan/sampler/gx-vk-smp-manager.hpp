#ifndef GEAROENIX_VULKAN_SAMPLER_MANAGER_HPP
#define GEAROENIX_VULKAN_SAMPLER_MANAGER_HPP
#include "../../core/gx-cr-build-configuration.hpp"
#ifdef GX_USE_VULKAN
#include "../../core/gx-cr-static.hpp"
#include "../../render/texture/gx-rnd-txt-sampler.hpp"
#include "../gx-vk-loader.hpp"
#include <map>

namespace gearoenix::vulkan::device {
class Logical;
}

namespace gearoenix::vulkan::sampler {
class Sampler;
class Manager final {
    const std::shared_ptr<device::Logical> logical_device;
    GX_CREATE_GUARD(samplers)
    std::map<render::texture::SamplerInfo, std::shared_ptr<Sampler>> samplers;

public:
    explicit Manager(std::shared_ptr<device::Logical> logical_device) noexcept;
    ~Manager() noexcept;
    [[nodiscard]] const std::shared_ptr<Sampler>& get(const render::texture::SamplerInfo& info) noexcept;
};
}
#endif
#endif
