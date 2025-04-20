#ifndef GEAROENIX_VULKAN_CAMERA_MANAGER_HPP
#define GEAROENIX_VULKAN_CAMERA_MANAGER_HPP
#include "../../render/gx-rnd-build-configuration.hpp"
#ifdef GX_RENDER_VULKAN_ENABLED
#include "../../render/camera/gx-rnd-cmr-manager.hpp"

namespace gearoenix::vulkan::engine {
struct Engine;
}

namespace gearoenix::vulkan::camera {
struct Manager final : public render::camera::Manager {
private:
    engine::Engine& vk_e;

    std::shared_ptr<render::camera::Builder> build(const std::string& name) override;
    void update() override;

public:
    explicit Manager(engine::Engine& e);
    ~Manager() override;
};
}

#endif
#endif