#pragma once
#include "../../core/gx-cr-build-configuration.hpp"
#ifdef GX_RENDER_VULKAN_ENABLED
#include "../../core/sync/gx-cr-job-end-caller.hpp"
#include "../../render/texture/gx-rnd-txt-texture-2d.hpp"
#include "../../render/texture/gx-rnd-txt-texture-info.hpp"
#include "../gx-vk-loader.hpp"

namespace gearoenix::vulkan::engine {
struct Engine;
}

namespace gearoenix::vulkan::image {
struct View;
}

namespace gearoenix::vulkan::texture {
struct Texture2D final : public render::texture::Texture2D {
    GX_GET_REFC_PRV(std::shared_ptr<image::View>, view)
public:
    Texture2D(
        core::Id id,
        std::string name,
        engine::Engine* e,
        const std::vector<std::vector<std::uint8_t>>& data,
        const render::texture::TextureInfo& info,
        std::uint64_t img_width,
        std::uint64_t img_height,
        const core::job::EndCaller<core::job::EndCallerIgnore>& call);
    ~Texture2D() final;
    void write_gx3d(
        const std::shared_ptr<platform::stream::Stream>& s,
        const gearoenix::core::job::EndCaller<gearoenix::core::job::EndCallerIgnore>&) final;
    [[nodiscard]] static VkFormat convert(render::texture::TextureFormat format);
};
}
#endif