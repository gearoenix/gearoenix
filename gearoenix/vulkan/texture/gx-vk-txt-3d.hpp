#pragma once
#include "../../render/gx-rnd-build-configuration.hpp"
#if GX_RENDER_VULKAN_ENABLED
#include "../../render/texture/gx-rnd-txt-texture-3d.hpp"
#include "../gx-vk-loader.hpp"

namespace gearoenix::vulkan::image {
struct View;
}

namespace gearoenix::vulkan::sampler {
struct Sampler;
}

namespace gearoenix::vulkan::texture {
struct Texture3D final : render::texture::Texture3D {
private:
    std::shared_ptr<image::View> view;

public:
    [[nodiscard]] const std::shared_ptr<image::View>& get_view() const { return view; }

    Texture3D(const render::texture::TextureInfo& info, std::string&&);
    ~Texture3D() override;
    void write(const std::shared_ptr<platform::stream::Stream>& s, const core::job::EndCaller<>& c, bool) const override;
};
}
#endif
