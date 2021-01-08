#ifndef GEAROENIX_RENDER_TEXTURE_TEXTURE_HPP
#define GEAROENIX_RENDER_TEXTURE_TEXTURE_HPP
#include "../../core/asset/gx-cr-asset.hpp"
#include "../../core/sync/gx-cr-sync-end-caller.hpp"
#include "gx-rnd-txt-format.hpp"
#include "gx-rnd-txt-sampler.hpp"
#include "gx-rnd-txt-type.hpp"

namespace gearoenix::render::engine {
struct Engine;
}

namespace gearoenix::platform::stream {
struct Stream;
}

namespace gearoenix::render::texture {
struct Texture : public core::asset::Asset {
    GX_GET_CVAL_PRT(Type, texture_type)
    GX_GET_VAL_PRT(TextureFormat, texture_format, TextureFormat::RgbaUint8)
    GX_GET_CPTR_PRT(engine::Engine, render_engine)
    GX_GET_CREF_PRT(SamplerInfo, sample_info)
protected:
    Texture(
        core::Id my_id,
        std::string name,
        Type texture_type,
        TextureFormat texture_format,
        const SamplerInfo& sample_info,
        engine::Engine* e) noexcept;

    static void write_gx3d_image(
        platform::stream::Stream* file,
        const float* data,
        std::size_t img_width,
        std::size_t img_height,
        std::size_t components_count) noexcept;

    static void write_gx3d_image(
        platform::stream::Stream* file,
        const unsigned char* data,
        std::size_t img_width,
        std::size_t img_height,
        std::size_t components_count) noexcept;

public:
    ~Texture() noexcept override;
    void write_gx3d(
        const std::string& file_address,
        const core::sync::EndCaller<core::sync::EndCallerIgnore>& c) noexcept;
    virtual void write_gx3d(
        const std::shared_ptr<platform::stream::Stream>& s,
        const core::sync::EndCaller<core::sync::EndCallerIgnore>& c) noexcept;

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
};
}

#endif
