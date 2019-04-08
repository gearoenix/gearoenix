#include "rnd-txt-manager.hpp"
#include "../../core/asset/cr-asset-manager.hpp"
#include "../../system/sys-app.hpp"
#include "../../system/stream/sys-stm-stream.hpp"
#include "../engine/rnd-eng-engine.hpp"
#include "rnd-txt-format.hpp"
#include "rnd-txt-texture-2d.hpp"
#include "rnd-txt-type.hpp"
#include "rnd-txt-png.hpp"

gearoenix::render::texture::Manager::Manager(const std::shared_ptr<system::stream::Stream>& s, const std::shared_ptr<engine::Engine>& e) :
	e(e),
	cache(s)
{}

gearoenix::render::texture::Manager::~Manager() {}

std::shared_ptr<gearoenix::render::texture::Texture2D> gearoenix::render::texture::Manager::get(
	const math::Vec4 & color, core::sync::EndCaller<Texture2D>& c, const unsigned int color_count)
{
	const TextureFormat::Id tex_format = 
		color_count == 4 ? TextureFormat::RGBA_UINT8 :
		color_count == 3 ? TextureFormat::RGB_UINT8 :
		color_count == 2 ? TextureFormat::RG_UINT8 : 
		TextureFormat::R_UINT8;
	const std::shared_ptr<core::Id> id_ptr(new core::Id);
	const std::function<std::shared_ptr<Texture>()> fun = [this, color, c, tex_format, id_ptr]
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
		const std::shared_ptr<Texture2D> t(e->create_texture_2d(
			*id_ptr, static_cast<const void *>(cc),
			tex_format, sample_info, 1, 1,
			core::sync::EndCaller<core::sync::EndCallerIgnore>([c] {})));
		color_id[color] = *id_ptr;
		return t;
	};
	const auto i1 = color_id.find(color);
	if (color_id.end() != i1)
	{
		*id_ptr = i1->second;
		std::shared_ptr<Texture2D> data = std::static_pointer_cast<Texture2D>(cache.get_cacher().get(i1->second, fun));
		c.set_data(data);
		return data;
	}
	const core::Id id = e->get_system_application()->get_asset_manager()->create_id();
	*id_ptr = id;
	std::shared_ptr<Texture2D> data = std::static_pointer_cast<Texture2D>(cache.get_cacher().get(id, fun));
	c.set_data(data);
	return data;
}

std::shared_ptr<gearoenix::render::texture::Texture2D> gearoenix::render::texture::Manager::get(const math::Vec3 & color, core::sync::EndCaller<Texture2D>& c)
{
	return get(math::Vec4(color, 1.0f), c, 3);
}

std::shared_ptr<gearoenix::render::texture::Texture2D> gearoenix::render::texture::Manager::get(const math::Vec2 & color, core::sync::EndCaller<Texture2D>& c)
{
	return get(math::Vec4(color, 1.0f, 1.0f), c, 2);
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