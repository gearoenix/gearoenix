#pragma once
#include "../../render/gx-rnd-build-configuration.hpp"
#ifdef GX_RENDER_VULKAN_ENABLED
#include "../../render/texture/gx-rnd-txt-manager.hpp"

namespace gearoenix::vulkan::texture {
struct Manager final : render::texture::Manager, core::Singleton<Manager> {
    Manager();
    ~Manager() override;

    void create_2d_from_pixels_v(std::string&& name, std::vector<std::vector<std::uint8_t>>&& pixels, const render::texture::TextureInfo& info, core::job::EndCallerShared<render::texture::Texture2D>&& c) override;
    void create_cube_from_pixels_v(std::string&& name, std::vector<std::vector<std::vector<std::uint8_t>>>&& pixels, const render::texture::TextureInfo& info, core::job::EndCallerShared<render::texture::TextureCube>&& c) override;
    void create_target_v(std::string&& name, std::vector<render::texture::Attachment>&& attachments, core::job::EndCallerShared<render::texture::Target>&& c) override;
};
}
#endif
