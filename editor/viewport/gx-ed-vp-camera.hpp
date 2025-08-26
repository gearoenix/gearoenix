#pragma once
#include <gearoenix/core/macro/gx-cr-mcr-getter-setter.hpp>

#include <memory>

namespace gearoenix::physics {
struct Transformation;
}

namespace gearoenix::render::camera {
struct Camera;
}

namespace gearoenix::editor::viewport {
struct Camera final {
    GX_GET_CREF_PRT(std::shared_ptr<render::camera::Camera>, camera);
    GX_GET_CREF_PRT(std::shared_ptr<physics::Transformation>, transformation);

public:
    Camera();
    ~Camera();
    void update();
};
}