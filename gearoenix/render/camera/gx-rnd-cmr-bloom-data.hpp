#ifndef GEAROENIX_RENDER_CAMERA_BLOOM_DATA_HPP
#define GEAROENIX_RENDER_CAMERA_BLOOM_DATA_HPP
#include "../../core/macro/gx-cr-mcr-getter-setter.hpp"
#include "../../math/gx-math-vector-4d.hpp"

namespace gearoenix::render::camera {
struct BloomData final {
    GX_GET_REF_PRV(math::Vec4<float>, scatter_clamp_max_threshold_threshold_knee);

public:
    BloomData();
    void show_debug_data();
};
}

#endif