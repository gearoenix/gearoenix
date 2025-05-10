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
#include <boost/mp11/algorithm.hpp>

#ifdef main
#undef main
#endif

gearoenix::render::texture::Manager::Manager()
    : Singleton(this)
{
}

gearoenix::render::texture::Manager::~Manager() = default;

void gearoenix::render::texture::Manager::read_gx3d(
    const platform::stream::Path& path,
    core::job::EndCallerShared<Texture>&& c)
{
    const auto stream = platform::stream::Stream::open(path);
    read_gx3d(*stream, std::move(c));
}

void gearoenix::render::texture::Manager::read_gx3d(
    platform::stream::Stream& stream,
    core::job::EndCallerShared<Texture>&& c)
{
    bool include_content;
    stream.read(include_content);
    std::string name;
    stream.read(name);
    TextureInfo txt_info;
    txt_info.read(stream);

    GX_LOG_D("Texture Info: " << std::to_string(txt_info));

    switch (txt_info.get_type()) {
    case Type::Texture2D: {
        const std::lock_guard _lg(textures_2d_lock);
        if (auto search = textures_2d.find(name); textures_2d.end() != search) {
            if (auto r = search->second.lock(); nullptr != r) {
                GX_ASSERT_D(r->get_info() == txt_info);
                c.set_return(std::move(r));
                GX_LOG_D("Texture2D " << name << " successfully cached.");
                return;
            }
        }
        break;
    }
    case Type::TextureCube: {
        const std::lock_guard _lg(textures_cube_lock);
        if (auto search = textures_cube.find(name); textures_cube.end() != search) {
            if (auto r = search->second.lock(); nullptr != r) {
                GX_ASSERT_D(r->get_info() == txt_info);
                c.set_return(std::move(r));
                GX_LOG_D("TextureCube: " << name << " successfully cached.");
                return;
            }
        }
        break;
    }
    default:
        GX_UNEXPECTED;
    }

    const auto is_float = format_has_float_component(txt_info.get_format());
    const auto comps_count = format_components_count(txt_info.get_format());
    const auto mips_count = Texture::compute_mipmaps_count(txt_info);
    const auto comp_bits = format_component_bits_count(txt_info.get_format());
    if (is_float) {
        GX_ASSERT(32 == comp_bits);
        if (!engine::Engine::get().get_specification().is_float_texture_supported) {
            txt_info.set_format(TextureFormat::RgbaUint8); // No other format is supported
        }
    } else {
        GX_ASSERT(8 == comp_bits);
    }
    const auto fill_mips = [&](std::vector<std::vector<std::uint8_t>>& pixels) {
        pixels.resize(mips_count);
        auto mip_w = txt_info.get_width();
        auto mip_h = txt_info.get_height();
        std::vector<std::uint8_t> data;
        for (std::uint32_t mip_index = 0; mip_index < mips_count; ++mip_index, mip_w >>= 1, mip_h >>= 1) {
            auto& mip_pixels = pixels[mip_index];
            stream.read(data);
            if (is_float) {
                std::vector<float> float_pixels;
                std::uint32_t decode_w = 0, decode_h = 0, ignored = 0;
                Image::decode(data.data(), static_cast<std::uint32_t>(data.size()), comps_count, float_pixels, decode_w, decode_h, ignored);
                GX_ASSERT(mip_w == decode_w);
                GX_ASSERT(mip_h == decode_h);
                if (engine::Engine::get().get_specification().is_float_texture_supported) {
                    mip_pixels.resize(float_pixels.size() << 2);
                    std::memcpy(mip_pixels.data(), float_pixels.data(), mip_pixels.size());
                } else {
                    GX_LOG_D("Loading float texture in a an unsupported engine, inefficient load of texture.");
                    mip_pixels.resize((float_pixels.size() * 4) / comps_count); // 4 is because we use RgbaUint8
                    for (std::uint32_t pixel_index = 0, float_index = 0; pixel_index < mip_pixels.size();) {
                        std::uint32_t comp_index = 0;
                        for (; comp_index < comps_count; ++comp_index, ++pixel_index, ++float_index) {
                            const float f = float_pixels[float_index];
                            mip_pixels[pixel_index] = f >= 1.0f ? 225 : f <= 0.0f ? 0
                                                                                  : static_cast<std::uint8_t>(std::lroundf(f * 255.0f));
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
                std::uint32_t decode_w = 0, decode_h = 0, ignored = 0;
                Image::decode(data.data(), static_cast<std::uint32_t>(data.size()), comps_count, mip_pixels, decode_w, decode_h, ignored);
                GX_ASSERT(mip_w == decode_w);
                GX_ASSERT(mip_h == decode_h);
            }
        }
    };
    if (Type::TextureCube == txt_info.get_type()) {
        GX_ASSERT(txt_info.get_width() == txt_info.get_height());
        std::vector<std::vector<std::vector<std::uint8_t>>> pixels(include_content ? 6 : 0);
        for (auto& face_pixels : pixels) {
            fill_mips(face_pixels);
        }
        return create_cube_from_pixels(
            std::move(name),
            std::move(pixels),
            txt_info,
            core::job::EndCallerShared<TextureCube>([c = std::move(c)](std::shared_ptr<TextureCube>&& t) {
                c.set_return(std::move(t));
            }));
    }
    if (Type::Texture2D == txt_info.get_type()) {
        std::vector<std::vector<std::uint8_t>> pixels;
        if (include_content) {
            fill_mips(pixels);
        }
        return create_2d_from_pixels(
            std::move(name), std::move(pixels), txt_info,
            core::job::EndCallerShared<Texture2D>([c = std::move(c)](std::shared_ptr<Texture2D>&& t) {
                c.set_return(std::move(t));
            }));
    }
    GX_UNIMPLEMENTED;
}

void gearoenix::render::texture::Manager::create_2d_from_colour(
    const math::Vec4<float>& colour,
    core::job::EndCallerShared<Texture2D>&& c)
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
        const std::lock_guard _lg(textures_2d_lock);
        if (const auto search = textures_2d.find(name); textures_2d.end() != search) {
            if (auto r = search->second.lock(); nullptr != r) {
                c.set_return(std::move(r));
                GX_LOG_D("Texture2D " << name << " successfully cached.");
                return;
            }
        }
    }
    std::vector<std::vector<std::uint8_t>> pixels { std::move(pixels0) };
    create_2d_from_pixels(
        std::move(name),
        std::move(pixels),
        TextureInfo()
            .set_format(TextureFormat::RgbaUint8)
            .set_sampler_info(SamplerInfo()
                    .set_min_filter(Filter::Nearest)
                    .set_mag_filter(Filter::Nearest)
                    .set_wrap_s(Wrap::Repeat)
                    .set_wrap_t(Wrap::Repeat)
                    .set_wrap_r(Wrap::Repeat))
            .set_width(1)
            .set_height(1)
            .set_type(Type::Texture2D)
            .set_has_mipmap(false),
        std::move(c));
}

void gearoenix::render::texture::Manager::get_brdflut(core::job::EndCallerShared<Texture2D>&& c)
{
    {
        const std::lock_guard _lg(brdflut_lock);
        if (nullptr != brdflut) {
            c.set_return(std::shared_ptr(brdflut));
            GX_LOG_D("Texture2D brdflut successfully cached.");
            return;
        }
    }
    constexpr auto file_name = "default-brdflut.gx-2d-texture";
    {
        std::unique_ptr<platform::stream::Stream> stream(platform::stream::Asset::construct(file_name));
        if (nullptr == stream) {
            stream = std::unique_ptr<platform::stream::Stream>(platform::stream::Local::open(file_name));
        }
        if (nullptr != stream) {
            read_gx3d(
                *stream,
                core::job::EndCallerShared<Texture>([this, c = std::move(c)](std::shared_ptr<Texture>&& t) {
                    {
                        const std::lock_guard _lg(brdflut_lock);
                        brdflut = std::dynamic_pointer_cast<Texture2D>(std::move(t));
                    }
                    GX_ASSERT(nullptr != brdflut);
                    c.set_return(std::shared_ptr(brdflut));
                }));
            return;
        }
    }
    constexpr std::uint32_t resolution = 256;
    const auto texture_info = TextureInfo()
                                  .set_format(TextureFormat::RgbaUint8)
                                  .set_sampler_info(SamplerInfo()
                                          .set_min_filter(Filter::Linear)
                                          .set_mag_filter(Filter::Linear)
                                          .set_wrap_s(Wrap::ClampToEdge)
                                          .set_wrap_t(Wrap::ClampToEdge)
                                          .set_wrap_r(Wrap::ClampToEdge))
                                  .set_width(resolution)
                                  .set_height(resolution)
                                  .set_type(Type::Texture2D)
                                  .set_has_mipmap(false);
    const auto pixels_vectors = create_brdflut_pixels();
    std::vector<std::uint8_t> pixels0(pixels_vectors.size() * sizeof(math::Vec4<std::uint8_t>));
    std::memcpy(pixels0.data(), pixels_vectors.data(), pixels0.size());
    std::vector<std::vector<std::uint8_t>> pixels { std::move(pixels0) };
    create_2d_from_pixels(
        "default-brdflut", std::move(pixels), texture_info,
        core::job::EndCallerShared<Texture2D>([this, c = std::move(c)](std::shared_ptr<Texture2D>&& t) mutable {
            {
                const std::lock_guard _lg(brdflut_lock);
                brdflut = std::move(t);
            }
            GX_ASSERT(nullptr != brdflut);
            c.set_return(std::shared_ptr(brdflut));
            const std::shared_ptr<platform::stream::Stream> stream(new platform::stream::Local(file_name, true));
            brdflut->write(stream, core::job::EndCaller([] { }));
        }));
}

void gearoenix::render::texture::Manager::get_checker(core::job::EndCallerShared<Texture2D>&& c)
{
    {
        const std::lock_guard _lg(checkers_lock);
        if (nullptr != checkers) {
            c.set_return(std::shared_ptr(checkers));
            GX_LOG_D("Texture2D checkers successfully cached.");
            return;
        }
    }
    auto texture_info = TextureInfo()
                            .set_format(TextureFormat::RgbaUint8)
                            .set_sampler_info(SamplerInfo()
                                    .set_min_filter(Filter::Nearest)
                                    .set_mag_filter(Filter::Nearest)
                                    .set_wrap_s(Wrap::Repeat)
                                    .set_wrap_t(Wrap::Repeat)
                                    .set_wrap_r(Wrap::Repeat))
                            .set_width(2)
                            .set_height(2)
                            .set_type(Type::Texture2D)
                            .set_has_mipmap(false);
    std::vector<std::vector<std::uint8_t>> pixels { {
        0u, 0u, 0u, 255u, // Pixel 0
        255u, 255u, 255u, 255u, // Pixel 1
        255u, 255u, 255u, 255u, // Pixel 2
        0u, 0u, 0u, 255u, // Pixel 3
    } };
    create_2d_from_pixels(
        "default-checker", std::move(pixels), texture_info,
        core::job::EndCallerShared<Texture2D>([this, c = std::move(c)](std::shared_ptr<Texture2D>&& t) {
            {
                const std::lock_guard _lg(checkers_lock);
                checkers = std::move(t);
            }
            c.set_return(std::shared_ptr(checkers));
        }));
}

void gearoenix::render::texture::Manager::create_2d_from_pixels(
    std::string&& name,
    std::vector<std::vector<std::uint8_t>>&& pixels,
    const TextureInfo& info,
    core::job::EndCallerShared<Texture2D>&& c)
{
    {
        const std::lock_guard _lg(textures_2d_lock);
        if (const auto search = textures_2d.find(name); textures_2d.end() != search) {
            if (auto r = search->second.lock(); nullptr != r) {
                GX_ASSERT_D(info == r->get_info());
                c.set_return(std::move(r));
                GX_LOG_D("Texture2D " << name << " successfully cached.");
                return;
            }
        }
    }
    core::job::EndCallerShared<Texture2D> end_caller([this, c = std::move(c), n = name](std::shared_ptr<Texture2D>&& t) mutable {
        {
            const std::lock_guard _lg(textures_2d_lock);
            textures_2d.emplace(std::move(n), std::shared_ptr(t));
        }
        c.set_return(std::move(t));
    });
    create_2d_from_pixels_v(std::move(name), std::move(pixels), info, std::move(end_caller));
}

void gearoenix::render::texture::Manager::create_2d_from_formatted(
    std::string&& name,
    const void* const data,
    const std::uint32_t size,
    const TextureInfo& info,
    core::job::EndCallerShared<Texture2D>&& c)
{
    {
        const std::lock_guard _lg(textures_2d_lock);
        if (auto search = textures_2d.find(name); textures_2d.end() != search) {
            if (auto r = search->second.lock(); nullptr != r) {
                GX_ASSERT_D(info == r->get_info());
                c.set_return(std::move(r));
                GX_LOG_D("Texture2D " << name << " successfully cached.");
                return;
            }
        }
    }
    GX_ASSERT(Type::Unknown == info.get_type()); // type converting is not implemented and is not going to be implemented soon.
    GX_ASSERT(TextureFormat::Unknown == info.get_format()); // format converting does not have a high priority
    GX_ASSERT(0 == info.get_width()); // dimension changing does not have a high priority
    GX_ASSERT(0 == info.get_height()); // dimension changing does not have a high priority
    std::uint32_t img_width = 0;
    std::uint32_t img_height = 0;
    std::uint32_t img_channels = 0;
    std::vector<std::uint8_t> pixels0;
    Image::decode(
        static_cast<const unsigned char*>(data), size, 4,
        pixels0, img_width, img_height, img_channels);
    GX_LOG_D("Texture 2D Image imported with file size: " << size << ", width: " << img_width << " height: " << img_height << ", channels: " << img_channels);
    TextureInfo new_info = info;
    new_info.set_format(TextureFormat::RgbaUint8);
    new_info.set_type(Type::Texture2D);
    new_info.set_width(img_width);
    new_info.set_height(img_height);
    create_2d_from_pixels(std::move(name), { pixels0 }, new_info, std::move(c));
}

void gearoenix::render::texture::Manager::create_2df_from_formatted(
    std::string&& name,
    const void* const data,
    std::uint32_t size,
    const TextureInfo& info,
    core::job::EndCallerShared<Texture2D>&& c)
{
    {
        const std::lock_guard _lg(textures_2d_lock);
        if (auto search = textures_2d.find(name); textures_2d.end() != search) {
            if (auto r = search->second.lock(); nullptr != r) {
                GX_ASSERT_D(info == r->get_info());
                c.set_return(std::move(r));
                GX_LOG_D("Texture2D " << name << " successfully cached.");
                return;
            }
        }
    }
    GX_ASSERT(Type::Unknown == info.get_type()); // type converting is not implemented and is not going to be implemented soon.
    GX_ASSERT(TextureFormat::Unknown == info.get_format()); // format converting does not have a high priority
    GX_ASSERT(0 == info.get_width()); // dimension changing does not have a high priority
    GX_ASSERT(0 == info.get_height()); // dimension changing does not have a high priority
    std::uint32_t img_width = 0;
    std::uint32_t img_height = 0;
    std::uint32_t img_channels = 0;
    std::vector<float> pixels0f;
    Image::decode(
        static_cast<const unsigned char*>(data), size, 4,
        pixels0f, img_width, img_height, img_channels);
    GX_LOG_D("Texture 2D Image imported with file size: " << size << ", width: " << img_width << " height: " << img_height << ", channels: " << img_channels);
    std::vector<std::uint8_t> pixels0;
    TextureInfo new_info = info;
    if (engine::Engine::get().get_specification().is_float_texture_supported) {
        new_info.set_format(TextureFormat::RgbaFloat32);
        const auto pixels0_sz = pixels0f.size() * sizeof(float);
        pixels0.resize(pixels0_sz);
        std::memcpy(pixels0.data(), pixels0f.data(), pixels0_sz);
    } else {
        new_info.set_format(TextureFormat::RgbaUint8);
        pixels0.resize(pixels0f.size());
        core::sync::ParallelFor::execi(pixels0f.begin(), pixels0f.end(), [&](const float p, const unsigned int index, unsigned int) {
            pixels0[index] = p >= 1.0f ? 255 : p <= 0.0f ? 0
                                                         : static_cast<std::uint8_t>(p * 255.0f + 0.501f);
        });
    }
    new_info.set_type(Type::Texture2D);
    new_info.set_width(img_width);
    new_info.set_height(img_height);
    create_2d_from_pixels(std::move(name), { std::move(pixels0) }, new_info, std::move(c));
}

void gearoenix::render::texture::Manager::create_2d_from_file(
    const platform::stream::Path& path,
    const TextureInfo& info,
    core::job::EndCallerShared<Texture2D>&& c)
{
    {
        const std::lock_guard _lg(textures_2d_lock);
        if (const auto search = textures_2d.find(path.get_raw_data()); textures_2d.end() != search) {
            if (auto r = search->second.lock(); nullptr != r) {
                GX_ASSERT_D(info == r->get_info());
                c.set_return(std::move(r));
                GX_LOG_D("Texture2D " << path.get_raw_data() << " successfully cached.");
                return;
            }
        }
    }
    const auto stream = platform::stream::Stream::open(path);
    GX_ASSERT(nullptr != stream);
    const auto data = stream->get_file_content();
    if (path.get_raw_data().ends_with(".hdr")) {
        create_2df_from_formatted(std::string(path.get_raw_data()), data.data(), static_cast<std::uint32_t>(data.size()), info, std::move(c));
        return;
    }
    return create_2d_from_formatted(std::string(path.get_raw_data()), data.data(), static_cast<std::uint32_t>(data.size()), info, std::move(c));
}

void gearoenix::render::texture::Manager::create_cube_from_colour(
    const math::Vec4<float>& colour, core::job::EndCallerShared<TextureCube>&& c)
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
        const std::lock_guard _lg(textures_cube_lock);
        if (const auto search = textures_cube.find(name); textures_cube.end() != search) {
            if (auto r = search->second.lock(); nullptr != r) {
                c.set_return(std::move(r));
                GX_LOG_D("TextureCube " << name << " successfully cached.");
                return;
            }
        }
    }
    std::vector<std::vector<std::vector<std::uint8_t>>> pixels { { pixels0 }, { pixels0 }, { pixels0 }, { pixels0 }, { pixels0 }, { pixels0 } };
    return create_cube_from_pixels(
        std::move(name),
        std::move(pixels),
        TextureInfo()
            .set_format(TextureFormat::RgbaUint8)
            .set_sampler_info(
                SamplerInfo()
                    .set_min_filter(Filter::Nearest)
                    .set_mag_filter(Filter::Nearest)
                    .set_wrap_s(Wrap::Repeat)
                    .set_wrap_t(Wrap::Repeat)
                    .set_wrap_r(Wrap::Repeat))
            .set_width(1)
            .set_height(1)
            .set_type(Type::TextureCube)
            .set_has_mipmap(false),
        std::move(c));
}

