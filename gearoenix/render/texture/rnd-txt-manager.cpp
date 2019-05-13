#include "rnd-txt-manager.hpp"
#include "../../core/asset/cr-asset-manager.hpp"
#include "../../system/stream/sys-stm-stream.hpp"
#include "../../system/sys-app.hpp"
#include "../engine/rnd-eng-engine.hpp"
#include "rnd-txt-format.hpp"
#include "rnd-txt-png.hpp"
#include "rnd-txt-texture-2d.hpp"
#include "rnd-txt-texture-cube.hpp"
#include "rnd-txt-type.hpp"

gearoenix::render::texture::Manager::Manager(const std::shared_ptr<system::stream::Stream>& s, const std::shared_ptr<engine::Engine>& e)
    : e(e)
	, cache(s)
{
}

gearoenix::render::texture::Manager::~Manager() {}

std::shared_ptr<gearoenix::render::texture::Texture2D> gearoenix::render::texture::Manager::get_2d(const math::Vec4& color, core::sync::EndCaller<Texture2D>& c)
{
    static_assert(sizeof(core::Real) == 4, "Only float 32 bit are supported.");
    std::shared_ptr<core::Real[]> cc(new core::Real[4]);
    cc[0] = color[0];
    cc[1] = color[1];
    cc[2] = color[2];
    cc[3] = color[3];
    SampleInfo sample_info;
    sample_info.mag_filter = Filter::NEAREST;
    sample_info.min_filter = Filter::NEAREST;
    sample_info.wrap_s = Wrap::REPEAT;
    sample_info.wrap_t = Wrap::REPEAT;
    sample_info.wrap_r = Wrap::REPEAT;
    const auto search = color_4d_id_t2d.find(color);
    const bool found = color_4d_id_t2d.end() != search;
    const core::Id id = found ? search->second : core::asset::Manager::create_id();
    if (!found)
        color_4d_id_t2d[color] = id;
    const std::function<std::shared_ptr<Texture>()> fun = [this, cc, c, id, sample_info] {
        return std::shared_ptr<Texture>(e->create_texture_2d(
            id, static_cast<const void*>(cc.get()),
            TextureFormat::RGBA_FLOAT32, sample_info, 1, 1,
            core::sync::EndCaller<core::sync::EndCallerIgnore>([c, cc] {})));
    };
    std::shared_ptr<Texture2D> data = std::static_pointer_cast<Texture2D>(cache.get_cacher().get(id, fun));
    c.set_data(data);
    return data;
}

std::shared_ptr<gearoenix::render::texture::Texture2D> gearoenix::render::texture::Manager::get_2d(const math::Vec3& color, core::sync::EndCaller<Texture2D>& c)
{
    const auto search = color_3d_id_t2d.find(color);
    const bool found = color_3d_id_t2d.end() != search;
    const core::Id id = found ? search->second : core::asset::Manager::create_id();
    if (!found)
        color_3d_id_t2d[color] = id;
    return get_2d(math::Vec4(color, 1.0f), c);
}

std::shared_ptr<gearoenix::render::texture::Texture2D> gearoenix::render::texture::Manager::get_2d(const math::Vec2& color, core::sync::EndCaller<Texture2D>& c)
{
    const auto search = color_2d_id_t2d.find(color);
    const bool found = color_2d_id_t2d.end() != search;
    const core::Id id = found ? search->second : core::asset::Manager::create_id();
    if (!found)
        color_2d_id_cube[color] = id;
    return get_2d(math::Vec4(color, 0.0f, 1.0f), c);
}

std::shared_ptr<gearoenix::render::texture::Texture2D> gearoenix::render::texture::Manager::get_2d(const core::Real value, core::sync::EndCaller<Texture2D>& c)
{
    const auto search = color_1d_id_t2d.find(value);
    const bool found = color_1d_id_t2d.end() != search;
    const core::Id id = found ? search->second : core::asset::Manager::create_id();
    if (!found)
        color_1d_id_t2d[value] = id;
    return get_2d(math::Vec4(value, 0.0f, 0.0f, 1.0f), c);
}

