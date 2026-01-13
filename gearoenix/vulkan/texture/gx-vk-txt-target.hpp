#pragma once
#include "../../render/gx-rnd-build-configuration.hpp"
#ifdef GX_RENDER_VULKAN_ENABLED
#include "../../core/job/gx-cr-job-end-caller.hpp"
#include "../../core/macro/gx-cr-mcr-getter-setter.hpp"
#include "../../render/texture/gx-rnd-txt-target.hpp"
#include "../gx-vk-loader.hpp"

#include <memory>
#include <variant>
#include <vector>

namespace gearoenix::vulkan::texture {
struct Texture2D;
struct TextureCube;

struct Target final : render::texture::Target {
    struct RenderingScope final {
    private:
        const VkCommandBuffer command_buffer;

    public:
        explicit RenderingScope(VkCommandBuffer cb);
        ~RenderingScope();
    };

    struct GapiAttachment final {
        constexpr static std::uint32_t VARIANT_2D_INDEX = 0;
        constexpr static std::uint32_t VARIANT_CUBE_INDEX = 1;

        std::variant<std::shared_ptr<Texture2D>, std::shared_ptr<TextureCube>> texture;

        VkExtent3D extent = {};
        VkImageView view = VK_NULL_HANDLE;
        VkImageLayout layout = VK_IMAGE_LAYOUT_UNDEFINED;
        VkClearValue clear = {};
    };

    GX_GET_CREF_PRV(std::vector<GapiAttachment>, gapi_attachments);

    Target(std::string&&, std::vector<render::texture::Attachment>&& attachments);

public:
    static void construct(
        std::string&& name, std::vector<render::texture::Attachment>&& attachments,
        core::job::EndCallerShared<render::texture::Target>&& end_callback);
    ~Target() override;
    void bind() const;
    [[nodiscard]] RenderingScope create_rendering_scope() const;
};
}

#endif