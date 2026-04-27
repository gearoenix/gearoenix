#pragma once
#include "../../core/job/gx-cr-job-end-caller.hpp"
#include "../../core/macro/gx-cr-mcr-getter-setter.hpp"
#include "gx-rnd-txt-texture-info.hpp"
#include <string>
#include <vector>

namespace gearoenix::platform::stream {
struct Stream;
}

namespace gearoenix::render::texture {
struct Texture {
    GX_GET_REFC_PRT(std::string, name);
    GX_GET_CREF_PRT(TextureInfo, info);
    std::uint32_t shader_resource_index = static_cast<std::uint32_t>(-1);
    std::uint32_t sampler_shader_resource_index = static_cast<std::uint32_t>(-1);

    Texture(std::string&& name, const TextureInfo& info);

private:
    static void write_image(platform::stream::Stream& s, const std::uint8_t* data, std::uint32_t img_width, std::uint32_t img_height, TextureFormat format);

public:
    virtual ~Texture();
    virtual void write(const std::shared_ptr<platform::stream::Stream>& s, const core::job::EndCaller<>& c, bool include_content) const;
    void write(const std::shared_ptr<platform::stream::Stream>& s, const core::job::EndCaller<>& c) const;
    [[nodiscard]] std::uint64_t get_mipmaps_count() const;

    [[nodiscard]] static std::vector<std::uint8_t> convert_pixels(const float* data, std::uint64_t in_components_count, std::uint64_t pixels_count, std::uint64_t out_components_count);
    [[nodiscard]] static std::vector<std::vector<std::uint8_t>> convert_float_pixels(const std::vector<std::vector<std::uint8_t>>& data, std::uint64_t in_components_count, std::uint64_t out_components_count);
    [[nodiscard]] static std::vector<std::vector<std::vector<std::uint8_t>>> convert_float_pixels(const std::vector<std::vector<std::vector<std::uint8_t>>>& data, std::uint64_t in_components_count, std::uint64_t out_components_count);
    [[nodiscard]] static std::uint64_t compute_mipmaps_count(std::uint64_t img_width, std::uint64_t img_height);
    [[nodiscard]] static std::uint64_t compute_mipmaps_count(const TextureInfo& info);
    static void write_gx3d_image(std::shared_ptr<platform::stream::Stream>&& s, std::vector<std::uint8_t>&& data, std::uint32_t img_width, std::uint32_t img_height, TextureFormat format, core::job::EndCaller<>&& end);

    [[nodiscard]] std::uint32_t get_shader_resource_index() const
    {
        GX_ASSERT_D(shader_resource_index != static_cast<std::uint32_t>(-1));
        return shader_resource_index;
    }

    [[nodiscard]] std::uint32_t get_sampler_shader_resource_index() const
    {
        GX_ASSERT_D(sampler_shader_resource_index != static_cast<std::uint32_t>(-1));
        return sampler_shader_resource_index;
    }
};
}