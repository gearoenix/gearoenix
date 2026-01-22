#pragma once
#include "../../render/gx-rnd-build-configuration.hpp"
#if GX_RENDER_VULKAN_ENABLED
#include "../../render/scene/gx-rnd-scn-manager.hpp"
#include "../gx-vk-loader.hpp"

#include <vector>

struct GxShaderDataScene;

namespace gearoenix::vulkan::buffer {
struct Uniform;
}

namespace gearoenix::vulkan::command {
struct Buffer;
}

namespace gearoenix::vulkan::scene {
struct Manager final : render::scene::Manager, core::Singleton<Manager> {
    Manager();
    ~Manager() override;
    [[nodiscard]] core::ecs::EntityPtr build(std::string&& name, double layer) const override;
    [[nodiscard]] static std::pair<GxShaderDataScene*, std::uint32_t> get_shader_data();
    void update() const override;
    void submit(VkCommandBuffer vk_cmd);
    void render_forward(VkCommandBuffer vk_cmd);
    [[nodiscard]] static const buffer::Uniform& get_uniform_buffer();
};
}
#endif