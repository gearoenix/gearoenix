#ifndef GEAROENIX_RENDER_CAMERA_BLOOM_DATA_HPP
#define GEAROENIX_RENDER_CAMERA_BLOOM_DATA_HPP
#include "../../core/macro/gx-cr-mcr-getter-setter.hpp"
#include "../../math/gx-math-vector-4d.hpp"

namespace gearoenix::render::camera {
struct Camera;
struct BloomData final {
    // todo engine must handle this in the frame-graph
    // typedef std::array<std::shared_ptr<texture::Target>, GX_RENDER_MAX_BLOOM_DOWN_SAMPLE_COUNT> Targets;
    // typedef std::array<std::shared_ptr<texture::Target>, GX_RENDER_MAX_BLOOM_DOWN_SAMPLE_COUNT + 1> UpTargets;

    GX_GET_REF_PRV(math::Vec4<float>, scatter_clamp_max_threshold_threshold_knee);
    // GX_GET_CREF_PRV(std::shared_ptr<texture::Target>, prefilter_target);
    // GX_GET_CREF_PRV(Targets, horizontal_targets);
    // GX_GET_CREF_PRV(Targets, vertical_targets);
    // GX_GET_CREF_PRV(UpTargets, upsampler_targets);

public:
    BloomData();
    void show_debug_data();
};
}

#endif