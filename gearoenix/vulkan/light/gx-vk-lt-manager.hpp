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
    [[nodiscard]] static const buffer::Uniform& get_uniform_buffer();
};
}
#endif