void gearoenix::render::texture::Manager::create_cube_from_pixels(
    std::string&& name, std::vector<std::vector<std::vector<std::uint8_t>>>&& pixels, const TextureInfo& info,
    core::job::EndCallerShared<TextureCube>&& c)
{
    {
        const std::lock_guard _lg(textures_cube_lock);
        if (const auto search = textures_cube.find(name); textures_cube.end() != search) {
            if (auto r = search->second.lock(); nullptr != r) {
                GX_ASSERT_D(info == r->get_info());
                c.set_return(std::move(r));
                GX_LOG_D("TextureCube " << name << " successfully cached.");
                return;
            }
        }
    }
    core::job::EndCallerShared<TextureCube> end([this, c = std::move(c), n = name](std::shared_ptr<TextureCube>&& t) mutable {
        {
            const std::lock_guard _lg(textures_cube_lock);
            textures_cube.emplace(std::move(n), std::shared_ptr(t));
        }
        c.set_return(std::move(t));
    });
    create_cube_from_pixels_v(std::move(name), std::move(pixels), info, std::move(end));
}

void gearoenix::render::texture::Manager::create_target(
    std::string&& name, std::vector<Attachment>&& attachments, core::job::EndCallerShared<Target>&& c)
{
    {
        const std::lock_guard _lg(targets_lock);
        if (const auto search = targets.find(name); targets.end() != search) {
            if (auto r = search->second.lock(); nullptr != r) {
                c.set_return(std::move(r));
                GX_LOG_D("Target " << name << " successfully cached.");
                return;
            }
        }
    }
    core::job::EndCallerShared<Target> end_caller([this, c = std::move(c), n = name](std::shared_ptr<Target>&& t) mutable {
        {
            const std::lock_guard _lg(targets_lock);
            targets.emplace(std::move(n), std::shared_ptr(t));
        }
        c.set_return(std::move(t));
    });
    create_target_v(std::move(name), std::move(attachments), std::move(end_caller));
}

