#include "gx-rnd-txt-manager.hpp"
#include "../../core/gx-cr-string.hpp"
#include "../../core/sync/gx-cr-sync-parallel-for.hpp"
#include "../../platform/gx-plt-application.hpp"
#include "../../platform/stream/gx-plt-stm-asset.hpp"
#include "../../platform/stream/gx-plt-stm-local.hpp"
#include "../../platform/stream/gx-plt-stm-path.hpp"
#include "../engine/gx-rnd-eng-engine.hpp"
#include "gx-rnd-txt-image.hpp"
#include "gx-rnd-txt-texture-2d.hpp"
#include "gx-rnd-txt-texture-cube.hpp"
#include <array>

gearoenix::render::texture::Manager::Manager(engine::Engine& e) noexcept
    : e(e)
{
}

gearoenix::render::texture::Manager::~Manager() noexcept = default;

std::shared_ptr<gearoenix::render::texture::Texture> gearoenix::render::texture::Manager::read_gx3d(
    const platform::stream::Path& path,
    const core::sync::EndCallerIgnored& c) noexcept
{
    const auto stream = platform::stream::Stream::open(path, e.get_platform_application());
    return read_gx3d(stream, c);
}

std::shared_ptr<gearoenix::render::texture::Texture> gearoenix::render::texture::Manager::read_gx3d(
    const std::shared_ptr<platform::stream::Stream>& stream,
    const core::sync::EndCallerIgnored& c) noexcept
{
    std::string name;
    stream->read(name);
    TextureInfo txt_info;
    txt_info.read(*stream);
    const auto is_float = format_has_float_component(txt_info.format);
    const auto comps_count = format_components_count(txt_info.format);
    const auto mips_count = Texture::compute_mipmaps_count(txt_info);
    const auto comp_bits = format_component_bits_count(txt_info.format);
    if (is_float) {
        GX_ASSERT(32 == comp_bits);
        if (!e.get_specification().is_float_texture_supported) {
            txt_info.format = TextureFormat::RgbaUint8; // No other format is supported
        }
    } else {
        GX_ASSERT(8 == comp_bits);
    }
    const auto fill_mips = [&](std::vector<std::vector<std::uint8_t>>& pixels) noexcept {
        pixels.resize(mips_count);
        auto mip_w = txt_info.width;
        auto mip_h = txt_info.height;
        for (std::size_t mip_index = 0; mip_index < mips_count; ++mip_index, mip_w >>= 1, mip_h >>= 1) {
            auto& mip_pixels = pixels[mip_index];
            const auto data = stream->read<std::vector<std::uint8_t>>();
            if (is_float) {
                std::vector<float> float_pixels;
                std::size_t decode_w, decode_h, ignored;
                Image::decode(data.data(), data.size(), comps_count, float_pixels, decode_w, decode_h, ignored);
                GX_ASSERT(mip_w == decode_w);
                GX_ASSERT(mip_h == decode_h);
                if (e.get_specification().is_float_texture_supported) {
                    mip_pixels.resize(float_pixels.size() << 2);
                    std::memcpy(mip_pixels.data(), float_pixels.data(), mip_pixels.size());
                } else {
                    GX_LOG_D("Loading float texture in a an unsupported engine, inefficient load of texture.");
                    mip_pixels.resize((float_pixels.size() * 4) / comps_count); // 4 is because we use RgbaUint8
                    for (std::size_t pixel_index = 0, float_index = 0, comp_index = 0; pixel_index < mip_pixels.size();) {
                        for (comp_index = 0; comp_index < comps_count; ++comp_index, ++pixel_index, ++float_index) {
                            const float f = float_pixels[float_index];
                            mip_pixels[pixel_index] = f >= 1.0f ? 225 : f <= 0.0f ? 0
                                                                                  : static_cast<std::uint8_t>(f * 255.0f + 0.5f);
                        }
                        for (; comp_index < 3; ++comp_index, ++pixel_index)
                            mip_pixels[pixel_index] = 0;
                        if (comp_index < 4) {
                            mip_pixels[pixel_index] = 0;
                            ++pixel_index;
                        }
                    }
                }
            } else {
                std::size_t decode_w, decode_h, ignored;
                Image::decode(data.data(), data.size(), comps_count, mip_pixels, decode_w, decode_h, ignored);
                GX_ASSERT(mip_w == decode_w);
                GX_ASSERT(mip_h == decode_h);
            }
        }
    };
    if (Type::TextureCube == txt_info.type) {
        GX_ASSERT(txt_info.width == txt_info.height);
        std::vector<std::vector<std::vector<std::uint8_t>>> pixels(6);
        for (auto& face_pixels : pixels)
            fill_mips(face_pixels);
        return create_cube_from_pixels(std::move(name), std::move(pixels), txt_info, c);
    } else if (Type::Texture2D == txt_info.type) {
        std::vector<std::vector<std::uint8_t>> pixels;
        fill_mips(pixels);
        return create_2d_from_pixels(std::move(name), std::move(pixels), txt_info, c);
    } else {
        GX_UNIMPLEMENTED;
    }
}

