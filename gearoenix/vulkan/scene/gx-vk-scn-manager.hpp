#pragma once
#include "../../render/gx-rnd-build-configuration.hpp"
#if GX_RENDER_VULKAN_ENABLED
#include "../../render/scene/gx-rnd-scn-manager.hpp"
#include "../descriptor/gx-vk-des-uniform-indexer.hpp"
#include "../gx-vk-loader.hpp"

#include <vector>

struct GxShaderDataScene;

namespace gearoenix::vulkan {
struct DrawState;
}

namespace gearoenix::vulkan::buffer {
struct Uniform;
}

namespace gearoenix::vulkan::command {
struct Buffer;
}

namespace gearoenix::vulkan::scene {
struct Manager final : render::scene::Manager, core::Singleton<Manager> {
    descriptor::UniformIndexer<GxShaderDataScene> uniform_indexer;

public:
    [[nodiscard]] const descriptor::UniformIndexer<GxShaderDataScene>& get_uniform_indexer() const { return uniform_indexer; }

    Manager();
    ~Manager() override;
    [[nodiscard]] core::ecs::EntityPtr build(std::string&& name, core::fp_t layer) const override;
    void update() override;
    static void submit(vk::CommandBuffer cmd);
    static void render_forward(DrawState& draw_state);
    static void render_shadows(DrawState& draw_state);
    void upload_uniforms();
};
}
#endif