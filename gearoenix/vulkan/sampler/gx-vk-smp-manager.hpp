#pragma once
#include "../../render/gx-rnd-build-configuration.hpp"
#if GX_RENDER_VULKAN_ENABLED
#include "../../core/gx-cr-singleton.hpp"
#include "../../render/texture/gx-rnd-txt-sampler.hpp"

#include <boost/container/flat_map.hpp>

#include <memory>
#include <mutex>

namespace gearoenix::vulkan::sampler {
struct Sampler;
struct Manager final : core::Singleton<Manager> {
private:
    std::mutex samplers_lock;
    boost::container::flat_map<render::texture::SamplerInfo, std::shared_ptr<Sampler>> samplers;

public:
    Manager();
    ~Manager() override;
    [[nodiscard]] const std::shared_ptr<Sampler>& get_sampler(const render::texture::SamplerInfo& info);
};
}
#endif