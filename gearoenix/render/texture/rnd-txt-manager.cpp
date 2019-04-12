#include "rnd-txt-manager.hpp"
#include "../../core/asset/cr-asset-manager.hpp"
#include "../../system/sys-app.hpp"
#include "../../system/stream/sys-stm-stream.hpp"
#include "../engine/rnd-eng-engine.hpp"
#include "rnd-txt-format.hpp"
#include "rnd-txt-texture-2d.hpp"
#include "rnd-txt-texture-cube.hpp"
#include "rnd-txt-type.hpp"
#include "rnd-txt-png.hpp"

gearoenix::render::texture::Manager::Manager(const std::shared_ptr<system::stream::Stream>& s, const std::shared_ptr<engine::Engine>& e) :
	e(e),
	cache(s)
{}

gearoenix::render::texture::Manager::~Manager() {}

std::shared_ptr<gearoenix::render::texture::Texture2D> gearoenix::render::texture::Manager::get_2d(const math::Vec4 & color, core::sync::EndCaller<Texture2D>& c)
{
	std::uint8_t *cc = new std::uint8_t[4];
	cc[0] = static_cast<std::uint8_t>(color[0] * 255.001f);
	cc[1] = static_cast<std::uint8_t>(color[1] * 255.001f);
	cc[2] = static_cast<std::uint8_t>(color[2] * 255.001f);
	cc[3] = static_cast<std::uint8_t>(color[3] * 255.001f);
	SampleInfo sample_info;
	sample_info.mag_filter = Filter::NEAREST;
	sample_info.min_filter = Filter::NEAREST;
	sample_info.wrap_s = Wrap::REPEAT;
	sample_info.wrap_t = Wrap::REPEAT;
	sample_info.wrap_r = Wrap::REPEAT;
	const auto search = color_4d_id_t2d.find(color);
	const core::Id id = color_4d_id_t2d.end() != search ? search->second : core::asset::Manager::create_id();
	const std::function<std::shared_ptr<Texture>()> fun = [this, cc, c, id, sample_info]
	{
		return std::shared_ptr<Texture>(e->create_texture_2d(
			id, static_cast<const void *>(cc),
			TextureFormat::RGBA_UINT8, sample_info, 1, 1,
			core::sync::EndCaller<core::sync::EndCallerIgnore>([c, cc] { delete cc; })));
	};
	std::shared_ptr<Texture2D> data = std::static_pointer_cast<Texture2D>(cache.get_cacher().get(id, fun));
	c.set_data(data);
	return data;
}

std::shared_ptr<gearoenix::render::texture::Texture2D> gearoenix::render::texture::Manager::get_2d(const math::Vec3 & color, core::sync::EndCaller<Texture2D>& c)
{
	std::uint8_t *cc = new std::uint8_t[3];
	cc[0] = static_cast<std::uint8_t>(color[0] * 255.001f);
	cc[1] = static_cast<std::uint8_t>(color[1] * 255.001f);
	cc[2] = static_cast<std::uint8_t>(color[2] * 255.001f);
	SampleInfo sample_info;
	sample_info.mag_filter = Filter::NEAREST;
	sample_info.min_filter = Filter::NEAREST;
	sample_info.wrap_s = Wrap::REPEAT;
	sample_info.wrap_t = Wrap::REPEAT;
	sample_info.wrap_r = Wrap::REPEAT;
	const auto search = color_3d_id_t2d.find(color);
	const core::Id id = color_3d_id_t2d.end() != search ? search->second : core::asset::Manager::create_id();
	const std::function<std::shared_ptr<Texture>()> fun = [this, cc, c, id, sample_info]
	{
		return std::shared_ptr<Texture>(e->create_texture_2d(
			id, static_cast<const void *>(cc),
			TextureFormat::RGB_UINT8, sample_info, 1, 1,
			core::sync::EndCaller<core::sync::EndCallerIgnore>([c, cc] { delete cc; })));
	};
	std::shared_ptr<Texture2D> data = std::static_pointer_cast<Texture2D>(cache.get_cacher().get(id, fun));
	c.set_data(data);
	return data;
}

