#include "rnd-scn-scene.hpp"
#include "../../audio/au-audio.hpp"
#include "../../audio/au-manager.hpp"
#include "../../core/asset/cr-asset-manager.hpp"
#include "../../core/sync/cr-sync-end-caller.hpp"
#include "../../physics/body/phs-bd-body.hpp"
#include "../../physics/constraint/phs-cns-constraint.hpp"
#include "../../physics/constraint/phs-cns-manager.hpp"
#include "../../system/stream/sys-stm-stream.hpp"
#include "../../system/sys-app.hpp"
#include "../buffer/rnd-buf-framed-uniform.hpp"
#include "../camera/rnd-cmr-camera.hpp"
#include "../camera/rnd-cmr-manager.hpp"
#include "../camera/rnd-cmr-orthographic.hpp"
#include "../light/rnd-lt-light.hpp"
#include "../light/rnd-lt-sun.hpp"
#include "../light/rnd-lt-manager.hpp"
#include "../material/rnd-mat-material.hpp"
#include "../mesh/rnd-msh-mesh.hpp"
#include "../model/rnd-mdl-model.hpp"
#include "../model/rnd-mdl-manager.hpp"
#include "../pipeline/rnd-pip-manager.hpp"
#include "../engine/rnd-eng-engine.hpp"
#include "../shader/rnd-shd-shader.hpp"
#include "../skybox/rnd-sky-skybox.hpp"
#include "rnd-scn-ui.hpp"

const std::shared_ptr<gearoenix::render::camera::Camera>          gearoenix::render::scene::Scene::null_camera = nullptr;
const std::shared_ptr<gearoenix::audio::Audio>                    gearoenix::render::scene::Scene::null_audio = nullptr;
const std::shared_ptr<gearoenix::render::light::Light>            gearoenix::render::scene::Scene::null_light = nullptr;
const std::shared_ptr<gearoenix::render::model::Model>            gearoenix::render::scene::Scene::null_model = nullptr;
const std::shared_ptr<gearoenix::physics::constraint::Constraint> gearoenix::render::scene::Scene::null_constraint = nullptr;

gearoenix::render::scene::Scene::Scene(
	const core::Id my_id,
	const std::shared_ptr<system::stream::Stream>& f,
	const std::shared_ptr<engine::Engine> &e,
	const core::sync::EndCaller<core::sync::EndCallerIgnore> &c)
	: core::asset::Asset(my_id, core::asset::Type::SCENE)
	, e(e)
	, scene_type_id(Type::GAME)
	, uniform_buffers(std::make_shared<buffer::FramedUniform>(sizeof(Uniform), e))
{
	const std::shared_ptr<core::asset::Manager> &astmgr = e->get_system_application()->get_asset_manager();
#define GXHELPER(x, n, cls)                                                   \
	{                                                                         \
        const std::shared_ptr<n::Manager> &mgr = astmgr->get_##x##_manager(); \
		std::vector<core::Id> ids;                                            \
		f->read(ids);                                                         \
		if(ids.size() > 0) {                                                  \
			core::sync::EndCaller<n::cls> call([c](std::shared_ptr<n::cls>){});                            \
			for (const core::Id id : ids) add_##x(mgr->get_gx3d(id, call));   \
		}                                                                     \
	}

	GXHELPER(camera, camera, Camera);
	GXHELPER(audio, audio, Audio);
	GXHELPER(light, light, Light);
	GXHELPER(model, model, Model);
	if (f->read_bool()) {
		core::Id skybox_id = 0;
		f->read(skybox_id);
	}
	GXHELPER(constraint, physics::constraint, Constraint);

#undef GXHELPER

	if (f->read_bool()) {
		GXUNIMPLEMENTED;
	}
}

