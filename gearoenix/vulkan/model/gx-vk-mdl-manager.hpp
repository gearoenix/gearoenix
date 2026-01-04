#pragma once
#include "../../render/gx-rnd-build-configuration.hpp"
#if GX_RENDER_VULKAN_ENABLED
#include "../../render/model/gx-rnd-mdl-manager.hpp"

namespace gearoenix::vulkan::model {
struct Manager final : render::model::Manager {

    Manager();
    ~Manager() override;
};
}
#endif