std::shared_ptr<gearoenix::render::texture::Texture2D> gearoenix::render::texture::Manager::get_2d(const math::Vec2 & color, core::sync::EndCaller<Texture2D>& c)
{
	std::uint8_t *cc = new std::uint8_t[2];
	cc[0] = static_cast<std::uint8_t>(color[0] * 255.001f);
	cc[1] = static_cast<std::uint8_t>(color[1] * 255.001f);
	SampleInfo sample_info;
	sample_info.mag_filter = Filter::NEAREST;
	sample_info.min_filter = Filter::NEAREST;
	sample_info.wrap_s = Wrap::REPEAT;
	sample_info.wrap_t = Wrap::REPEAT;
	sample_info.wrap_r = Wrap::REPEAT;
	const auto search = color_2d_id_t2d.find(color);
	const core::Id id = color_2d_id_t2d.end() != search ? search->second : core::asset::Manager::create_id();
	const std::function<std::shared_ptr<Texture>()> fun = [this, cc, c, id, sample_info]
	{
		return std::shared_ptr<Texture>(e->create_texture_2d(
			id, static_cast<const void *>(cc),
			TextureFormat::RG_UINT8, sample_info, 1, 1,
			core::sync::EndCaller<core::sync::EndCallerIgnore>([c, cc] { delete cc; })));
	};
	std::shared_ptr<Texture2D> data = std::static_pointer_cast<Texture2D>(cache.get_cacher().get(id, fun));
	c.set_data(data);
	return data;
}

std::shared_ptr<gearoenix::render::texture::Cube> gearoenix::render::texture::Manager::get_cube(const math::Vec4 & color, core::sync::EndCaller<Cube>& c)
{
	std::uint8_t *cc = new std::uint8_t[4 * 6];
	cc[0] = static_cast<std::uint8_t>(color[0] * 255.001f);
	cc[1] = static_cast<std::uint8_t>(color[1] * 255.001f);
	cc[2] = static_cast<std::uint8_t>(color[2] * 255.001f);
	cc[3] = static_cast<std::uint8_t>(color[3] * 255.001f);
	cc[20] = cc[16] = cc[12] = cc[8] = cc[4] = cc[0];
	cc[21] = cc[17] = cc[13] = cc[9] = cc[5] = cc[1];
	cc[22] = cc[18] = cc[14] = cc[10] = cc[6] = cc[2];
	cc[23] = cc[19] = cc[15] = cc[11] = cc[7] = cc[3];
	SampleInfo sample_info;
	sample_info.mag_filter = Filter::NEAREST;
	sample_info.min_filter = Filter::NEAREST;
	sample_info.wrap_s = Wrap::REPEAT;
	sample_info.wrap_t = Wrap::REPEAT;
	sample_info.wrap_r = Wrap::REPEAT;
	const auto search = color_4d_id_cube.find(color);
	const core::Id id = color_4d_id_cube.end() != search ? search->second : core::asset::Manager::create_id();
	const std::function<std::shared_ptr<Texture>()> fun = [this, cc, c, id, sample_info]
	{
		return std::shared_ptr<Texture>(e->create_texture_cube(
			id, static_cast<const void *>(cc),
			TextureFormat::RGBA_UINT8, sample_info, 1,
			core::sync::EndCaller<core::sync::EndCallerIgnore>([c, cc] { delete cc; })));
	};
	std::shared_ptr<Cube> data = std::static_pointer_cast<Cube>(cache.get_cacher().get(id, fun));
	c.set_data(data);
	return data;
}

