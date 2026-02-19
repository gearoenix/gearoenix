#pragma once
#include "../../render/gx-rnd-build-configuration.hpp"
#if GX_RENDER_VULKAN_ENABLED
#include "../../render/scene/gx-rnd-scn-manager.hpp"
#include "../descriptor/gx-vk-des-uniform-indexer.hpp"
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
    GX_GET_CREF_PRV(descriptor::UniformIndexer<GxShaderDataScene>, uniform_indexer);

public:
    Manager();
    ~Manager() override;
    [[nodiscard]] core::ecs::EntityPtr build(std::string&& name, double layer) const override;
    void update() override;
    void submit(VkCommandBuffer cmd);
    void render_forward(VkCommandBuffer cmd);
    void render_shadows(VkCommandBuffer cmd);
    void upload_uniforms();
};
}
#endif