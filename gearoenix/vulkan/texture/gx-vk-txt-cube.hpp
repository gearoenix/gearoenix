#pragma once
#include "../../core/gx-cr-build-configuration.hpp"
#ifdef GX_RENDER_VULKAN_ENABLED
#include "../../core/sync/gx-cr-job-end-caller.hpp"
#include "../../render/texture/gx-rnd-txt-texture-cube.hpp"
#include "../../render/texture/gx-rnd-txt-texture-info.hpp"

namespace gearoenix::vulkan::engine {
struct Engine;
}

namespace gearoenix::vulkan::image {
struct View;
}

namespace gearoenix::vulkan::texture {
struct TextureCube final : public render::texture::TextureCube {
    GX_GET_REFC_PRV(std::shared_ptr<image::View>, view)
public:
    TextureCube(
        core::Id id,
        std::string name,
        engine::Engine* engine,
        std::vector<std::vector<std::vector<std::uint8_t>>> data,
        const render::texture::TextureInfo& info,
        std::uint64_t aspect,
        const core::job::EndCaller<core::job::EndCallerIgnore>& call);
    ~TextureCube() override;
    void write_gx3d(
        const std::shared_ptr<platform::stream::Stream>& s,
        const core::job::EndCaller<core::job::EndCallerIgnore>& c) override;
};
}
#endif