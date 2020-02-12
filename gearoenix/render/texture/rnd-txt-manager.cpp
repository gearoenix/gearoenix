#include "rnd-txt-manager.hpp"
#include "../../core/asset/cr-asset-manager.hpp"
#include "../../system/sys-app.hpp"
#include "../engine/rnd-eng-engine.hpp"
#include "rnd-txt-image.hpp"
#include "rnd-txt-texture-2d.hpp"
#include "rnd-txt-texture-cube.hpp"
#include "rnd-txt-type.hpp"
#include <array>

gearoenix::render::texture::Manager::Manager(std::unique_ptr<system::stream::Stream> s, engine::Engine* const e) noexcept
    : e(e)
    , cache(std::move(s))
{
}

std::shared_ptr<gearoenix::render::texture::Texture2D> gearoenix::render::texture::Manager::get_2d(const math::Vec4& color, core::sync::EndCaller<Texture2D>& c) noexcept
{
    static_assert(sizeof(core::Real) == 4, "Only float 32 bit are supported.");
    std::shared_ptr<std::array<core::Real, 4>> cc(new std::array<core::Real, 4>());
    (*cc)[0] = color[0];
    (*cc)[1] = color[1];
    (*cc)[2] = color[2];
    (*cc)[3] = color[3];
    SampleInfo sample_info;
    sample_info.mag_filter = Filter::NEAREST;
    sample_info.min_filter = Filter::NEAREST;
    sample_info.wrap_s = Wrap::Repeat;
    sample_info.wrap_t = Wrap::Repeat;
    sample_info.wrap_r = Wrap::Repeat;
    const auto search = color_4d_id_t2d.find(color);
    const bool found = color_4d_id_t2d.end() != search;
    const core::Id id = found ? search->second : core::asset::Manager::create_id();
    if (!found)
        color_4d_id_t2d[color] = id;
    const std::function<std::shared_ptr<Texture>()> fun = [this, cc, c, id, sample_info] {
        return e->create_texture_2d(
            id, static_cast<const void*>(cc->data()),
            TextureFormat::RgbaFloat32, sample_info, 1, 1,
            core::sync::EndCaller<core::sync::EndCallerIgnore>([c, cc] {}));
    };
    std::shared_ptr<Texture2D> data = std::dynamic_pointer_cast<Texture2D>(cache.get_cacher().get(id, fun));
    c.set_data(data);
    return data;
}

std::shared_ptr<gearoenix::render::texture::Texture2D> gearoenix::render::texture::Manager::get_2d(const math::Vec3& color, core::sync::EndCaller<Texture2D>& c) noexcept
{
    const auto search = color_3d_id_t2d.find(color);
    const bool found = color_3d_id_t2d.end() != search;
    const core::Id id = found ? search->second : core::asset::Manager::create_id();
    if (!found)
        color_3d_id_t2d[color] = id;
    return get_2d(math::Vec4(color, 1.0f), c);
}

std::shared_ptr<gearoenix::render::texture::Texture2D> gearoenix::render::texture::Manager::get_2d(const math::Vec2& color, core::sync::EndCaller<Texture2D>& c) noexcept
{
    const auto search = color_2d_id_t2d.find(color);
    const bool found = color_2d_id_t2d.end() != search;
    const core::Id id = found ? search->second : core::asset::Manager::create_id();
    if (!found)
        color_2d_id_cube[color] = id;
    return get_2d(math::Vec4(color, 0.0f, 1.0f), c);
}

std::shared_ptr<gearoenix::render::texture::Texture2D> gearoenix::render::texture::Manager::get_2d(const core::Real value, core::sync::EndCaller<Texture2D>& c) noexcept
{
    const auto search = color_1d_id_t2d.find(value);
    const bool found = color_1d_id_t2d.end() != search;
    const core::Id id = found ? search->second : core::asset::Manager::create_id();
    if (!found)
        color_1d_id_t2d[value] = id;
    return get_2d(math::Vec4(value, 0.0f, 0.0f, 1.0f), c);
}

