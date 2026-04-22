#pragma once
#include "../../render/gx-rnd-build-configuration.hpp"
#if GX_RENDER_VULKAN_ENABLED
#include "../../render/scene/gx-rnd-scn-manager.hpp"
#include "../gx-vk-loader.hpp"

namespace gearoenix::vulkan {
struct DrawState;
}

namespace gearoenix::vulkan::scene {
struct Manager final : render::scene::Manager, core::Singleton<Manager> {
    Manager();
    ~Manager() override;
    void build(std::string&& name, core::fp_t layer, core::job::EndCaller<core::ecs::EntityPtr>&& end) const override;
    void update() override;
    static void submit(vk::CommandBuffer cmd);
    static void render_forward(DrawState& draw_state);
    static void render_shadows(DrawState& draw_state);
};
}
#endif