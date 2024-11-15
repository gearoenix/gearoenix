#ifndef GEAROENIX_EDITOR_VIEWPORT_CAMERA_HPP
#define GEAROENIX_EDITOR_VIEWPORT_CAMERA_HPP
#include <gearoenix/core/macro/gx-cr-mcr-getter-setter.hpp>
#include <memory>

namespace gearoenix::physics {
struct Transformation;
}

namespace gearoenix::render::camera {
struct Camera;
}

namespace gearoenix::render::engine {
struct Engine;
}

namespace gearoenix::editor {
struct Application;
}

namespace gearoenix::editor::viewport {
struct Camera final {
    Application& app;
    render::engine::Engine& e;

    GX_GET_CREF_PRT(std::shared_ptr<render::camera::Camera>, camera);
    GX_GET_CREF_PRT(std::shared_ptr<physics::Transformation>, transformation);

public:
    explicit Camera(Application& app);
    ~Camera();
    void update();
};
}

#endif