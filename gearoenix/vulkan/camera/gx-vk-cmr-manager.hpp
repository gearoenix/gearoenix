#pragma once
#include "../../render/gx-rnd-build-configuration.hpp"
#if GX_RENDER_VULKAN_ENABLED
#include "../../render/camera/gx-rnd-cmr-manager.hpp"

namespace gearoenix::vulkan::buffer {
struct Uniform;
}

namespace gearoenix::vulkan::camera {
struct Manager final : render::camera::Manager, core::Singleton<Manager> {
    Manager();
    ~Manager() override;
    void build(
        std::string&& name,
        core::ecs::Entity* parent,
        core::job::EndCaller<core::ecs::EntityPtr>&& entity_callback) override;
    void window_resized() override;
    [[nodiscard]] static const buffer::Uniform& get_uniform_buffer();
};
}

#endif