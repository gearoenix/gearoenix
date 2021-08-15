#ifndef GEAROENIX_RENDER_CAMERA_PROJECTION_HPP
#define GEAROENIX_RENDER_CAMERA_PROJECTION_HPP
#include <cstdint>

namespace gearoenix::render::camera {
enum struct Projection : std::uint8_t {
    Perspective = 1,
    Orthographic = 2,
};
}

#endif