std::shared_ptr<gearoenix::render::texture::Texture2D> gearoenix::render::texture::Manager::create_2d_from_colour(
    const math::Vec4<float>& colour, const core::sync::EndCallerIgnored& c) noexcept
{
    std::vector<std::uint8_t> pixels0(4);
    pixels0[0] = static_cast<std::uint8_t>(colour.x >= 1.0 ? 255.1 : colour.x <= 0.0 ? 0.1
                                                                                     : colour.x * 255.0f + 0.51f);
    pixels0[1] = static_cast<std::uint8_t>(colour.y >= 1.0 ? 255.1 : colour.y <= 0.0 ? 0.1
                                                                                     : colour.y * 255.0f + 0.51f);
    pixels0[2] = static_cast<std::uint8_t>(colour.z >= 1.0 ? 255.1 : colour.z <= 0.0 ? 0.1
                                                                                     : colour.z * 255.0f + 0.51f);
    pixels0[3] = static_cast<std::uint8_t>(colour.w >= 1.0 ? 255.1 : colour.w <= 0.0 ? 0.1
                                                                                     : colour.w * 255.0f + 0.51f);
    auto name = "colour{" + std::to_string(pixels0[0]) + "," + std::to_string(pixels0[1]) + "," + std::to_string(pixels0[2]) + "," + std::to_string(pixels0[3]) + "}";
    {
        GX_GUARD_LOCK(textures_2d);
        if (const auto search = textures_2d.find(name); textures_2d.end() != search)
            if (auto r = search->second.lock(); nullptr != r)
                return r;
    }
    std::vector<std::vector<std::uint8_t>> pixels { std::move(pixels0) };
    return create_2d_from_pixels(
        std::move(name),
        std::move(pixels),
        TextureInfo {
            .format = TextureFormat::RgbaUint8,
            .sampler_info = SamplerInfo {
                .min_filter = Filter::Nearest,
                .mag_filter = Filter::Nearest,
                .wrap_s = Wrap::Repeat,
                .wrap_t = Wrap::Repeat,
                .wrap_r = Wrap::Repeat,
            },
            .width = 1,
            .height = 1,
            .type = Type::Texture2D,
            .has_mipmap = false,
        },
        c);
}

std::shared_ptr<gearoenix::render::texture::Texture2D> gearoenix::render::texture::Manager::get_brdflut(
    const core::sync::EndCallerIgnored& c) noexcept
{
    GX_GUARD_LOCK(brdflut);
    if (nullptr != brdflut)
        return brdflut;
    constexpr auto file_name = "default-brdflut.gx-2d-texture";
    std::shared_ptr<platform::stream::Stream> stream(platform::stream::Asset::construct(e.get_platform_application(), file_name));
    if (nullptr == stream)
        stream = std::shared_ptr<platform::stream::Stream>(platform::stream::Local::open(e.get_platform_application(), file_name));
    if (nullptr != stream) {
        brdflut = std::dynamic_pointer_cast<Texture2D>(read_gx3d(stream, c));
        GX_ASSERT(nullptr != brdflut);
        return brdflut;
    }
    constexpr std::size_t resolution = 256;
    const TextureInfo texture_info {
        .format = TextureFormat::RgbaUint8,
        .sampler_info = SamplerInfo {
            .min_filter = Filter::Linear,
            .mag_filter = Filter::Linear,
            .wrap_s = Wrap::ClampToEdge,
            .wrap_t = Wrap::ClampToEdge,
            .wrap_r = Wrap::ClampToEdge,
        },
        .width = resolution,
        .height = resolution,
        .type = Type::Texture2D,
        .has_mipmap = false
    };
    const auto pixels_vectors = create_brdflut_pixels();
    std::vector<std::uint8_t> pixels0(pixels_vectors.size() * sizeof(math::Vec4<std::uint8_t>));
    std::memcpy(pixels0.data(), pixels_vectors.data(), pixels0.size());
    std::vector<std::vector<std::uint8_t>> pixels { std::move(pixels0) };
    brdflut = create_2d_from_pixels("default-brdflut", std::move(pixels), texture_info, c);
    brdflut->write(std::shared_ptr<platform::stream::Stream>(new platform::stream::Local(e.get_platform_application(), file_name, true)), c);
    return brdflut;
}

