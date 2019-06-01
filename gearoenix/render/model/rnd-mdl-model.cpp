#include "rnd-mdl-model.hpp"
#include "../../core/asset/cr-asset-manager.hpp"
#include "../../core/cr-static.hpp"
#include "../../physics/collider/phs-collider.hpp"
#include "../../system/stream/sys-stm-asset.hpp"
#include "../../system/stream/sys-stm-stream.hpp"
#include "../../system/sys-app.hpp"
#include "../buffer/rnd-buf-framed-uniform.hpp"
#include "../camera/rnd-cmr-camera.hpp"
#include "../camera/rnd-cmr-orthographic.hpp"
#include "../engine/rnd-eng-engine.hpp"
#include "../material/rnd-mat-material.hpp"
#include "../mesh/rnd-msh-manager.hpp"
#include "../mesh/rnd-msh-mesh.hpp"
#include "../pipeline/rnd-pip-manager.hpp"
#include "../scene/rnd-scn-scene.hpp"
#include "../widget/rnd-wdg-widget.hpp"
#include "rnd-mdl-manager.hpp"
#include "rnd-mdl-mesh.hpp"
#include "rnd-mdl-transformation.hpp"
#include <iostream>

gearoenix::render::model::Model::Model(
    const core::Id my_id,
    system::stream::Stream* const f,
    engine::Engine* const e,
    const core::sync::EndCaller<core::sync::EndCallerIgnore>& c) noexcept
    : core::asset::Asset(my_id, core::asset::Type::MODEL)
    , e(e)
    , uniform_buffers(std::make_shared<buffer::FramedUniform>(static_cast<unsigned int>(sizeof(Uniform)), e))
    , transformation(new Transformation(&uniform, &occlusion_sphere))
{
    uniform.m.read(f);
    const auto meshes_count = f->read<core::Count>();
    for (core::Count i = 0; i < meshes_count; ++i) {
        add_mesh(std::make_shared<Mesh>(f, e, c));
    }
    // TODO child
    /*std::vector<core::Id> children_ids;
	if (children_ids.size() > 0)
	{
		core::sync::EndCaller<Model> call([](const std::shared_ptr<Model>) {});
		const std::shared_ptr<Manager> &mdlmgr = astmgr->get_model_manager();
		f->read(children_ids);
		for (const core::Id mdlid : children_ids)
		{
			add_child(mdlmgr->get_gx3d(mdlid, call));
		}
	}*/
}

gearoenix::render::model::Model::Model(
    const core::Id my_id,
    engine::Engine* const e,
    const core::sync::EndCaller<core::sync::EndCallerIgnore>&) noexcept
    : core::asset::Asset(my_id, core::asset::Type::MODEL)
    , e(e)
    , uniform_buffers(std::make_shared<buffer::FramedUniform>(static_cast<unsigned int>(sizeof(Uniform)), e))
    , transformation(new Transformation(&uniform, &occlusion_sphere))
{
}

void gearoenix::render::model::Model::update_uniform() noexcept
{
    uniform_buffers->update(&uniform);
    for (const auto& msh : meshes)
        msh.second->update_uniform();
    for (const auto& ch : children)
        ch.second->update_uniform();
}

void gearoenix::render::model::Model::add_mesh(const std::shared_ptr<Mesh>& m) noexcept
{
    meshes[m->get_mesh()->get_asset_id()] = m;
    occlusion_sphere.insert(m->get_mesh()->get_radius());
}

void gearoenix::render::model::Model::add_child(const std::shared_ptr<Model>&) noexcept
{
}

bool gearoenix::render::model::Model::is_enabled() const noexcept
{
    return enabled;
}

void gearoenix::render::model::Model::enable() noexcept
{
    enabled = true;
}

void gearoenix::render::model::Model::disable() noexcept
{
    enabled = false;
}

