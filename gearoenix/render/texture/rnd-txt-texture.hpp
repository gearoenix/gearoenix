#ifndef GEAROENIX_RENDER_TEXTURE_TEXTURE_HPP
#define GEAROENIX_RENDER_TEXTURE_TEXTURE_HPP
#include "../../core/asset/cr-asset.hpp"
#include "../../core/sync/cr-sync-end-caller.hpp"
#include "rnd-txt-format.hpp"
#include "rnd-txt-sample.hpp"
#include "rnd-txt-type.hpp"

namespace gearoenix::render::engine {
class Engine;
}

namespace gearoenix::system::stream {
class Stream;
}

namespace gearoenix::render::texture {
class Texture : public core::asset::Asset {
    GX_GET_CVAL_PRT(Type, texture_type)
    GX_GET_VAL_PRT(TextureFormat, texture_format, TextureFormat::RgbaUint8)
    GX_GET_CPTR_PRT(engine::Engine, render_engine)
    GX_GET_CREF_PRT(SampleInfo, sample_info)
protected:
    Texture(
        core::Id my_id,
        std::string name,
        Type texture_type,
        TextureFormat texture_format,
        const SampleInfo& sample_info,
        engine::Engine* e) noexcept;

    static void write_gx3d_image(
        system::stream::Stream* file,
        const float* data,
        std::size_t img_width,
        std::size_t img_height,
        std::size_t components_count) noexcept;

    static void write_gx3d_image(
        system::stream::Stream* file,
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
        const std::shared_ptr<system::stream::Stream>& s,
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
};
}

#endif