std::shared_ptr<gearoenix::render::texture::Texture2D> gearoenix::render::texture::Manager::get_checker(
    const core::sync::EndCallerIgnored& c) noexcept
{
    GX_GUARD_LOCK(checkers);
    if (nullptr != checkers)
        return checkers;
    const TextureInfo texture_info {
        .format = TextureFormat::RgbaUint8,
        .sampler_info = SamplerInfo {
            .min_filter = Filter::Nearest,
            .mag_filter = Filter::Nearest,
            .wrap_s = Wrap::Repeat,
            .wrap_t = Wrap::Repeat,
            .wrap_r = Wrap::Repeat,
        },
        .width = 2,
        .height = 2,
        .type = Type::Texture2D,
        .has_mipmap = false
    };
    std::vector<std::vector<std::uint8_t>> pixels { {
        0u, 0u, 0u, 255u, // Pixel 0
        255u, 255u, 255u, 255u, // Pixel 1
        255u, 255u, 255u, 255u, // Pixel 2
        0u, 0u, 0u, 255u, // Pixel 3
    } };
    checkers = create_2d_from_pixels("default-checker", std::move(pixels), texture_info, c);
    return checkers;
}

std::shared_ptr<gearoenix::render::texture::Texture2D> gearoenix::render::texture::Manager::create_2d_from_pixels(
    std::string name, std::vector<std::vector<std::uint8_t>> pixels, const TextureInfo& info, const core::sync::EndCallerIgnored& c) noexcept
{
    {
        GX_GUARD_LOCK(textures_2d);
        if (const auto search = textures_2d.find(name); textures_2d.end() != search)
            if (auto r = search->second.lock(); nullptr != r)
                return r;
    }
    return create_2d_from_pixels_v(std::move(name), std::move(pixels), info, c);
}

std::shared_ptr<gearoenix::render::texture::Texture2D> gearoenix::render::texture::Manager::create_2d_from_formatted(
    std::string name,
    const void* const data,
    const std::size_t size,
    const TextureInfo& info,
    const core::sync::EndCallerIgnored& c) noexcept
{
    {
        GX_GUARD_LOCK(textures_2d);
        if (auto search = textures_2d.find(name); textures_2d.end() != search)
            if (auto r = search->second.lock(); nullptr != r)
                return r;
    }
    if (Type::Unknown != info.type)
        GX_UNIMPLEMENTED; // type converting is not implemented and is not going to be implemented in near future.
    if (TextureFormat::Unknown != info.format)
        GX_UNIMPLEMENTED; // formate converting does not have a high priority
    if (0 != info.width)
        GX_UNIMPLEMENTED; // dimention changing does not have a high priority
    if (0 != info.height)
        GX_UNIMPLEMENTED; // dimention changing does not have a high priority
    std::size_t img_width;
    std::size_t img_height;
    std::size_t img_channels;
    std::vector<std::uint8_t> pixels0;
    Image::decode(
        reinterpret_cast<const unsigned char*>(data), size, 4,
        pixels0, img_width, img_height, img_channels);
    GX_LOG_D("Texture 2D Image imported with file size: " << size << ", width: " << img_width << " height: " << img_height << ", channels: " << img_channels);
    TextureInfo new_info = info;
    new_info.format = TextureFormat::RgbaUint8;
    new_info.type = Type::Texture2D;
    new_info.width = img_width;
    new_info.height = img_height;
    return create_2d_from_pixels(std::move(name), { pixels0 }, new_info, c);
}

