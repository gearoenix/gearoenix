#ifndef GEAROENIX_EDITOR_VIEWPORT_CAMERA_HPP
#define GEAROENIX_EDITOR_VIEWPORT_CAMERA_HPP
#include <gearoenix/core/macro/gx-cr-mcr-getter-setter.hpp>
#include <memory>

namespace gearoenix::physics {
struct TransformationComponent;
}

namespace gearoenix::render::camera {
struct Camera;
}

namespace gearoenix::editor {
struct Application;
}

namespace gearoenix::editor::viewport {
struct Camera final {
    Application& app;

    GX_GET_CREF_PRT(std::shared_ptr<render::camera::Camera>, camera);
    GX_GET_CREF_PRT(std::shared_ptr<physics::TransformationComponent>, transformation);

public:
    explicit Camera(Application& app);
    ~Camera();
    void update();
};
}

#endif