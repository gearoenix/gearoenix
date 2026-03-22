#pragma once
#include "../../render/gx-rnd-build-configuration.hpp"
#if GX_RENDER_VULKAN_ENABLED
#include "../../render/texture/gx-rnd-txt-sampler.hpp"
#include "../gx-vk-loader.hpp"

namespace gearoenix::vulkan::sampler {
struct Sampler final {
private:
    vk::raii::Sampler vulkan_data;
    std::uint32_t bindless_index = static_cast<std::uint32_t>(-1);

public:
    [[nodiscard]] vk::Sampler get_vulkan_data() const { return *vulkan_data; }
    [[nodiscard]] std::uint32_t get_bindless_index() const { return bindless_index; }

    explicit Sampler(const render::texture::SamplerInfo& sampler_info);
    ~Sampler();
    [[nodiscard]] static vk::Filter convert(render::texture::Filter filter);
    [[nodiscard]] static vk::SamplerAddressMode convert(render::texture::Wrap wrap);
};
}
#endif
