#pragma once
#include "../../render/gx-rnd-build-configuration.hpp"
#if GX_RENDER_VULKAN_ENABLED
#include "../../render/camera/gx-rnd-cmr-manager.hpp"

namespace gearoenix::vulkan::camera {
struct Manager final : render::camera::Manager {
private:
    std::shared_ptr<render::camera::Builder> build(const std::string& name);
    void update() override;

public:
    explicit Manager();
    ~Manager() override;
};
}

#endif