//    //std::lock_guard<std::mutex> lg(locker);
//    m.read(f);
//    occrdss.read(f);
//    occloc.read(f);
//    collider = std::shared_ptr<physics::collider::Collider>(physics::collider::Collider::read(f));
//    std::vector<core::Id> model_children;
//    f->read(model_children);
//    std::vector<core::Id> mesh_ids;
//    f->read(mesh_ids);
//    occrds = GX_MAX(occrdss[0], occrdss[1]);
//    occrds = GX_MAX(occrds, occrdss[2]);
//    core::Count mesh_count = mesh_ids.size();
//    std::map<core::Id, std::tuple<std::shared_ptr<material::Material>, std::shared_ptr<material::Depth>>> materials;
//    for (core::Count i = 0; i < mesh_count; ++i) {
//        std::shared_ptr<material::Material> mat(material::Material::read(f, e, c));
//        if (shader::Shader::is_shadow_caster(mat->get_shader_id())) {
//            std::shared_ptr<material::Depth> dp(new material::Depth(
//                shader::Shader::get_shadow_caster_shader_id(mat->get_shader_id()),
//                e, c));
//            materials[mesh_ids[i]] = std::make_tuple(mat, dp);
//            has_shadow_caster = true;
//        } else {
//            materials[mesh_ids[i]] = std::make_tuple(mat, nullptr);
//        }
//        has_transparent |= shader::Shader::is_transparent(mat->get_shader_id());
//        needs_mvp |= mat->needs_mvp();
//        needs_dbm |= mat->needs_dbm();
//    }
//    core::asset::Manager* astmgr = e->get_system_application()->get_asset_manager();
//    system::stream::Asset* asset_file = astmgr->get_file();
//    core::Count last_pos = asset_file->tell();
//    for (core::Id mesh_id : mesh_ids) {
//        const std::tuple<std::shared_ptr<material::Material>, std::shared_ptr<material::Depth>>& mat = materials[mesh_id];
//        meshes[mesh_id] = std::make_tuple(astmgr->get_mesh(mesh_id, core::sync::EndCaller<mesh::Mesh>([c](std::shared_ptr<mesh::Mesh>) -> void {})), std::get<0>(mat), std::get<1>(mat));
//    }
//    for (core::Id model_id : model_children) {
//        children[model_id] = astmgr->get_model(model_id, core::sync::EndCaller<model::Model>([c](std::shared_ptr<Model>) -> void {}));
//    }
//    asset_file->seek(last_pos);
//}

//gearoenix::render::model::Model* gearoenix::render::model::Model::read(core::Id my_id, system::stream::Stream* f, Engine* e, core::sync::EndCaller<core::sync::EndCallerIgnore> c)
//{
//    core::Id t;
//    f->read(t);
//    switch (t) {
//    case RenderModel::WIDGET:
//        return widget::Widget::read(my_id, f, e, c);
//    case RenderModel::DYNAMIC:
//        return new Dynamic(my_id, f, e, c);
//    case RenderModel::STATIC:
//        return new Static(my_id, f, e, c);
//    default:
//        GXUNEXPECTED;
//    }
//    return nullptr;
//}

const std::map<gearoenix::core::Id, std::shared_ptr<gearoenix::render::model::Model>>& gearoenix::render::model::Model::get_children() const noexcept
{
    return children;
}

const std::map<gearoenix::core::Id, std::shared_ptr<gearoenix::render::model::Mesh>>& gearoenix::render::model::Model::get_meshes() const noexcept
{
    return meshes;
}

const std::shared_ptr<gearoenix::physics::collider::Collider>& gearoenix::render::model::Model::get_collider() const noexcept
{
    return collider;
}

const std::shared_ptr<gearoenix::render::buffer::FramedUniform>& gearoenix::render::model::Model::get_uniform_buffers() const noexcept
{
    return uniform_buffers;
}

const std::shared_ptr<gearoenix::render::model::Transformation>& gearoenix::render::model::Model::get_transformation() const noexcept
{
    return transformation;
}

const gearoenix::math::Sphere& gearoenix::render::model::Model::get_occlusion_sphere() const noexcept
{
    return occlusion_sphere;
}

const gearoenix::math::Mat4x4& gearoenix::render::model::Model::get_model_matrix() const noexcept
{
    return uniform.m;
}
