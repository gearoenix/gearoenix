#pragma once
#include "../../render/gx-rnd-build-configuration.hpp"
#if GX_RENDER_VULKAN_ENABLED
#include "../../render/texture/gx-rnd-txt-texture-cube.hpp"
#include "../../render/texture/gx-rnd-txt-texture-info.hpp"

namespace gearoenix::vulkan::image {
struct View;
}

namespace gearoenix::vulkan::texture {
struct TextureCube final : render::texture::TextureCube {
    using mips_t = std::array<std::vector<std::shared_ptr<image::View>>, 6>;

    GX_GET_REFC_PRV(std::shared_ptr<image::View>, view);
    GX_GET_REFC_PRV(mips_t, mips);

public:
    TextureCube(const render::texture::TextureInfo& info, std::string&& name);
    ~TextureCube() override;
    void write(const std::shared_ptr<platform::stream::Stream>& s, const core::job::EndCaller<>& c, bool) const override;
};
}
#endif