#pragma once
#include "../../render/gx-rnd-build-configuration.hpp"
#if GX_RENDER_VULKAN_ENABLED
#include "../../render/texture/gx-rnd-txt-texture-2d.hpp"
#include "../gx-vk-loader.hpp"

namespace gearoenix::vulkan::image {
struct View;
}

namespace gearoenix::vulkan::sampler {
struct Sampler;
}

namespace gearoenix::vulkan::texture {
struct Texture2D final : render::texture::Texture2D {
private:
    std::shared_ptr<image::View> view;
    std::uint32_t view_index;
    std::vector<std::shared_ptr<image::View>> mips;
    std::uint32_t sampler_index;

public:
    [[nodiscard]] const std::shared_ptr<image::View>& get_view() const { return view; }
    [[nodiscard]] std::uint32_t get_view_index() const { return view_index; }
    [[nodiscard]] const std::vector<std::shared_ptr<image::View>>& get_mips() const { return mips; }
    [[nodiscard]] std::uint32_t get_sampler_index() const { return sampler_index; }

    Texture2D(const render::texture::TextureInfo& info, std::string&&);
    ~Texture2D() override;
    void write(const std::shared_ptr<platform::stream::Stream>& s, const core::job::EndCaller<>& c, bool) const override;
};
}
#endif