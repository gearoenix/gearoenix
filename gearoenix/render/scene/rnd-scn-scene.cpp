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
#include "../buffer/rnd-buf-manager.hpp"
#include "../buffer/rnd-buf-uniform.hpp"
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
#include "../pipeline/rnd-pip-resource.hpp"
#include "../engine/rnd-eng-engine.hpp"
#include "../shader/rnd-shd-shader.hpp"
#include "../skybox/rnd-sky-skybox.hpp"
#include "rnd-scn-ui.hpp"

gearoenix::render::scene::Scene::Scene(
	const core::Id my_id,
	const std::shared_ptr<system::stream::Stream>& f,
	const std::shared_ptr<engine::Engine> &e,
	const core::sync::EndCaller<core::sync::EndCallerIgnore> &c)
	: core::asset::Asset(my_id, core::asset::Type::SCENE)
	, e(e)
	, scene_type_id(Type::GAME)
	, pipeline_resource(e->get_pipeline_manager()->create_resource({}))
{
	for (unsigned int i = 0; i < GX_FRAMES_COUNT; ++i) {
		uniform_buffers[i] = std::shared_ptr<buffer::Uniform>(e->get_buffer_manager()->create_uniform(sizeof(Uniform)));
	}
	const std::shared_ptr<core::asset::Manager> &astmgr = e->get_system_application()->get_asset_manager();
#define GXHELPER(x, y)                                                        \
	{                                                                         \
		const std::shared_ptr<x::Manager> &mgr = astmgr->get_##x##_manager(); \
		const core::sync::EndCaller<x::y> call(c);                            \
		std::vector<core::Id> ids;                                            \
		f->read(ids);                                                         \
		for (const core::Id id : ids) add_##x(mgr->get_gx3d(id, call));            \
	}
	GXHELPER(camera, Camera);
	GXHELPER(audio, Audio);
	GXHELPER(light, Light);
	GXHELPER(model, Model);
	if (f->read_bool()) {
		core::Id skybox_id = 0;
		f->read(skybox_id);
	}
	GXHELPER(constraint, Constraint);
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
	, pipeline_resource(e->get_pipeline_manager()->create_resource({}))
{
	for (unsigned int i = 0; i < GX_FRAMES_COUNT; ++i) {
		uniform_buffers[i] = std::shared_ptr<buffer::Uniform>(e->get_buffer_manager()->create_uniform(sizeof(Uniform)));
	}
}

gearoenix::render::scene::Scene::~Scene()
{}

gearoenix::render::scene::Scene* gearoenix::render::scene::Scene::read_gx3d(
	const core::Id my_id,
	const std::shared_ptr<system::stream::Stream>& f,
	const std::shared_ptr<engine::Engine> &e,
	const core::sync::EndCaller<core::sync::EndCallerIgnore> &c)
{
    const Type::Id t = f->read<Type::Id>();
    switch (t) {
    case Type::GAME:
		GXUNIMPLEMENTED;
    case Type::UI:
		GXUNIMPLEMENTED;
    default:
        GXUNEXPECTED;
    }
}

void gearoenix::render::scene::Scene::enable_rendering() {
	renderable = true;
}

void gearoenix::render::scene::Scene::disable_rendering() {
	renderable = false;
}

bool gearoenix::render::scene::Scene::is_renderable() const {
	return renderable;
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
		return nullptr;
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
		return nullptr;
	}
	return find->second;
}