#pragma once
#include "../../render/gx-rnd-build-configuration.hpp"
#if GX_RENDER_VULKAN_ENABLED
#include "../../render/texture/gx-rnd-txt-texture-cube.hpp"
#include "../../render/texture/gx-rnd-txt-texture-info.hpp"
#include "../gx-vk-loader.hpp"

#include <array>

namespace gearoenix::vulkan::image {
struct View;
}

namespace gearoenix::vulkan::texture {
struct TextureCube final : render::texture::TextureCube {
    using mips_t = std::array<std::vector<std::shared_ptr<image::View>>, 6>;

private:
    std::shared_ptr<image::View> view;
    mips_t mips;

public:
    [[nodiscard]] const std::shared_ptr<image::View>& get_view() const { return view; }
    [[nodiscard]] const mips_t& get_mips() const { return mips; }

    TextureCube(const render::texture::TextureInfo& info, std::string&&);
    ~TextureCube() override;
    void write(const std::shared_ptr<platform::stream::Stream>& s, const core::job::EndCaller<>& c, bool) const override;
    void generate_mipmap(vk::CommandBuffer cmd);
};
}
#endif