std::shared_ptr<gearoenix::render::texture::Texture2D> gearoenix::render::texture::Manager::create_2df_from_formatted(
    std::string name,
    const void* const data,
    std::size_t size,
    const TextureInfo& info,
    const core::sync::EndCallerIgnored& c) noexcept
{
    {
        GX_GUARD_LOCK(textures_2d);
        if (auto search = textures_2d.find(name); textures_2d.end() != search)
            if (auto r = search->second.lock(); nullptr != r)
                return r;
    }
    if (Type::Unknown != info.type)
        GX_UNIMPLEMENTED; // type converting is not implemented and is not going to be implemented in near future.
    if (TextureFormat::Unknown != info.format)
        GX_UNIMPLEMENTED; // formate converting does not have a high priority
    if (0 != info.width)
        GX_UNIMPLEMENTED; // dimention changing does not have a high priority
    if (0 != info.height)
        GX_UNIMPLEMENTED; // dimention changing does not have a high priority
    std::size_t img_width;
    std::size_t img_height;
    std::size_t img_channels;
    std::vector<float> pixels0f;
    Image::decode(
        reinterpret_cast<const unsigned char*>(data), size, 4,
        pixels0f, img_width, img_height, img_channels);
    GX_LOG_D("Texture 2D Image imported with file size: " << size << ", width: " << img_width << " height: " << img_height << ", channels: " << img_channels);
    TextureInfo new_info = info;
    new_info.format = TextureFormat::RgbaFloat32;
    new_info.type = Type::Texture2D;
    new_info.width = img_width;
    new_info.height = img_height;
    const auto pixels0_sz = pixels0f.size() * sizeof(float);
    std::vector<std::uint8_t> pixels0(pixels0_sz);
    std::memcpy(pixels0.data(), pixels0f.data(), pixels0_sz);
    return create_2d_from_pixels(std::move(name), { pixels0 }, new_info, c);
}

std::shared_ptr<gearoenix::render::texture::Texture2D> gearoenix::render::texture::Manager::create_2d_from_file(
    std::string name,
    const platform::stream::Path& path,
    const TextureInfo& info,
    const core::sync::EndCallerIgnored& c) noexcept
{
    {
        GX_GUARD_LOCK(textures_2d);
        if (auto search = textures_2d.find(name); textures_2d.end() != search)
            if (auto r = search->second.lock(); nullptr != r)
                return r;
    }
    const auto stream = platform::stream::Stream::open(path, e.get_platform_application());
    auto data = stream->get_file_content();
    if (path.get_raw_data().ends_with(".hdr"))
        return create_2df_from_formatted(std::move(name), data.data(), data.size(), info, c);
    return create_2d_from_formatted(std::move(name), data.data(), data.size(), info, c);
}

std::shared_ptr<gearoenix::render::texture::TextureCube> gearoenix::render::texture::Manager::create_cube_from_colour(
    const math::Vec4<float>& colour, const core::sync::EndCallerIgnored& c) noexcept
{
    std::vector<std::uint8_t> pixels0(4);
    pixels0[0] = static_cast<std::uint8_t>(colour.x >= 1.0 ? 255.1 : colour.x <= 0.0 ? 0.1
                                                                                     : colour.x * 255.0f + 0.51f);
    pixels0[1] = static_cast<std::uint8_t>(colour.y >= 1.0 ? 255.1 : colour.y <= 0.0 ? 0.1
                                                                                     : colour.y * 255.0f + 0.51f);
    pixels0[2] = static_cast<std::uint8_t>(colour.z >= 1.0 ? 255.1 : colour.z <= 0.0 ? 0.1
                                                                                     : colour.z * 255.0f + 0.51f);
    pixels0[3] = static_cast<std::uint8_t>(colour.w >= 1.0 ? 255.1 : colour.w <= 0.0 ? 0.1
                                                                                     : colour.w * 255.0f + 0.51f);
    auto name = "colour{" + std::to_string(pixels0[0]) + "," + std::to_string(pixels0[1]) + "," + std::to_string(pixels0[2]) + "," + std::to_string(pixels0[3]) + "}";
    {
        GX_GUARD_LOCK(textures_cube);
        if (const auto search = textures_cube.find(name); textures_cube.end() != search)
            if (auto r = search->second.lock(); nullptr != r)
                return r;
    }
    std::vector<std::vector<std::vector<std::uint8_t>>> pixels { { pixels0 }, { pixels0 }, { pixels0 }, { pixels0 }, { pixels0 }, { pixels0 } };
    return create_cube_from_pixels(
        std::move(name),
        std::move(pixels),
        TextureInfo {
            .format = TextureFormat::RgbaUint8,
            .sampler_info = SamplerInfo {
                .min_filter = Filter::Nearest,
                .mag_filter = Filter::Nearest,
                .wrap_s = Wrap::Repeat,
                .wrap_t = Wrap::Repeat,
                .wrap_r = Wrap::Repeat,
            },
            .width = 1,
            .height = 1,
            .type = Type::TextureCube,
            .has_mipmap = false,
        },
        c);
}

