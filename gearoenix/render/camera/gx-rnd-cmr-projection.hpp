#pragma once
#include <variant>

namespace gearoenix::render::camera {
struct PerspectiveProjectionData {
    float field_of_view_y = 0.8f;
};

struct OrthographicProjectionData {
    float scale = 1.0;
};

typedef std::variant<PerspectiveProjectionData, OrthographicProjectionData> ProjectionData;
}