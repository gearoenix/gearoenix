#include "gx-rnd-txt-manager.hpp"
#include "../../core/asset/gx-cr-asset-manager.hpp"
#include "../../core/gx-cr-string.hpp"
#include "../../core/sync/gx-cr-sync-parallel-for.hpp"
#include "../../platform/gx-plt-application.hpp"
#include "../../platform/stream/gx-plt-stm-asset.hpp"
#include "../../platform/stream/gx-plt-stm-local.hpp"
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

std::shared_ptr<gearoenix::render::texture::Texture2D> gearoenix::render::texture::Manager::create_2d_from_colour(
    const math::Vec4<float>& colour, const core::sync::EndCallerIgnored& c) noexcept
{
    static_assert(sizeof(float) == 4, "Only float 32 bit are supported.");
    const auto search = colours_2d.find(colour);
    if (colours_2d.end() != search) {
        return search->second;
    }
    std::vector<std::vector<std::uint8_t>> pixels { std::vector<std::uint8_t>(sizeof(math::Vec4<float>)) };
    std::memcpy(pixels[0].data(), colour.data(), sizeof(math::Vec4<float>));
    auto result = create_2d_from_pixels(
        "colour{" + std::to_string(colour.x) + "," + std::to_string(colour.y) + "," + std::to_string(colour.z) + "," + std::to_string(colour.w) + "}",
        std::move(pixels),
        TextureInfo {
            .format = TextureFormat::RgbaFloat32,
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
    colours_2d[colour] = result;
    return result;
}

std::shared_ptr<gearoenix::render::texture::Texture2D> gearoenix::render::texture::Manager::get_brdflut(
    const core::sync::EndCallerIgnored& c) noexcept
{
    constexpr std::size_t resolution = 256;
    if (nullptr != brdflut)
        return brdflut;
    const TextureInfo texture_info {
        .format = TextureFormat::RgFloat32,
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
    std::vector<std::uint8_t> pixels0(resolution * resolution * 4);
    std::size_t data_index = 0;
    for (const auto& p : pixels_vectors) {
        {
            const auto b = static_cast<int>(p.x * 255.0f);
            if (b < 0) {
                pixels0[data_index] = 0;
            } else if (b > 255) {
                pixels0[data_index] = 255;
            } else {
                pixels0[data_index] = static_cast<std::uint8_t>(b);
            }
        }
        ++data_index;
        {
            const auto b = static_cast<int>(p.y * 255.0f);
            if (b < 0) {
                pixels0[data_index] = 0;
            } else if (b > 255) {
                pixels0[data_index] = 255;
            } else {
                pixels0[data_index] = static_cast<std::uint8_t>(b);
            }
        }
        pixels0[++data_index] = 0;
        pixels0[++data_index] = 255;
        ++data_index;
    }
    std::vector<std::vector<std::uint8_t>> pixels { std::move(pixels0) };
    brdflut = create_2d_from_pixels("default-brdflut", std::move(pixels), texture_info, c);
    return brdflut;
}

// std::shared_ptr<gearoenix::render::texture::Texture2D> gearoenix::render::texture::Manager::create_2d_from_formatted(
//     std::string name,
//     const void* const data,
//     const std::size_t size,
//     const TextureInfo& info,
//     const core::sync::EndCallerIgnored& c) noexcept
//{
//     std::size_t img_width;
//     std::size_t img_height;
//     std::size_t img_channels;
//     std::vector<std::uint8_t> pixels0;
//     Image::decode(data, size, std::nullopt, pixels, img_width, img_height, img_channels);
//     GXLOGD("Texture 2D Image imported with file size: " << size << ", width: " << img_width << " height: " << img_height << ", channels: " << img_channels)
//     TextureInfo info;
//     info.sampler_info = sampler_info;
//     switch (img_channels) {
//     case 1:
//         info.format = TextureFormat::Uint8;
//         break;
//     case 2:
//         info.format = TextureFormat::RgUint8;
//         break;
//     case 3:
//         info.format = TextureFormat::RgbUint8;
//         break;
//     case 4:
//         info.format = TextureFormat::RgbaUint8;
//         break;
//     default:
//         GX_UNEXPECTED
//     }
//     info.texture_type = Type::Texture2D;
//     return create_2d(std::move(name), { pixels }, info, img_width, img_height, c);
// }
//
// std::shared_ptr<gearoenix::render::texture::Texture2D> gearoenix::render::texture::Manager::create_2d_f(
//     std::string name,
//     const unsigned char* const data,
//     const std::size_t size, core::sync::EndCaller<Texture2D>& c,
//     const SamplerInfo& sampler_info) noexcept
//{
//     std::size_t img_width;
//     std::size_t img_height;
//     std::size_t img_channels;
//     std::vector<float> pixels;
//     Image::decode(data, size, std::nullopt, pixels, img_width, img_height, img_channels);
//     GXLOGD("Texture 2D Image imported with file size: " << size << ", width: " << img_width << " height: " << img_height << ", channels: " << img_channels)
//     TextureInfo info;
//     info.sampler_info = sampler_info;
//     info.has_mipmap = sampler_info.needs_mipmap();
//     switch (img_channels) {
//     case 1:
//         GX_UNIMPLEMENTED
//     case 2:
//         GX_UNIMPLEMENTED
//     case 3:
//         info.format = TextureFormat::RgbFloat32;
//         break;
//     case 4:
//         GX_UNIMPLEMENTED
//     default:
//         GX_UNEXPECTED
//     }
//     info.texture_type = Type::Texture2D;
//     std::vector<std::vector<std::uint8_t>> pixels_data(1);
//     pixels_data[0].resize(pixels.size() * sizeof(float));
//     std::memcpy(pixels_data[0].data(), pixels.data(), pixels_data[0].size());
//     return create_2d(std::move(name), std::move(pixels_data), info, img_width, img_height, c);
// }
//
// std::shared_ptr<gearoenix::render::texture::Texture2D> gearoenix::render::texture::Manager::create_2d_f(
//     std::string name,
//     const std::wstring& file_address,
//     core::sync::EndCaller<Texture2D>& c,
//     const SamplerInfo& sample_info) noexcept
//{
//     return create_2d_f(std::move(name), core::String::to_string(file_address), c, sample_info);
// }
//
// std::shared_ptr<gearoenix::render::texture::Texture2D> gearoenix::render::texture::Manager::create_2d_f(
//     std::string name,
//     const std::string& file_address,
//     core::sync::EndCaller<render::texture::Texture2D>& c,
//     const SamplerInfo& sample_info,
//     const bool relative_path) noexcept
//{
//     const std::unique_ptr<platform::stream::Asset> file(platform::stream::Asset::construct(
//         e->get_platform_application(), file_address, relative_path));
//     if (file == nullptr)
//         GXLOGF("Texture file " << file_address << " not found.")
//     file->set_endian_compatibility(true);
//     auto const file_content = file->get_file_content();
//     return create_2d_f(std::move(name), file_content.data(), file_content.size(), c, sample_info);
// }
//
// std::shared_ptr<gearoenix::render::texture::TextureCube> gearoenix::render::texture::Manager::get_cube(const math::Vec4<float>& color, core::sync::EndCaller<TextureCube>& c) noexcept
//{
//     /// TODO: It is better to have different types of color and elements
//     static_assert(sizeof(float) == 4, "Only float 32 bit are supported.");
//     const auto search = color_4d_id_cube.find(color);
//     const bool found = color_4d_id_cube.end() != search;
//     const core::Id id = found ? search->second : core::asset::Manager::create_id();
//     if (!found)
//         color_4d_id_cube[color] = id;
//     std::vector<std::uint8_t> c1(sizeof(color));
//     std::memcpy(c1.data(), reinterpret_cast<const void*>(color.data()), c1.size());
//     std::vector<std::vector<std::uint8_t>> c2 = { std::move(c1) };
//     std::vector<std::vector<std::vector<std::uint8_t>>> colors = { c2, c2, c2, c2, c2, std::move(c2) };
//     const std::function<std::shared_ptr<Texture>()> fun = [this, colors { move(colors) }, c, id]() mutable {
//         const TextureInfo txt_info {
//             .format = TextureFormat::RgbaFloat32,
//             .sampler_info = SamplerInfo {
//                 .min_filter = Filter::Nearest,
//                 .mag_filter = Filter::Nearest,
//                 .wrap_s = Wrap::Repeat,
//                 .wrap_t = Wrap::Repeat,
//                 .wrap_r = Wrap::Repeat,
//             },
//             .texture_type = Type::TextureCube,
//             .has_mipmap = false,
//         };
//         return e->create_texture_cube(
//             id, "texture-cube-colored-" + std::to_string(id), std::move(colors), txt_info, 1,
//             core::sync::EndCaller<core::sync::EndCallerIgnore>([c] {}));
//     };
//     std::shared_ptr<TextureCube> data = std::dynamic_pointer_cast<TextureCube>(cache.get_cacher().get(id, fun));
//     c.set_data(data);
//     return data;
// }
//
// std::shared_ptr<gearoenix::render::texture::TextureCube> gearoenix::render::texture::Manager::get_cube(const math::Vec3<float>& color, core::sync::EndCaller<TextureCube>& c) noexcept
//{
//     const auto search = color_3d_id_cube.find(color);
//     const bool found = color_3d_id_cube.end() != search;
//     const core::Id id = found ? search->second : core::asset::Manager::create_id();
//     if (!found)
//         color_3d_id_cube[color] = id;
//     return get_cube(math::Vec4(color, 1.0f), c);
// }
//
// std::shared_ptr<gearoenix::render::texture::TextureCube> gearoenix::render::texture::Manager::get_cube(const math::Vec2<float>& color, core::sync::EndCaller<TextureCube>& c) noexcept
//{
//     const auto search = color_2d_id_cube.find(color);
//     const bool found = color_2d_id_cube.end() != search;
//     const core::Id id = found ? search->second : core::asset::Manager::create_id();
//     if (!found)
//         color_2d_id_cube[color] = id;
//     return get_cube(math::Vec4(color, 0.0f, 1.0f), c);
// }
//
// std::shared_ptr<gearoenix::render::texture::TextureCube> gearoenix::render::texture::Manager::get_cube_zero_3c(core::sync::EndCaller<TextureCube>& c) noexcept
//{
//     if (default_zero_3c_cube == nullptr)
//         default_zero_3c_cube = get_cube(math::Vec3(0.0f, 0.0f, 0.0f), c);
//     else
//         c.set_data(default_zero_3c_cube);
//     return default_zero_3c_cube;
// }
//
// std::shared_ptr<gearoenix::render::texture::TextureCube> gearoenix::render::texture::Manager::create_cube(
//     std::string name,
//     const TextureInfo& info, const int img_aspect, core::sync::EndCaller<TextureCube>& c) noexcept
//{
//     const auto id = core::asset::Manager::create_id();
//     const std::function<std::shared_ptr<Texture>()> fun = [this, &info, name { move(name) }, img_aspect, &c, id]() mutable noexcept {
//         return e->create_texture_cube(id, std::move(name), {}, info, img_aspect, core::sync::EndCaller<core::sync::EndCallerIgnore>([c] {}));
//     };
//     auto data = std::dynamic_pointer_cast<TextureCube>(cache.get_cacher().get(id, fun));
//     c.set_data(data);
//     return data;
// }

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
        const auto l = (h * (2.0f * v.dot(h)) - v).normalized();

        const auto n_dot_l = math::Numeric::maximum(l.z, 0.0f);
        const auto n_dot_h = math::Numeric::maximum(h.z, 0.0f);
        const auto v_dot_h = math::Numeric::maximum(v.dot(h), 0.0f);

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

std::vector<gearoenix::math::Vec2<float>> gearoenix::render::texture::Manager::create_brdflut_pixels(const std::size_t resolution) noexcept
{
    std::vector<math::Vec2<float>> pixels(resolution * resolution);
    const auto inv_res = 1.0f / float(resolution);
    core::sync::ParallelFor::execi(pixels.begin(), pixels.end(), [&](math::Vec2<float>& pixel, const unsigned int index, const unsigned int) {
        pixel = integrate_brdf(
            (static_cast<float>(index % resolution) + 0.5f) * inv_res,
            (static_cast<float>(index / resolution) + 0.5f) * inv_res);
    });
    return pixels;
}
