#include "rnd-mat-material.hpp"
#include "../engine/rnd-eng-engine.hpp"
#include "../../system/sys-app.hpp"
#include "../../core/asset/cr-asset-manager.hpp"
#include "../texture/rnd-txt-manager.hpp"
#include "../texture/rnd-txt-texture-2d.hpp"

gearoenix::render::material::Material::Material(const std::shared_ptr<engine::Engine>& e, const core::sync::EndCaller<core::sync::EndCallerIgnore>& end)
	: e(e)
{
	const std::shared_ptr<texture::Manager> &txtmgr = e->get_system_application()->get_asset_manager()->get_texture_manager();
	GXUNIMPLEMENTED;
}