std::shared_ptr<gearoenix::render::texture::Cube> gearoenix::render::texture::Manager::get_cube(const math::Vec4& color, core::sync::EndCaller<Cube>& c)
{
    static_assert(sizeof(core::Real) == 4, "Only float 32 bit are supported.");
    std::shared_ptr<core::Real[]> cc(new core::Real[4]);
    cc[0] = color[0];
    cc[1] = color[1];
    cc[2] = color[2];
    cc[3] = color[3];
    SampleInfo sample_info;
    sample_info.mag_filter = Filter::NEAREST;
    sample_info.min_filter = Filter::NEAREST;
    sample_info.wrap_s = Wrap::REPEAT;
    sample_info.wrap_t = Wrap::REPEAT;
    sample_info.wrap_r = Wrap::REPEAT;
    const auto search = color_4d_id_cube.find(color);
    const bool found = color_4d_id_cube.end() != search;
    const core::Id id = found ? search->second : core::asset::Manager::create_id();
    if (!found)
        color_4d_id_cube[color] = id;
    const std::function<std::shared_ptr<Texture>()> fun = [this, cc, c, id, sample_info] {
        return std::shared_ptr<Texture>(e->create_texture_cube(
            id, static_cast<const void*>(cc.get()),
            TextureFormat::RGBA_FLOAT32, sample_info, 1,
            core::sync::EndCaller<core::sync::EndCallerIgnore>([c, cc] {})));
    };
    std::shared_ptr<Cube> data = std::static_pointer_cast<Cube>(cache.get_cacher().get(id, fun));
    c.set_data(data);
    return data;
}

std::shared_ptr<gearoenix::render::texture::Cube> gearoenix::render::texture::Manager::get_cube(const math::Vec3& color, core::sync::EndCaller<Cube>& c)
{
    const auto search = color_3d_id_cube.find(color);
    const bool found = color_3d_id_cube.end() != search;
    const core::Id id = found ? search->second : core::asset::Manager::create_id();
    if (!found)
        color_3d_id_cube[color] = id;
    return get_cube(math::Vec4(color, 1.0f), c);
}

std::shared_ptr<gearoenix::render::texture::Cube> gearoenix::render::texture::Manager::get_cube(const math::Vec2& color, core::sync::EndCaller<Cube>& c)
{
    const auto search = color_2d_id_cube.find(color);
    const bool found = color_2d_id_cube.end() != search;
    const core::Id id = found ? search->second : core::asset::Manager::create_id();
    if (!found)
        color_2d_id_cube[color] = id;
    return get_cube(math::Vec4(color, 0.0f, 1.0f), c);
}

std::shared_ptr<gearoenix::render::texture::Texture> gearoenix::render::texture::Manager::get_gx3d(const core::Id id, core::sync::EndCaller<Texture>& c)
{
    const std::shared_ptr<Texture> o = cache.get<Texture>(id, [this, id, c] {
        const std::shared_ptr<system::stream::Stream>& f = cache.get_file();
        switch (f->read<Type::Id>()) {
        case Type::TEXTURE_2D: {
            std::shared_ptr<std::vector<unsigned char>> data(new std::vector<unsigned char>);
            unsigned int img_width;
            unsigned int img_height;
            PNG::decode(f, *(data.get()), img_width, img_height);
            core::sync::EndCaller<core::sync::EndCallerIgnore> call([c, data] {});
            SampleInfo sinfo;
            return std::shared_ptr<Texture>(e->create_texture_2d(
                id, static_cast<const void*>(data->data()), TextureFormat::RGBA_UINT8, sinfo, img_width, img_height, call));
        }
        case Type::TEXTURE_3D:
            GXUNIMPLEMENTED;
            break;
        case Type::CUBE:
            GXUNIMPLEMENTED;
            break;
        default:
            GXUNEXPECTED;
            break;
        }
    });
    c.set_data(o);
    return o;
}