std::shared_ptr<gearoenix::render::texture::Texture2D> gearoenix::render::texture::Manager::get_2d_one_1c(core::sync::EndCaller<Texture2D>& c) noexcept
{
    if (nullptr == default_one_1c_2d)
        default_one_1c_2d = get_2d(1.0f, c);
    else
        c.set_data(default_one_1c_2d);
    return default_one_1c_2d;
}

std::shared_ptr<gearoenix::render::texture::Texture2D> gearoenix::render::texture::Manager::get_2d_one_2c(core::sync::EndCaller<Texture2D>& c) noexcept
{
    if (nullptr == default_one_2c_2d)
        default_one_2c_2d = get_2d(1.0f, c);
    else
        c.set_data(default_one_2c_2d);
    return default_one_2c_2d;
}

std::shared_ptr<gearoenix::render::texture::Texture2D> gearoenix::render::texture::Manager::create_2d(unsigned char* const data, const Info& info, int img_width, int img_height, core::sync::EndCaller<Texture2D>& c) noexcept
{
    const auto id = core::asset::Manager::create_id();
    auto t = e->create_texture_2d(
        id,
        data,
        info.f,
        info.s,
        img_width,
        img_height,
        core::sync::EndCaller<core::sync::EndCallerIgnore>([data, c] { delete[] data; }));
    c.set_data(t);
    cache.get_cacher().get_cacheds()[id] = t;
    return t;
}

std::shared_ptr<gearoenix::render::texture::Texture2D> gearoenix::render::texture::Manager::create_2d_f(
    const unsigned char* const data,
    const std::size_t size, core::sync::EndCaller<Texture2D>& c,
    const SampleInfo& sample_info) noexcept
{
    std::size_t img_width;
    std::size_t img_height;
    std::size_t img_channels;
    std::vector<float> pixels;
    Image::decode(data, size, std::nullopt, pixels, img_width, img_height, img_channels);
    GXLOGD("Texture 2D Image imported with file size: " << size << ", width: " << img_width << " height: " << img_height << ", channels: " << img_channels)
    Info info;
    info.s = sample_info;
    switch (img_channels) {
    case 1:
        GXUNIMPLEMENTED
    case 2:
        GXUNIMPLEMENTED
    case 3:
        info.f = TextureFormat::RgbFloat32;
        break;
    case 4:
        GXUNIMPLEMENTED
    default:
        GXUNEXPECTED
    }
    const auto id = core::asset::Manager::create_id();
    auto t = e->create_texture_2d(
        id,
        pixels.data(),
        info.f,
        info.s,
        img_width,
        img_height,
        core::sync::EndCaller<core::sync::EndCallerIgnore>([c] {}));
    c.set_data(t);
    cache.get_cacher().get_cacheds()[id] = t;
    return t;
}

std::shared_ptr<gearoenix::render::texture::Texture2D> gearoenix::render::texture::Manager::create_2d_f(
    const std::wstring& file_address,
    core::sync::EndCaller<Texture2D>& c,
    const SampleInfo& sample_info) noexcept
{
    auto const file_content = system::stream::Stream::get_file_content(file_address);
    return create_2d_f(reinterpret_cast<const unsigned char*>(file_content.data()), file_content.size(), c, sample_info);
}

