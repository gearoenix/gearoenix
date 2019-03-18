#include "rnd-mat-material.hpp"
#include "../engine/rnd-eng-engine.hpp"
#include "../../system/sys-app.hpp"
#include "../../core/asset/cr-asset-manager.hpp"
#include "../texture/rnd-txt-manager.hpp"
#include "../texture/rnd-txt-texture-2d.hpp"
#include "../buffer/rnd-buf-manager.hpp"
#include "../pipeline/rnd-pip-manager.hpp"

#ifdef GX_DEBUG_MODE
#define GX_DEBUG_MATERIAL_IMPORT
#endif

void gearoenix::render::material::Material::initialize()
{
	const std::shared_ptr<pipeline::Manager> &pipmgr = e->get_pipeline_manager();
	const std::shared_ptr<buffer::Manager> &bufmgr = e->get_buffer_manager();
	for (Frame &frame : frames)
	{
		frame.textures_changes = false;
		frame.uniform_buffer = std::shared_ptr<buffer::Uniform>(bufmgr->create_uniform(sizeof(Uniform), e));
        frame.pipeline_resouce = pipmgr->create_resource(frame.uniform_buffer, { color, metallic_roughness, normal, emissive });
	}
}

gearoenix::render::material::Material::Material(const std::shared_ptr<engine::Engine>& e, const core::sync::EndCaller<core::sync::EndCallerIgnore>& end)
	: e(e)
{
	core::sync::EndCaller<texture::Texture2D> calltxt2d(end);
	const std::shared_ptr<texture::Manager> &txtmgr = e->get_system_application()->get_asset_manager()->get_texture_manager();
	color = txtmgr->get(math::Vec3(0.9f, 0.1f, 0.3f), calltxt2d);
	metallic_roughness = txtmgr->get(math::Vec2(0.5f, 0.5f), calltxt2d);
	normal = txtmgr->get(math::Vec3(0.5f, 0.5f, 1.0f), calltxt2d);
	emissive = txtmgr->get(math::Vec3(0.0f, 0.0f, 0.0f), calltxt2d);
	initialize();
}

gearoenix::render::material::Material::Material(const std::shared_ptr<system::stream::Stream>& f, const std::shared_ptr<engine::Engine>& e, const core::sync::EndCaller<core::sync::EndCallerIgnore>& end)
{
#ifdef GX_DEBUG_MATERIAL_IMPORT
	bool alpha_init = false;
	bool alpha_cutoff_init = false;
	bool base_color_init = false;
	bool emissive_init = false;
	bool metallic_factor_init = false;
	bool metallic_roughness_init = false;
	bool normal_init = false;
	bool normal_scale_init = false;
	bool roughness_factor_init = false;
#endif
	enum Field : core::TypeId {
		Float = 1,
		Texture = 2,
		Vector = 3,
	};
	const std::shared_ptr<texture::Manager> &txtmgr = e->get_system_application()->get_asset_manager()->get_texture_manager();
	auto read_texture = [&, end] {
		switch (f->read<Field>())
		{
		case Field::Vector:
		{
			core::sync::EndCaller<texture::Texture2D> calltxt2d(end);
			math::Vec4 color_value;
			color_value.read(f);
			return txtmgr->get(color_value, calltxt2d);
		}
		case Field::Texture:
		{
			core::sync::EndCaller<texture::Texture> calltxt(end);
			return std::static_pointer_cast<texture::Texture2D>(txtmgr->get_gx3d(f->read<core::Id>(), calltxt));
		}
		default:
			GXUNEXPECTED;
		}
	};

	auto read_value = [&] {
		if (f->read<Field>() == Field::Float)
			return f->read<core::Real>();
		GXUNEXPECTED;
	};

	const std::uint8_t elements_count = f->read<std::uint8_t>();
	for (std::uint8_t ei = 0; ei < elements_count; ++ei)
	{
		switch (f->read<core::TypeId>())
		{
		case 1: // Alpha
#ifdef GX_DEBUG_MATERIAL_IMPORT
			if (alpha_init) GXUNEXPECTED;
			alpha_init = true;
#endif
			switch (f->read<Field>())
			{
			case Field::Float:
				f->read(uniform.alpha);
				break;
			case Field::Texture:
				f->read<core::Id>();
				translucency = TranslucencyMode::Tansparent;
				break;
			default:
				GXUNEXPECTED;
				break;
			}
			GXLOGD("Alpha is: " << uniform.alpha);
			break;
		case 2: // AlphaCutoff
#ifdef GX_DEBUG_MATERIAL_IMPORT
			if (alpha_cutoff_init) GXUNEXPECTED;
			alpha_cutoff_init = true;
#endif
			uniform.alpha_cutoff = read_value();
			GXLOGD("Alpha cutoff is: " << uniform.alpha_cutoff);
			break;
		case 3: // BaseColor
#ifdef GX_DEBUG_MATERIAL_IMPORT
			if (base_color_init) GXUNEXPECTED;
			base_color_init = true;
#endif
			color = read_texture();
			break;
		case 4: // Emissive
#ifdef GX_DEBUG_MATERIAL_IMPORT
			if (emissive_init) GXUNEXPECTED;
			emissive_init = true;
#endif
			emissive = read_texture();
			break;
		case 5: // MetallicFactor
#ifdef GX_DEBUG_MATERIAL_IMPORT
			if (metallic_factor_init) GXUNEXPECTED;
			metallic_factor_init = true;
#endif
			uniform.metallic_factor = read_value();
			break;
		case 6: // MetallicRoughness
#ifdef GX_DEBUG_MATERIAL_IMPORT
			if (metallic_roughness_init) GXUNEXPECTED;
			metallic_roughness_init = true;
#endif
			metallic_roughness = read_texture();
			break;
		case 7: // Normal
#ifdef GX_DEBUG_MATERIAL_IMPORT
			if (normal_init) GXUNEXPECTED;
			normal_init = true;
#endif
			normal = read_texture();
			break;
		case 8: // NormalScale
#ifdef GX_DEBUG_MATERIAL_IMPORT
			if (normal_scale_init) GXUNEXPECTED;
			normal_scale_init = true;
#endif
			uniform.normal_scale = read_value();
			break;
		case 9: // RoughnessFactor
#ifdef GX_DEBUG_MATERIAL_IMPORT
			if (roughness_factor_init) GXUNEXPECTED;
			roughness_factor_init = true;
#endif
			uniform.roughness_factor = read_value();
			break;
		default:
			GXUNEXPECTED;
		}
	}
#ifdef GX_DEBUG_MATERIAL_IMPORT
	if (!alpha_init) 
	{
		GXLOGF("Alpha is missing.");
	}
	if (!alpha_cutoff_init)
	{
		GXLOGF("Alpha cutoff is missing.");
	}
	if (!base_color_init)
	{
		GXLOGF("Base color is missing.");
	}
	if (!emissive_init)
	{
		GXLOGF("Emissive is missing.");
	}
	if (!metallic_factor_init)
	{
		GXLOGF("Metallic factor is missing.");
	}
	if (!metallic_roughness_init)
	{
		GXLOGF("Metallic roughness is missing.");
	}
	if (!normal_init)
	{
		GXLOGF("Normal is missing.");
	}
	if (!normal_scale_init)
	{
		GXLOGF("Normal scale is missing.");
	}
	if (!roughness_factor_init)
	{
		GXLOGF("Roughness factor is missing.");
	}
#endif
	initialize();
}

gearoenix::render::material::Material::~Material()
{
}
