#ifndef GEAROENIX_VULKAN_TEXTURE_TARGET_HPP
#define GEAROENIX_VULKAN_TEXTURE_TARGET_HPP
#include "../../core/gx-cr-build-configuration.hpp"
#ifdef GX_USE_VULKAN
#include "../../core/sync/gx-cr-sync-end-caller.hpp"
#include "../../render/texture/gx-rnd-txt-target.hpp"

namespace gearoenix::vulkan {
class Framebuffer;
class RenderPass;
}

namespace gearoenix::vulkan::engine {
class Engine;
}

namespace gearoenix::vulkan::memory {
class Manager;
}

namespace gearoenix::vulkan::sync {
class Semaphore;
}

namespace gearoenix::vulkan::texture {
class Target final : public render::texture::Target {
    GX_GET_CREF_PRV(std::shared_ptr<RenderPass>, render_pass)
    GX_GET_CREF_PRV(std::shared_ptr<sync::Semaphore>, start_semaphore)
    GX_GET_CREF_PRV(std::shared_ptr<sync::Semaphore>, end_semaphore)
    GX_GET_CREF_PRV(std::shared_ptr<Framebuffer>, framebuffer)

    Target(const Target& o) noexcept;

public:
    Target(
        core::Id id,
        const std::vector<render::texture::AttachmentInfo>& infos,
        engine::Engine* e,
        const core::sync::EndCaller<core::sync::EndCallerIgnore>& call) noexcept;
    ~Target() noexcept final;
    [[nodiscard]] Target* clone() const noexcept final;
};
}
#endif
#endif