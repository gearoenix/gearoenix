#pragma once
#include "../../render/gx-rnd-build-configuration.hpp"
#if GX_RENDER_VULKAN_ENABLED
#include "../../render/model/gx-rnd-mdl-manager.hpp"

namespace gearoenix::vulkan::buffer {
struct Uniform;
}

namespace gearoenix::vulkan::model {
struct Manager final : render::model::Manager, core::Singleton<Manager> {

    Manager();
    ~Manager() override;
    [[nodiscard]] static const buffer::Uniform& get_uniform_buffer();
};
}
#endif