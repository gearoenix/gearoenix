#pragma once
#include "../../render/gx-rnd-build-configuration.hpp"
#if GX_RENDER_VULKAN_ENABLED
#include "../../render/texture/gx-rnd-txt-texture-2d.hpp"
#include "../gx-vk-loader.hpp"

namespace gearoenix::vulkan::image {
struct View;
}

namespace gearoenix::vulkan::sampler {
struct Sampler;
}

namespace gearoenix::vulkan::texture {
struct Texture2D final : render::texture::Texture2D {
private:
    std::shared_ptr<image::View> view;
    std::vector<std::shared_ptr<image::View>> mips;
    mutable VkDescriptorSet imgui_descriptor_set = VK_NULL_HANDLE;

public:
    [[nodiscard]] const std::shared_ptr<image::View>& get_view() const { return view; }
    [[nodiscard]] const std::vector<std::shared_ptr<image::View>>& get_mips() const { return mips; }

    Texture2D(const render::texture::TextureInfo& info, std::string&&);
    ~Texture2D() override;
    void write(const std::shared_ptr<platform::stream::Stream>& s, const core::job::EndCaller<>& c, bool) const override;
    [[nodiscard]] void* get_imgui_ptr() const override;
};
}
#endif