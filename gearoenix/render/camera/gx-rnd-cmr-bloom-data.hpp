#ifndef GEAROENIX_RENDER_CAMERA_BLOOM_DATA_HPP
#define GEAROENIX_RENDER_CAMERA_BLOOM_DATA_HPP
#include "../../core/macro/gx-cr-mcr-getter-setter.hpp"
#include "../../math/gx-math-vector-4d.hpp"
#include "../gx-rnd-build-configuration.hpp"
#include <array>
#include <memory>
#include <string>

namespace gearoenix::core::sync {
struct EndCaller;
}

namespace gearoenix::render::engine {
struct Engine;
}

namespace gearoenix::render::texture {
struct Target;
struct Texture2D;
}

namespace gearoenix::render::camera {
struct BloomData final {
    typedef std::array<std::shared_ptr<texture::Target>, GX_RENDER_MAX_BLOOM_DOWN_SAMPLE_COUNT> Targets;
    typedef std::array<std::shared_ptr<texture::Target>, GX_RENDER_MAX_BLOOM_DOWN_SAMPLE_COUNT - 1> TargetsLess;

    GX_GET_CREF_PRV(std::shared_ptr<texture::Texture2D>, horizontal_texture);
    GX_GET_CREF_PRV(std::shared_ptr<texture::Texture2D>, vertical_texture);
    GX_GET_CREF_PRV(std::shared_ptr<texture::Target>, source_target);
    GX_GET_CREF_PRV(std::shared_ptr<texture::Target>, prefilter_target);
    GX_GET_CREF_PRV(Targets, horizontal_targets);
    GX_GET_CREF_PRV(TargetsLess, vertical_targets);
    GX_GET_CREF_PRV(Targets, upsampler_targets);
    GX_GET_REF_PRV(math::Vec4<float>, scatter_clamp_max_threshold_threshold_knee);

public:
    BloomData(
        engine::Engine& e,
        const std::string& name,
        const core::sync::EndCaller& end_caller,
        const std::shared_ptr<texture::Target>& target) noexcept;
    BloomData(BloomData&&) noexcept;
    ~BloomData() noexcept;
};
}

#endif