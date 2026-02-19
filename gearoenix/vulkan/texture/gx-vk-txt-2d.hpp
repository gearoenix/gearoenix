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
    GX_GET_REFC_PRV(std::shared_ptr<image::View>, view);
    GX_GET_CVAL_PRV(std::uint32_t, view_index);
    GX_GET_REFC_PRV(std::vector<std::shared_ptr<image::View>>, mips);
    GX_GET_CVAL_PRV(std::uint32_t, sampler_index);

public:
    Texture2D(const render::texture::TextureInfo& info, std::string&&);
    ~Texture2D() override;
    void write(const std::shared_ptr<platform::stream::Stream>& s, const core::job::EndCaller<>& c, bool) const override;
};
}
#endif