std::shared_ptr<gearoenix::render::texture::TextureCube> gearoenix::render::texture::Manager::create_cube_from_pixels(
    std::string name, std::vector<std::vector<std::vector<std::uint8_t>>> pixels, const TextureInfo& info, const core::sync::EndCallerIgnored& c) noexcept
{
    {
        GX_GUARD_LOCK(textures_cube);
        if (const auto search = textures_cube.find(name); textures_cube.end() != search)
            if (auto r = search->second.lock(); nullptr != r)
                return r;
    }
    return create_cube_from_pixels_v(std::move(name), std::move(pixels), info, c);
}

std::shared_ptr<gearoenix::render::texture::Target> gearoenix::render::texture::Manager::create_target(
    std::string name, std::vector<Attachment>&& attachments, const core::sync::EndCallerIgnored& c) noexcept
{
    {
        GX_GUARD_LOCK(targets);
        if (const auto search = targets.find(name); targets.end() != search)
            if (auto r = search->second.lock(); nullptr != r)
                return r;
    }
    return create_target_v(std::move(name), std::move(attachments), c);
}

gearoenix::math::Vec2<float> gearoenix::render::texture::Manager::integrate_brdf(const float n_dot_v, const float roughness) noexcept
{
    const math::Vec3<float> v(std::sqrt(1.0f - n_dot_v * n_dot_v), 0.0f, n_dot_v);

    float a = 0.0f;
    float b = 0.0f;

    const auto n = math::Vec3<float>(0.0f, 0.0f, 1.0f);

    const unsigned int SAMPLE_COUNT = 1024u;
    for (unsigned int i = 0u; i < SAMPLE_COUNT; ++i) {
        // generates a sample vector that's biased towards the
        // preferred alignment direction (importance sampling).
        const auto xi = math::Vec2<float>::hammersley(i, SAMPLE_COUNT);
        const auto h = math::Vec3<float>::importance_sample_ggx(xi, n, roughness);
        const auto l = (h * (2.0f * v.dot(h)) - v).normalised();

        const auto n_dot_l = l.z > 0.0f ? l.z : 0.0f;
        const auto n_dot_h = h.z > 0.0f ? h.z : 0.0f;
        auto temp = v.dot(h);
        const auto v_dot_h = temp > 0.0f ? temp : 0.0f;

        if (n_dot_l > 0.0f) {
            const auto g = geometry_smith(n, v, l, roughness);
            const auto g_vis = (g * v_dot_h) / (n_dot_h * n_dot_v);
            auto tmp = 1.0f - v_dot_h;
            auto tmp_p2 = tmp * tmp;
            const auto fc = tmp * tmp_p2 * tmp_p2;
            tmp = fc * g_vis;
            a += g_vis - tmp;
            b += tmp;
        }
    }
    a /= float(SAMPLE_COUNT);
    b /= float(SAMPLE_COUNT);
    return math::Vec2<float>(a, b);
}

std::vector<gearoenix::math::Vec4<std::uint8_t>> gearoenix::render::texture::Manager::create_brdflut_pixels(const std::size_t resolution) noexcept
{
    std::vector<math::Vec4<std::uint8_t>> pixels(resolution * resolution);
    const auto inv_res = 1.0f / float(resolution);
    core::sync::ParallelFor::execi(pixels.begin(), pixels.end(), [&](auto& pixel, const std::size_t index, auto) {
        auto p = integrate_brdf(
            (static_cast<float>(index % resolution) + 0.5f) * inv_res,
            (static_cast<float>(index / resolution) + 0.5f) * inv_res);
        p *= 255.0f;
        p += 0.5f;
        pixel.x = p.x >= 255.0f ? 255 : p.x <= 0.0f ? 0
                                                    : static_cast<std::uint8_t>(p.x);
        pixel.y = p.y >= 255.0f ? 255 : p.y <= 0.0f ? 0
                                                    : static_cast<std::uint8_t>(p.y);
        pixel.z = 0;
        pixel.w = 255;
    });
    return pixels;
}
