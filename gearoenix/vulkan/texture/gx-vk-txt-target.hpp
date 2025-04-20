#pragma once
#include "../../render/gx-rnd-build-configuration.hpp"
#ifdef GX_RENDER_VULKAN_ENABLED
#include "../../core/sync/gx-cr-job-end-caller.hpp"
#include "../../render/texture/gx-rnd-txt-target.hpp"

namespace gearoenix::vulkan {
struct Framebuffer;
struct RenderPass;
}

namespace gearoenix::vulkan::engine {
struct Engine;
}

namespace gearoenix::vulkan::memory {
struct Manager;
}

namespace gearoenix::vulkan::sync {
struct Semaphore;
}

namespace gearoenix::vulkan::texture {
struct Target final : public render::texture::Target {
    GX_GET_CREF_PRV(std::shared_ptr<RenderPass>, render_pass)
    GX_GET_CREF_PRV(std::shared_ptr<sync::Semaphore>, start_semaphore)
    GX_GET_CREF_PRV(std::shared_ptr<sync::Semaphore>, end_semaphore)
    GX_GET_CREF_PRV(std::shared_ptr<Framebuffer>, framebuffer)

    Target(const Target& o);

public:
    Target(
        core::Id id,
        const std::vector<render::texture::AttachmentInfo>& infos,
        engine::Engine* e,
        const core::job::EndCaller<core::job::EndCallerIgnore>& call);
    ~Target() override;
    [[nodiscard]] Target* clone() const override;
};
}
#endif