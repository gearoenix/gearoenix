#ifndef GEAROENIX_RENDER_TEXTURE_TEXTURE_HPP
#define GEAROENIX_RENDER_TEXTURE_TEXTURE_HPP
#include "../../core/macro/gx-cr-mcr-getter-setter.hpp"
#include "../../core/sync/gx-cr-sync-end-caller.hpp"
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

protected:
    Texture(
        std::string name,
        const TextureInfo& info,
        engine::Engine& e) noexcept;

    void write_image(
        platform::stream::Stream& s,
        const std::uint8_t* data,
        std::size_t img_width,
        std::size_t img_height,
        TextureFormat format) const noexcept;

    void write_gx3d_image(
        platform::stream::Stream& s,
        const std::uint8_t* data,
        std::size_t img_width,
        std::size_t img_height,
        TextureFormat format) const noexcept;

public:
    virtual ~Texture() noexcept;
    virtual void write(const std::shared_ptr<platform::stream::Stream>& s, const core::sync::EndCaller& c) const noexcept;
    [[nodiscard]] std::size_t get_mipmaps_count() const noexcept;

    [[nodiscard]] static std::vector<std::uint8_t> convert_pixels(
        const float* data,
        std::size_t in_components_count,
        std::size_t pixels_count,
        std::size_t out_components_count) noexcept;

    [[nodiscard]] static std::vector<std::vector<std::uint8_t>> convert_float_pixels(
        const std::vector<std::vector<std::uint8_t>>& data,
        std::size_t in_components_count,
        std::size_t out_components_count) noexcept;

    [[nodiscard]] static std::vector<std::vector<std::vector<std::uint8_t>>> convert_float_pixels(
        const std::vector<std::vector<std::vector<std::uint8_t>>>& data,
        std::size_t in_components_count,
        std::size_t out_components_count) noexcept;

    [[nodiscard]] static std::size_t compute_mipmaps_count(
        std::size_t img_width,
        std::size_t img_height) noexcept;

    [[nodiscard]] static std::size_t compute_mipmaps_count(
        const TextureInfo& info) noexcept;
};
}

#endif
