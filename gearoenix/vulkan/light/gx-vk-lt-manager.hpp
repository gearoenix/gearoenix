#pragma once
#include "../../render/gx-rnd-build-configuration.hpp"
#if GX_RENDER_VULKAN_ENABLED
#include "../../render/light/gx-rnd-lt-manager.hpp"

namespace gearoenix::vulkan::buffer {
struct Uniform;
}

namespace gearoenix::vulkan::light {

struct Manager final : render::light::Manager, core::Singleton<Manager> {
    Manager();
    ~Manager() override;
    void update() override;
    [[nodiscard]] core::ecs::EntityPtr build_directional(std::string&&, core::ecs::Entity*) override;
    void build_shadow_caster_directional(
        std::string&& name,
        core::ecs::Entity* parent,
        std::uint32_t shadow_map_resolution,
        float camera_far,
        float camera_near,
        float camera_aspect,
        core::job::EndCaller<core::ecs::EntityPtr>&& entity_callback) override;
};
}
#endif