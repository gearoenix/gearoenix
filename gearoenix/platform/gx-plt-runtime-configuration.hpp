#ifndef GEAROENIX_PLATFORM_RUNTIME_CONFIGURATION_HPP
#define GEAROENIX_PLATFORM_RUNTIME_CONFIGURATION_HPP
#include "../core/ecs/gx-cr-ecs-component.hpp"

namespace gearoenix::core::ecs {
struct World;
}

namespace gearoenix::render::engine {
struct Engine;
}

namespace gearoenix::platform {
struct Application;
struct BaseApplication;
struct RuntimeConfiguration final : core::ecs::Component {
    GX_GETSET_CREF_PRV(std::string, application_name);
    GX_GETSET_VAL_PRV(bool, fullscreen, false);
    GX_GETSET_VAL_PRV(bool, landscape, true);
    GX_GETSET_VAL_PRV(std::uint16_t, window_width, 1280);
    GX_GETSET_VAL_PRV(std::uint16_t, window_height, 720);
    GX_GETSET_VAL_PRV(std::uint16_t, window_resizing_event_interval_ms, 500);
    GX_GETSET_VAL_PRV(bool, show_cursor, true);

    // Following settings are in platform because actually before engine creation we don't have any render settings
    GX_GETSET_VAL_PRV(bool, vulkan_render_backend_enabled, true);
    GX_GETSET_VAL_PRV(bool, direct3dx_render_backend_enabled, true);
    GX_GETSET_VAL_PRV(bool, metal_render_backend_enabled, true);
    GX_GETSET_VAL_PRV(bool, opengl_render_backend_enabled, true);

    [[nodiscard]] const HierarchyTypes& get_hierarchy_types() const override;

    explicit RuntimeConfiguration(core::ecs::entity_id_t);

public:
    RuntimeConfiguration(const RuntimeConfiguration&) = delete;
    RuntimeConfiguration(RuntimeConfiguration&&) = delete;
    [[nodiscard]] static RuntimeConfiguration& get(core::ecs::World* w);
    [[nodiscard]] static RuntimeConfiguration& get(render::engine::Engine* e);
    [[nodiscard]] static RuntimeConfiguration& get(Application* a);
    [[nodiscard]] static RuntimeConfiguration& get(Application& a);
    [[nodiscard]] static RuntimeConfiguration& get(BaseApplication* b);
    ~RuntimeConfiguration() override;
    void show_debug_gui(const core::ecs::World& w) override;
};
}
#endif