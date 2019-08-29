#include "rnd-mat-material.hpp"
#include "../../core/asset/cr-asset-manager.hpp"
#include "../../system/sys-app.hpp"
#include "../buffer/rnd-buf-framed-uniform.hpp"
#include "../engine/rnd-eng-engine.hpp"
#include "../pipeline/rnd-pip-manager.hpp"
#include "../texture/rnd-txt-manager.hpp"
#include "../texture/rnd-txt-texture-2d.hpp"

#ifdef GX_DEBUG_MODE
#define GX_DEBUG_MATERIAL_IMPORT
#endif

gearoenix::render::material::Material::Material(engine::Engine* const e, const core::sync::EndCaller<core::sync::EndCallerIgnore>& end) noexcept
    : e(e)
    , uniform_buffers(std::make_shared<buffer::FramedUniform>(sizeof(Uniform), e))
{
    core::sync::EndCaller<texture::Texture2D> calltxt2d([end](std::shared_ptr<texture::Texture2D>) {});
    const std::shared_ptr<texture::Manager>& txtmgr = e->get_system_application()->get_asset_manager()->get_texture_manager();
    color = txtmgr->get_2d(math::Vec3(1.0f, 0.0f, 0.0f), calltxt2d);
    metallic_roughness = txtmgr->get_2d(math::Vec2(0.5f, 0.5f), calltxt2d);
    normal = txtmgr->get_2d(math::Vec3(0.5f, 0.5f, 1.0f), calltxt2d);
    emission = txtmgr->get_2d(math::Vec3(0.0f, 0.0f, 0.0f), calltxt2d);
}

gearoenix::render::material::Material::Material(system::stream::Stream* const f, engine::Engine* const e, const core::sync::EndCaller<core::sync::EndCallerIgnore>& end) noexcept
    : e(e)
    , uniform_buffers(std::make_shared<buffer::FramedUniform>(sizeof(Uniform), e))
{
    const std::shared_ptr<texture::Manager>& txtmgr = e->get_system_application()->get_asset_manager()->get_texture_manager();
    
	// Reading alpha
	if (f->read_bool()) 
	{
		uniform.alpha = 1.0f;
	}
	else 
	{
		f->read(uniform.alpha);
	}
	// Reading color
	if (f->read_bool())
	{
        core::sync::EndCaller<texture::Texture> tcall([end](std::shared_ptr<texture::Texture>) {});
		color = std::dynamic_pointer_cast<texture::Texture2D>(txtmgr->get_gx3d(f->read<core::Id>(), tcall));
	}
	else {
        core::sync::EndCaller<texture::Texture2D> tcall([end](std::shared_ptr<texture::Texture2D>) {});
		math::Vec4 color_value;
		color_value.read(f);
		color = std::dynamic_pointer_cast<texture::Texture2D>(txtmgr->get_2d(color_value, tcall));
	}
	// Reading emission
	if (f->read_bool())
	{
        core::sync::EndCaller<texture::Texture> tcall([end](std::shared_ptr<texture::Texture>) {});
		emission = std::dynamic_pointer_cast<texture::Texture2D>(txtmgr->get_gx3d(f->read<core::Id>(), tcall));
	}
	else {
        core::sync::EndCaller<texture::Texture2D> tcall([end](std::shared_ptr<texture::Texture2D>) {});
		math::Vec4 emission_value;
		emission_value.read(f);
		emission = std::dynamic_pointer_cast<texture::Texture2D>(txtmgr->get_2d(emission_value, tcall));
	}
	// Reading metallic_roughness
	if (f->read_bool())
	{
        core::sync::EndCaller<texture::Texture> tcall([end](std::shared_ptr<texture::Texture>) {});
		metallic_roughness = std::dynamic_pointer_cast<texture::Texture2D>(txtmgr->get_gx3d(f->read<core::Id>(), tcall));
	}
	else {
        core::sync::EndCaller<texture::Texture2D> tcall([end](std::shared_ptr<texture::Texture2D>) {});
		math::Vec2 metallic_roughness_value;
		metallic_roughness_value.read(f);
		metallic_roughness = std::dynamic_pointer_cast<texture::Texture2D>(txtmgr->get_2d(metallic_roughness_value, tcall));
	}
	// Reading normal
	if (f->read_bool())
	{
        core::sync::EndCaller<texture::Texture> tcall([end](std::shared_ptr<texture::Texture>) {});
		normal = std::dynamic_pointer_cast<texture::Texture2D>(txtmgr->get_gx3d(f->read<core::Id>(), tcall));
	}
	else {
        core::sync::EndCaller<texture::Texture2D> tcall([end](std::shared_ptr<texture::Texture2D>) {});
		normal = std::dynamic_pointer_cast<texture::Texture2D>(txtmgr->get_2d(math::Vec3(0.5f, 0.5f, 1.0f), tcall));
	}
	// Translucency
	if (f->read_bool()) translucency = TranslucencyMode::Tansparent;
	is_shadow_caster = f->read_bool();
	f->read(uniform.alpha_cutoff);
}

void gearoenix::render::material::Material::update_uniform() noexcept
{
    uniform_buffers->update(&uniform);
}

const std::shared_ptr<gearoenix::render::buffer::FramedUniform>& gearoenix::render::material::Material::get_uniform_buffers() const noexcept
{
    return uniform_buffers;
}

const std::shared_ptr<gearoenix::render::texture::Texture2D>& gearoenix::render::material::Material::get_color() const noexcept
{
    return color;
}

const std::shared_ptr<gearoenix::render::texture::Texture2D>& gearoenix::render::material::Material::get_metallic_roughness() const noexcept
{
    return metallic_roughness;
}

const std::shared_ptr<gearoenix::render::texture::Texture2D>& gearoenix::render::material::Material::get_normal() const noexcept
{
    return normal;
}

const std::shared_ptr<gearoenix::render::texture::Texture2D>& gearoenix::render::material::Material::get_emissive() const noexcept
{
    return emission;
}

bool gearoenix::render::material::Material::get_is_shadow_caster() const noexcept
{
    return is_shadow_caster;
}

void gearoenix::render::material::Material::set_metallic_factor(const core::Real f) noexcept
{
    uniform.metallic_factor = f;
}

void gearoenix::render::material::Material::set_roughness_factor(const core::Real f) noexcept
{
    uniform.roughness_factor = f;
}

void gearoenix::render::material::Material::set_color(
    const core::Real r, const core::Real g, const core::Real b,
    const core::sync::EndCaller<core::sync::EndCallerIgnore>& end) noexcept
{
    core::sync::EndCaller<texture::Texture2D> calltxt2d([end](std::shared_ptr<texture::Texture2D>) {});
    const std::shared_ptr<texture::Manager>& txtmgr = e->get_system_application()->get_asset_manager()->get_texture_manager();
    color = txtmgr->get_2d(math::Vec3(r, g, b), calltxt2d);
}

void gearoenix::render::material::Material::set_color(std::shared_ptr<texture::Texture2D> c) noexcept
{
	color = std::move(c);
}