void gearoenix::render::texture::Manager::create_target(
    std::shared_ptr<platform::stream::Stream>&& stream,
    core::job::EndCallerShared<Target>&& c)
{
    core::job::send_job_to_pool([this, s = std::move(stream), c = std::move(c)]() mutable {
        std::string n;
        s->read(n);
        {
            const std::lock_guard _lg(targets_lock);
            if (const auto search = targets.find(n); targets.end() != search) {
                if (auto r = search->second.lock(); nullptr != r) {
                    c.set_return(std::move(r));
                    GX_LOG_D("Target " << n << " successfully cached.");
                    return;
                }
            }
        }
        Attachment::read(std::move(s), core::job::EndCaller<std::vector<Attachment>>([this, n = std::move(n), e = std::move(c)](std::vector<Attachment>&& a) mutable {
            create_target(std::move(n), std::move(a), std::move(e));
        }));
    });
}

gearoenix::math::Vec2<float> gearoenix::render::texture::Manager::integrate_brdf(const float n_dot_v, const float roughness)
{
    const math::Vec3 v(std::sqrt(1.0f - n_dot_v * n_dot_v), 0.0f, n_dot_v);

    float a = 0.0f;
    float b = 0.0f;

    constexpr auto n = math::Vec3(0.0f, 0.0f, 1.0f);

    constexpr unsigned int SAMPLE_COUNT = 1024u;
    for (unsigned int i = 0u; i < SAMPLE_COUNT; ++i) {
        // generates a sample vector biased towards the
        // preferred alignment direction (importance sampling).
        const auto xi = math::Vec2<float>::hammersley(i, SAMPLE_COUNT);
        const auto h = math::Vec3<float>::importance_sample_ggx(xi, n, roughness);
        const auto l = (h * (2.0f * v.dot(h)) - v).normalised();

        const auto n_dot_l = l.z > 0.0f ? l.z : 0.0f;
        const auto n_dot_h = h.z > 0.0f ? h.z : 0.0f;
        const auto temp = v.dot(h);
        const auto v_dot_h = temp > 0.0f ? temp : 0.0f;

        if (n_dot_l > 0.0f) {
            const auto g = geometry_smith(n, v, l, roughness);
            const auto g_vis = (g * v_dot_h) / (n_dot_h * n_dot_v);
            auto tmp = 1.0f - v_dot_h;
            const auto tmp_p2 = tmp * tmp;
            const auto fc = tmp * tmp_p2 * tmp_p2;
            tmp = fc * g_vis;
            a += g_vis - tmp;
            b += tmp;
        }
    }
    a /= static_cast<float>(SAMPLE_COUNT);
    b /= static_cast<float>(SAMPLE_COUNT);
    return { a, b };
}

