#include "rnd-txt-manager.hpp"
#include "../../core/asset/cr-asset-manager.hpp"
#include "../../system/sys-app.hpp"
#include "../engine/rnd-eng-engine.hpp"
#include "rnd-txt-format.hpp"
#include "rnd-txt-texture-2d.hpp"

gearoenix::render::texture::Manager::Manager(const std::shared_ptr<system::stream::Stream>& s, const std::shared_ptr<engine::Engine>& e) :
	e(e),
	cache(s)
{}

gearoenix::render::texture::Manager::~Manager() {}

std::shared_ptr<gearoenix::render::texture::Texture2D> gearoenix::render::texture::Manager::get(const math::Vec4 & color, core::sync::EndCaller<Texture2D>& c)
{
	const std::shared_ptr<core::Id> id_ptr(new core::Id);
	const std::function<std::shared_ptr<Texture>()> fun = [this, color, c, id_ptr]
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
			TextureFormat::RGBA_UINT8, sample_info, 1, 1, 
			core::sync::EndCaller<core::sync::EndCallerIgnore>(c)));
		color_id[color] = *id_ptr;
		return t;
	};
	const auto &i1 = color_id.find(color);
	if (color_id.end() != i1)
	{
		*id_ptr = i1->second;
		return std::static_pointer_cast<Texture2D>(cache.get_cacher().get(i1->second, fun));
	}
	const core::Id id = e->get_system_application()->get_asset_manager()->create_id();
	*id_ptr = id;
	return std::static_pointer_cast<Texture2D>(cache.get_cacher().get(id, fun));
}
