#pragma once
#include "../../core/job/gx-cr-job-end-caller.hpp"
#include "../../core/macro/gx-cr-mcr-getter-setter.hpp"
#include "gx-rnd-txt-texture-info.hpp"
#include <string>
#include <vector>

namespace gearoenix::platform::stream {
struct Stream;
}

namespace gearoenix::render::engine {
struct Engine;
}

namespace gearoenix::render::texture {
struct Texture {
    GX_GET_RRF_PRT(engine::Engine, e);
    GX_GET_REFC_PRT(std::string, name);
    GX_GET_CREF_PRT(TextureInfo, info);

    Texture(
        std::string name,
        const TextureInfo& info,
        engine::Engine& e);

    static void write_image(
        platform::stream::Stream& s,
        const std::uint8_t* data,
        std::uint32_t img_width,
        std::uint32_t img_height,
        TextureFormat format);

    static void write_gx3d_image(
        platform::stream::Stream& s,
        const std::uint8_t* data,
        std::uint32_t img_width,
        std::uint32_t img_height,
        TextureFormat format);

public:
    virtual ~Texture();
    virtual void write(const std::shared_ptr<platform::stream::Stream>& s, const core::job::EndCaller<>& c) const;
    [[nodiscard]] std::uint64_t get_mipmaps_count() const;

    [[nodiscard]] static std::vector<std::uint8_t> convert_pixels(
        const float* data,
        std::uint64_t in_components_count,
        std::uint64_t pixels_count,
        std::uint64_t out_components_count);

    [[nodiscard]] static std::vector<std::vector<std::uint8_t>> convert_float_pixels(
        const std::vector<std::vector<std::uint8_t>>& data,
        std::uint64_t in_components_count,
        std::uint64_t out_components_count);

    [[nodiscard]] static std::vector<std::vector<std::vector<std::uint8_t>>> convert_float_pixels(
        const std::vector<std::vector<std::vector<std::uint8_t>>>& data,
        std::uint64_t in_components_count,
        std::uint64_t out_components_count);

    [[nodiscard]] static std::uint64_t compute_mipmaps_count(
        std::uint64_t img_width,
        std::uint64_t img_height);

    [[nodiscard]] static std::uint64_t compute_mipmaps_count(
        const TextureInfo& info);
};
}