std::vector<gearoenix::math::Vec4<std::uint8_t>> gearoenix::render::texture::Manager::create_brdflut_pixels(const std::uint32_t resolution)
{
    std::vector<math::Vec4<std::uint8_t>> pixels(resolution * resolution);
    const auto inv_res = 1.0f / static_cast<float>(resolution);
    core::sync::ParallelFor::execi(pixels.begin(), pixels.end(), [&](auto& pixel, const std::uint32_t index, auto) {
        const auto roughness_index = (resolution - 1) - index / resolution;
        auto p = integrate_brdf(
            (static_cast<float>(index % resolution) + 0.5f) * inv_res,
            (static_cast<float>(roughness_index) + 0.5f) * inv_res);
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

gearoenix::math::Vec2<std::uint32_t> gearoenix::render::texture::Manager::get_default_camera_render_target_dimensions() const
{
    switch (auto& resolution = RuntimeConfiguration::get().get_runtime_resolution().get(); resolution.get_index()) {
    case Resolution::fixed_index: {
        const auto [width, height] = resolution.get_fixed();
        return { width, height };
    }
    case Resolution::screen_based_index: {
        const auto [nom, dom] = resolution.get_screen_based();
        const auto wh = (platform::Application::get().get_base().get_window_size() * static_cast<int>(nom)) / static_cast<int>(dom);
        return math::Vec2<std::uint32_t> { wh };
    }
    default:
        GX_UNEXPECTED;
    }
}

void gearoenix::render::texture::Manager::create_default_camera_render_target(
    const std::string& camera_name,
    core::job::EndCaller<DefaultCameraTargets>&& callback)
{
    static std::atomic unique_id = 0;
    ++unique_id; // This allows having multiple initialisations.

    const auto unique_id_str = std::to_string(unique_id);

    auto first_colour_name = camera_name + "-first-render-target-colour-" + unique_id_str;
    auto depth_name = camera_name + "-render-target-depth-" + unique_id_str;
    auto second_colour_name = camera_name + "-second-render-target-colour-" + unique_id_str;

    struct TargetGatherer {
        core::job::EndCaller<DefaultCameraTargets> callback;

        DefaultCameraTargets targets;

        ~TargetGatherer()
        {
            for (const auto& ts : targets.targets) {
                for (const auto& t : ts) {
                    GX_ASSERT_D(nullptr != t);
                }
            }

            callback.set_return(std::move(targets));
        }
    };

    struct TextureGatherer {
        core::job::EndCaller<DefaultCameraTargets> callback;
        Manager* const manager;
        const std::string camera_name;
        const std::string unique_id_str;

        std::array<std::shared_ptr<Texture2D>, 2> colours;
        std::shared_ptr<Texture2D> depth;

        ~TextureGatherer()
        {
            GX_ASSERT_D(nullptr != colours[0]);
            GX_ASSERT_D(nullptr != colours[1]);
            GX_ASSERT_D(nullptr != depth);

            auto target_gatherer = std::make_shared<TargetGatherer>(std::move(callback));

            const auto target_name = camera_name + "-render-target-uid:" + unique_id_str;

            manager->create_target(
                target_name + "-main",
                std::vector {
                    Attachment { Attachment2D { std::shared_ptr(colours[0]) } },
                    Attachment { Attachment2D { std::move(depth) } },
                },
                core::job::EndCallerShared<Target>([target_gatherer](std::shared_ptr<Target>&& t) {
                    target_gatherer->targets.main = std::move(t);
                }));

            for (std::uint32_t target_index = 0; target_index < target_gatherer->targets.targets.size(); ++target_index) {
                const auto target_target_name = target_name + "-index:" + std::to_string(target_index) + "-mip:";
                for (std::uint32_t mip_index = 0; mip_index < target_gatherer->targets.targets[0].size(); ++mip_index) {
                    manager->create_target(
                        target_target_name + std::to_string(mip_index),
                        std::vector {
                            Attachment {
                                Attachment2D { std::shared_ptr(colours[target_index]) },
                                static_cast<decltype(Attachment::mipmap_level)>(mip_index) } },
                        core::job::EndCallerShared<Target>(
                            [target_gatherer, mip_index, target_index](std::shared_ptr<Target>&& t) {
                                target_gatherer->targets.targets[target_index][mip_index] = std::move(t);
                            }));
                }
            }
        }
    };

    auto texture_gatherer = std::make_shared<TextureGatherer>(std::move(callback), this, camera_name, unique_id_str);

    const auto dim = get_default_camera_render_target_dimensions();

    const auto txt_info = TextureInfo()
                              .set_format(TextureFormat::RgbaFloat16)
                              .set_sampler_info(SamplerInfo()
                                      .set_min_filter(Filter::LinearMipmapLinear)
                                      .set_mag_filter(Filter::Linear)
                                      .set_wrap_s(Wrap::ClampToEdge)
                                      .set_wrap_t(Wrap::ClampToEdge)
                                      .set_wrap_r(Wrap::ClampToEdge))
                              .set_width(dim.x)
                              .set_height(dim.y)
                              .set_type(Type::Texture2D)
                              .set_has_mipmap(true);

    create_2d_from_pixels(
        std::move(first_colour_name), {}, txt_info,
        core::job::EndCallerShared<Texture2D>([texture_gatherer](std::shared_ptr<Texture2D>&& t) {
            texture_gatherer->colours[0] = std::move(t);
        }));

    create_2d_from_pixels(
        std::move(second_colour_name), {}, txt_info,
        core::job::EndCallerShared<Texture2D>([texture_gatherer](std::shared_ptr<Texture2D>&& t) {
            texture_gatherer->colours[1] = std::move(t);
        }));

    auto depth_info = txt_info;
    depth_info.set_format(TextureFormat::D32);
    depth_info.set_sampler_info(
        SamplerInfo(depth_info.get_sampler_info())
            .set_min_filter(Filter::Nearest)
            .set_mag_filter(Filter::Nearest));
    depth_info.set_has_mipmap(false);

    create_2d_from_pixels(
        std::move(depth_name), {}, depth_info,
        core::job::EndCallerShared<Texture2D>([texture_gatherer = std::move(texture_gatherer)](std::shared_ptr<Texture2D>&& t) {
            texture_gatherer->depth = std::move(t);
        }));
}
