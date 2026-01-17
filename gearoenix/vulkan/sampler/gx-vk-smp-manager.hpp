#pragma once
#include "../../render/gx-rnd-build-configuration.hpp"
#if GX_RENDER_VULKAN_ENABLED
#include "../../core/macro/gx-cr-mcr-getter-setter.hpp"
#include "../../core/gx-cr-singleton.hpp"
#include "../gx-vk-loader.hpp"
#include "../../render/texture/gx-rnd-txt-sampler.hpp"

#include <map>
#include <mutex>
#include <memory>

namespace gearoenix::vulkan::sampler {
struct Sampler;
struct Manager final: core::Singleton<Manager> {
    std::mutex samplers_lock;
    std::map<render::texture::SamplerInfo, std::shared_ptr<Sampler>> samplers;

    Manager();
    ~Manager() override;
    [[nodiscard]] const std::shared_ptr<Sampler>& get(const render::texture::SamplerInfo& info);
};
}
#endif