std::shared_ptr<gearoenix::render::texture::TextureCube> gearoenix::render::texture::Manager::get_cube(const math::Vec4& color, core::sync::EndCaller<TextureCube>& c) noexcept
{
    /// TODO: It is better to have different types of color and elements
    static_assert(sizeof(core::Real) == 4, "Only float 32 bit are supported.");
    std::shared_ptr<std::array<core::Real, 4>> cc(new std::array<core::Real, 4>());
    (*cc)[0] = color[0];
    (*cc)[1] = color[1];
    (*cc)[2] = color[2];
    (*cc)[3] = color[3];
    SampleInfo sample_info;
    sample_info.mag_filter = Filter::NEAREST;
    sample_info.min_filter = Filter::NEAREST;
    sample_info.wrap_s = Wrap::Repeat;
    sample_info.wrap_t = Wrap::Repeat;
    sample_info.wrap_r = Wrap::Repeat;
    const auto search = color_4d_id_cube.find(color);
    const bool found = color_4d_id_cube.end() != search;
    const core::Id id = found ? search->second : core::asset::Manager::create_id();
    if (!found)
        color_4d_id_cube[color] = id;
    const std::function<std::shared_ptr<Texture>()> fun = [this, cc, c, id, sample_info] {
        return e->create_texture_cube(
            id, static_cast<const void*>(cc->data()),
            TextureFormat::RgbaFloat32, sample_info, 1,
            core::sync::EndCaller<core::sync::EndCallerIgnore>([c, cc] {}));
    };
    std::shared_ptr<TextureCube> data = std::dynamic_pointer_cast<TextureCube>(cache.get_cacher().get(id, fun));
    c.set_data(data);
    return data;
}

std::shared_ptr<gearoenix::render::texture::TextureCube> gearoenix::render::texture::Manager::get_cube(const math::Vec3& color, core::sync::EndCaller<TextureCube>& c) noexcept
{
    const auto search = color_3d_id_cube.find(color);
    const bool found = color_3d_id_cube.end() != search;
    const core::Id id = found ? search->second : core::asset::Manager::create_id();
    if (!found)
        color_3d_id_cube[color] = id;
    return get_cube(math::Vec4(color, 1.0f), c);
}

std::shared_ptr<gearoenix::render::texture::TextureCube> gearoenix::render::texture::Manager::get_cube(const math::Vec2& color, core::sync::EndCaller<TextureCube>& c) noexcept
{
    const auto search = color_2d_id_cube.find(color);
    const bool found = color_2d_id_cube.end() != search;
    const core::Id id = found ? search->second : core::asset::Manager::create_id();
    if (!found)
        color_2d_id_cube[color] = id;
    return get_cube(math::Vec4(color, 0.0f, 1.0f), c);
}

std::shared_ptr<gearoenix::render::texture::TextureCube> gearoenix::render::texture::Manager::get_cube_zero_3c(core::sync::EndCaller<TextureCube>& c) noexcept
{
    if (default_zero_3c_cube == nullptr)
        default_zero_3c_cube = get_cube(math::Vec3(0.0f, 0.0f, 0.0f), c);
    else
        c.set_data(default_zero_3c_cube);
    return default_zero_3c_cube;
}

std::shared_ptr<gearoenix::render::texture::Texture> gearoenix::render::texture::Manager::get_gx3d(const core::Id id, core::sync::EndCaller<Texture>& c) noexcept
{
    const std::shared_ptr<Texture> o = cache.get<Texture>(id, [this, id, c] {
        system::stream::Stream* const f = cache.get_file();
        switch (f->read<Type>()) {
        case Type::Texture2D: {
            std::shared_ptr<std::vector<unsigned char>> data(new std::vector<unsigned char>);
            unsigned int img_width;
            unsigned int img_height;
            Image::decode(f, *(data.get()), img_width, img_height);
            core::sync::EndCaller<core::sync::EndCallerIgnore> call([c, data] {});
            SampleInfo sinfo = SampleInfo();
            return e->create_texture_2d(
                id, static_cast<const void*>(data->data()), TextureFormat::RgbaUint8, sinfo, img_width, img_height, call);
        }
        case Type::Texture3D:
            GXUNIMPLEMENTED
        case Type::TextureCube:
            GXUNIMPLEMENTED
        default:
            GXUNEXPECTED
        }
    });
    c.set_data(o);
    return o;
}

gearoenix::render::engine::Engine* gearoenix::render::texture::Manager::get_engine() const noexcept
{
    return e;
}
