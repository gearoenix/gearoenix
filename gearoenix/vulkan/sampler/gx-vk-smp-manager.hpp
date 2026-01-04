#pragma once
#include "../../render/gx-rnd-build-configuration.hpp"
#if GX_RENDER_VULKAN_ENABLED
#include "../../core/macro/gx-cr-mcr-getter-setter.hpp"
#include "../../render/texture/gx-rnd-txt-sampler.hpp"
#include "../gx-vk-loader.hpp"

#include <map>
#include <mutex>
#include <memory>

namespace gearoenix::vulkan::device {
struct Logical;
}

namespace gearoenix::vulkan::sampler {
struct Sampler;
struct Manager final {
    const std::shared_ptr<device::Logical> logical_device;
    std::mutex samplers_lock;
    std::map<render::texture::SamplerInfo, std::shared_ptr<Sampler>> samplers;

    explicit Manager(std::shared_ptr<device::Logical> logical_device);
    ~Manager();
    [[nodiscard]] const std::shared_ptr<Sampler>& get(const render::texture::SamplerInfo& info);
};
}
#endif