gearoenix::render::scene::Scene::Scene(
	const std::shared_ptr<engine::Engine> &e,
	const core::sync::EndCaller<core::sync::EndCallerIgnore> &c)
	: core::asset::Asset(e->get_system_application()->get_asset_manager()->create_id(), core::asset::Type::SCENE)
	, e(e)
	, scene_type_id(Type::GAME)
	, uniform_buffers(std::make_shared<buffer::FramedUniform>(sizeof(Uniform), e))
{}

gearoenix::render::scene::Scene::~Scene()
{}

void gearoenix::render::scene::Scene::enable_rendering() {
	renderable = true;
}

void gearoenix::render::scene::Scene::disable_rendering() {
	renderable = false;
}

bool gearoenix::render::scene::Scene::is_renderable() const {
	return renderable;
}

void gearoenix::render::scene::Scene::add_camera(const std::shared_ptr<camera::Camera>& o)
{
	const core::Id oid = o->get_asset_id();
#ifdef GX_DEBUG_MODE
	if (cameras.find(oid) != cameras.end()) {
		GXLOGE("Error overriding of a camera with same Id: " << oid);
	}
#endif
	cameras[oid] = o;
}

const std::map<gearoenix::core::Id, std::shared_ptr<gearoenix::render::camera::Camera>>& gearoenix::render::scene::Scene::get_cameras() const
{
	return cameras;
}

void gearoenix::render::scene::Scene::add_audio(const std::shared_ptr<audio::Audio>& o)
{
	const core::Id oid = o->get_asset_id();
#ifdef GX_DEBUG_MODE
	if (audios.find(oid) != audios.end()) {
		GXLOGE("Error overriding of a audio with same Id: " << oid);
	}
#endif
	audios[oid] = o;
}

void gearoenix::render::scene::Scene::add_light(const std::shared_ptr<light::Light>& o)
{
	const core::Id oid = o->get_asset_id();
#ifdef GX_DEBUG_MODE
	if (lights.find(oid) != lights.end()) {
		GXLOGE("Error overriding of a light with same Id: " << oid);
	}
#endif
	lights[oid] = o;
}

const std::map<gearoenix::core::Id, std::shared_ptr<gearoenix::render::light::Light>>& gearoenix::render::scene::Scene::get_lights() const
{
	return lights;
}

void gearoenix::render::scene::Scene::add_model(const std::shared_ptr<model::Model>& m)
{
	const core::Id mid = m->get_asset_id();
#ifdef GX_DEBUG_MODE
	if (models.find(mid) != models.end()) {
		GXLOGE("Error overriding of a model with same Id: " << mid);
	}
#endif // GX_DEBUG_MODE
	models[mid] = m;
}

const std::shared_ptr<gearoenix::render::model::Model> &gearoenix::render::scene::Scene::get_model(const core::Id model_id) const {
	const auto &find = models.find(model_id);
	if (models.end() == find) {
		return null_model;
	}
	return find->second;
}

const std::map<gearoenix::core::Id, std::shared_ptr<gearoenix::render::model::Model>>& gearoenix::render::scene::Scene::get_models() const
{
	return models;
}

void gearoenix::render::scene::Scene::add_constraint(const std::shared_ptr<physics::constraint::Constraint>& c)
{
	const core::Id cid = c->get_asset_id();
#ifdef GX_DEBUG_MODE
	if (constraints.find(cid) != constraints.end()) {
		GXLOGE("Error overriding of a constraint with same Id: " << cid);
	}
#endif // GX_DEBUG_MODE
	constraints[cid] = c;
}

const std::shared_ptr<gearoenix::physics::constraint::Constraint>& gearoenix::render::scene::Scene::get_constraint(const core::Id constraint_id) const
{
	const auto &find = constraints.find(constraint_id);
	if (constraints.end() == find) {
		return null_constraint;
	}
	return find->second;
}

const std::shared_ptr<gearoenix::render::buffer::FramedUniform>& gearoenix::render::scene::Scene::get_uniform_buffers() const
{
	return uniform_buffers;
}