std::shared_ptr<gearoenix::render::texture::Cube> gearoenix::render::texture::Manager::get_cube(const math::Vec3 & color, core::sync::EndCaller<Cube>& c)
{
	std::uint8_t *cc = new std::uint8_t[3 * 6];
	cc[0] = static_cast<std::uint8_t>(color[0] * 255.001f);
	cc[1] = static_cast<std::uint8_t>(color[1] * 255.001f);
	cc[2] = static_cast<std::uint8_t>(color[2] * 255.001f);
	cc[15] = cc[12] = cc[9] = cc[6] = cc[3] = cc[0];
	cc[16] = cc[13] = cc[10] = cc[7] = cc[4] = cc[1];
	cc[17] = cc[14] = cc[11] = cc[8] = cc[5] = cc[2];
	SampleInfo sample_info;
	sample_info.mag_filter = Filter::NEAREST;
	sample_info.min_filter = Filter::NEAREST;
	sample_info.wrap_s = Wrap::REPEAT;
	sample_info.wrap_t = Wrap::REPEAT;
	sample_info.wrap_r = Wrap::REPEAT;
	const auto search = color_3d_id_cube.find(color);
	const core::Id id = color_3d_id_cube.end() != search ? search->second : core::asset::Manager::create_id();
	const std::function<std::shared_ptr<Texture>()> fun = [this, cc, c, id, sample_info]
	{
		return std::shared_ptr<Texture>(e->create_texture_cube(
			id, static_cast<const void *>(cc),
			TextureFormat::RGB_UINT8, sample_info, 1,
			core::sync::EndCaller<core::sync::EndCallerIgnore>([c, cc] { delete cc; })));
	};
	std::shared_ptr<Cube> data = std::static_pointer_cast<Cube>(cache.get_cacher().get(id, fun));
	c.set_data(data);
	return data;
}

std::shared_ptr<gearoenix::render::texture::Cube> gearoenix::render::texture::Manager::get_cube(const math::Vec2 & color, core::sync::EndCaller<Cube>& c)
{
	std::uint8_t *cc = new std::uint8_t[2 * 6];
	cc[0] = static_cast<std::uint8_t>(color[0] * 255.001f);
	cc[1] = static_cast<std::uint8_t>(color[1] * 255.001f);
	cc[10] = cc[8] = cc[6] = cc[4] = cc[2] = cc[0];
	cc[11] = cc[9] = cc[7] = cc[5] = cc[3] = cc[1];
	SampleInfo sample_info;
	sample_info.mag_filter = Filter::NEAREST;
	sample_info.min_filter = Filter::NEAREST;
	sample_info.wrap_s = Wrap::REPEAT;
	sample_info.wrap_t = Wrap::REPEAT;
	sample_info.wrap_r = Wrap::REPEAT;
	const auto search = color_2d_id_cube.find(color);
	const core::Id id = color_2d_id_cube.end() != search ? search->second : core::asset::Manager::create_id();
	const std::function<std::shared_ptr<Texture>()> fun = [this, cc, c, id, sample_info]
	{
		return std::shared_ptr<Texture>(e->create_texture_cube(
			id, static_cast<const void *>(cc),
			TextureFormat::RG_UINT8, sample_info, 1,
			core::sync::EndCaller<core::sync::EndCallerIgnore>([c, cc] { delete cc; })));
	};
	std::shared_ptr<Cube> data = std::static_pointer_cast<Cube>(cache.get_cacher().get(id, fun));
	c.set_data(data);
	return data;
}

std::shared_ptr<gearoenix::render::texture::Texture> gearoenix::render::texture::Manager::get_gx3d(const core::Id id, core::sync::EndCaller<Texture> &c)
{
	const std::shared_ptr<Texture> o = cache.get<Texture>(id, [this, id, c] {
		const std::shared_ptr<system::stream::Stream> &f = cache.get_file();
		switch (f->read<Type::Id>())
		{
		case Type::TEXTURE_2D:
		{
			std::shared_ptr<std::vector<unsigned char>> data(new std::vector<unsigned char>);
			unsigned int img_width;
			unsigned int img_height;
			PNG::decode(f, *data, img_width, img_height);
			core::sync::EndCaller<core::sync::EndCallerIgnore> call([c, data] {});
			SampleInfo sinfo;
			return std::shared_ptr<Texture>(e->create_texture_2d(
				id, static_cast<const void *>(data->data()), TextureFormat::RGBA_UINT8, sinfo, img_width, img_height, call));
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