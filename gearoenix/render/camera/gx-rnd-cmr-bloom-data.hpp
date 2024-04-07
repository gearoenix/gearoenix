#ifndef GEAROENIX_RENDER_CAMERA_BLOOM_DATA_HPP
#define GEAROENIX_RENDER_CAMERA_BLOOM_DATA_HPP
#include "../../core/job/gx-cr-job-end-caller.hpp"
#include "../../core/macro/gx-cr-mcr-getter-setter.hpp"
#include "../../math/gx-math-vector-4d.hpp"
#include "../gx-rnd-build-configuration.hpp"
#include <array>
#include <memory>
#include <string>

namespace gearoenix::render::texture {
struct Target;
}

namespace gearoenix::render::engine {
struct Engine;
}

namespace gearoenix::render::camera {
struct Camera;
struct Target;
struct BloomData final {
    typedef std::array<std::shared_ptr<texture::Target>, GX_RENDER_MAX_BLOOM_DOWN_SAMPLE_COUNT> Targets;
    typedef std::array<std::shared_ptr<texture::Target>, GX_RENDER_MAX_BLOOM_DOWN_SAMPLE_COUNT + 1> UpTargets;

    GX_GET_REF_PRV(math::Vec4<float>, scatter_clamp_max_threshold_threshold_knee);
    GX_GET_CREF_PRV(std::shared_ptr<texture::Target>, prefilter_target);
    GX_GET_CREF_PRV(Targets, horizontal_targets);
    GX_GET_CREF_PRV(Targets, vertical_targets);
    GX_GET_CREF_PRV(UpTargets, upsampler_targets);

    BloomData();

public:
    static void construct(engine::Engine& e, const std::string& name, const Target& target, const core::job::EndCaller<BloomData>& c);
    BloomData(BloomData&&) noexcept;
    BloomData(const BloomData&) noexcept;
    BloomData& operator=(BloomData&&) noexcept;
    BloomData& operator=(const BloomData&) noexcept;
    ~BloomData();
    void show_debug_data();
};
}

#endif