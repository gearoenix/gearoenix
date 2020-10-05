#ifndef GEAROENIX_VULKAN_TEXTURE_TARGET_HPP
#define GEAROENIX_VULKAN_TEXTURE_TARGET_HPP
#include "../../core/gx-cr-build-configuration.hpp"
#ifdef GX_USE_VULKAN
#include "../../render/texture/gx-rnd-txt-target.hpp"

namespace gearoenix::vulkan::texture {
class Target final : public render::texture::Target {
public:
    ~Target() noexcept final;
    [[nodiscard]] Target* clone() const noexcept final;
};
}
#endif
#endif