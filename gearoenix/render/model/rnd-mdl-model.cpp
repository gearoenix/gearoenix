#include "rnd-mdl-model.hpp"
#include "../../core/asset/cr-asset-manager.hpp"
#include "../../core/cr-static.hpp"
#include "../../physics/collider/phs-collider.hpp"
#include "../../system/stream/sys-stm-asset.hpp"
#include "../../system/stream/sys-stm-stream.hpp"
#include "../../system/sys-app.hpp"
#include "../camera/rnd-cmr-camera.hpp"
#include "../camera/rnd-cmr-orthographic.hpp"
#include "../light/rnd-lt-sun.hpp"
#include "../material/rnd-mat-depth.hpp"
#include "../material/rnd-mat-material.hpp"
#include "../mesh/rnd-msh-mesh.hpp"
#include "../rnd-engine.hpp"
#include "../scene/rnd-scn-scene.hpp"
#include "../widget/rnd-wdg-widget.hpp"
#include "rnd-mdl-dynamic.hpp"
#include "rnd-mdl-static.hpp"
#include <iostream>

gearoenix::render::model::Model::Model(core::Id my_id, RenderModel::Type t, system::stream::Stream* f, Engine* e, core::sync::EndCaller<core::sync::EndCallerIgnore> c)
    : core::asset::Asset(my_id, core::asset::Asset::AssetType::MODEL)
    , render_model_type(t)
    , render_engine(e)
{
    //std::lock_guard<std::mutex> lg(locker);
    m.read(f);
    occrdss.read(f);
    occloc.read(f);
    collider = std::shared_ptr<physics::collider::Collider>(physics::collider::Collider::read(f));
    std::vector<core::Id> model_children;
    f->read(model_children);
    std::vector<core::Id> mesh_ids;
    f->read(mesh_ids);
    occrds = GXMAX(occrdss[0], occrdss[1]);
    occrds = GXMAX(occrds, occrdss[2]);
    core::Count mesh_count = mesh_ids.size();
    std::map<core::Id, std::tuple<std::shared_ptr<material::Material>, std::shared_ptr<material::Depth>>> materials;
    for (core::Count i = 0; i < mesh_count; ++i) {
        std::shared_ptr<material::Material> mat(material::Material::read(f, e, c));
        if (shader::Shader::is_shadow_caster(mat->get_shader_id())) {
            std::shared_ptr<material::Depth> dp(new material::Depth(
                shader::Shader::get_shadow_caster_shader_id(mat->get_shader_id()),
                e, c));
            materials[mesh_ids[i]] = std::make_tuple(mat, dp);
            has_shadow_caster = true;
        } else {
            materials[mesh_ids[i]] = std::make_tuple(mat, nullptr);
        }
        has_transparent |= shader::Shader::is_transparent(mat->get_shader_id());
        needs_mvp |= mat->needs_mvp();
        needs_dbm |= mat->needs_dbm();
    }
    core::asset::Manager* astmgr = e->get_system_application()->get_asset_manager();
    system::stream::Asset* asset_file = astmgr->get_file();
    core::Count last_pos = asset_file->tell();
    for (core::Id mesh_id : mesh_ids) {
        const std::tuple<std::shared_ptr<material::Material>, std::shared_ptr<material::Depth>>& mat = materials[mesh_id];
        meshes[mesh_id] = std::make_tuple(astmgr->get_mesh(mesh_id, core::sync::EndCaller<mesh::Mesh>([c](std::shared_ptr<mesh::Mesh>) -> void {})), std::get<0>(mat), std::get<1>(mat));
    }
    for (core::Id model_id : model_children) {
        children[model_id] = astmgr->get_model(model_id, core::sync::EndCaller<model::Model>([c](std::shared_ptr<Model>) -> void {}));
    }
    asset_file->seek(last_pos);
}

gearoenix::render::model::Model* gearoenix::render::model::Model::read(core::Id my_id, system::stream::Stream* f, Engine* e, core::sync::EndCaller<core::sync::EndCallerIgnore> c)
{
    core::Id t;
    f->read(t);
    switch (t) {
    case RenderModel::WIDGET:
        return widget::Widget::read(my_id, f, e, c);
    case RenderModel::DYNAMIC:
        return new Dynamic(my_id, f, e, c);
    case RenderModel::STATIC:
        return new Static(my_id, f, e, c);
    default:
        GXUNEXPECTED;
    }
    return nullptr;
}

gearoenix::render::model::Model::~Model()
{
    //std::lock_guard<std::mutex> lg(locker);
}

void gearoenix::render::model::Model::commit(const scene::Scene* s)
{
    //std::lock_guard<std::mutex> lg(locker);
    if (s->get_current_camera()->is_transformed()) {
        commit_camera_changes(s);
    }
    if (s->get_sun()->get_camera()->is_transformed()) {
        commit_lights_changes(s);
    }
}

void gearoenix::render::model::Model::commit_camera_changes(const scene::Scene* s)
{
    const std::shared_ptr<camera::Camera>& cam = s->get_current_camera();
    is_in_camera = cam->in_sight(moccloc, occrds);
    if (is_in_camera) {
        if (needs_mvp) {
            mvp = cam->get_view_projection() * m;
        }
        if (has_transparent) {
            distcam = cam->get_distance(moccloc);
        }
        for (std::pair<const core::Id, std::tuple<std::shared_ptr<mesh::Mesh>, std::shared_ptr<material::Material>, std::shared_ptr<material::Depth>>>& mshmtr : meshes) {
            std::get<1>(mshmtr.second)->update(s, this);
        }
        // std::cout << "\n\nis in camera\n\n";
    }
}

void gearoenix::render::model::Model::commit_lights_changes(const scene::Scene* s)
{
    if (has_shadow_caster) {
        const light::Sun* sun = s->get_sun();
        const camera::Orthographic* suncam = sun->get_camera();
        is_in_sun = suncam->in_sight(moccloc, occrds);
        if (is_in_sun) {
            sunmvp = sun->get_camera()->get_view_projection() * m;
            if (needs_dbm) {
                dbm = sun->get_bias() * m;
            }
            for (std::pair<const core::Id, std::tuple<std::shared_ptr<mesh::Mesh>, std::shared_ptr<material::Material>, std::shared_ptr<material::Depth>>>& mshmtr : meshes) {
                std::shared_ptr<material::Depth>& dp = std::get<2>(mshmtr.second);
                if (nullptr != dp) {
                    dp->update(s, this);
                }
            }
        }
    }
}

void gearoenix::render::model::Model::draw(core::Id mesh_id, texture::Texture2D* shadow_texture)
{
    //std::lock_guard<std::mutex> lg(locker);
    if (is_in_camera) {
        std::tuple<std::shared_ptr<mesh::Mesh>, std::shared_ptr<material::Material>, std::shared_ptr<material::Depth>>& mshmtr = meshes[mesh_id];
        std::get<0>(mshmtr)->bind();
        std::get<1>(mshmtr)->bind(shadow_texture);
        std::get<0>(mshmtr)->draw();
    }
}

void gearoenix::render::model::Model::cast_shadow(core::Id mesh_id)
{
    //std::lock_guard<std::mutex> lg(locker);
    if (is_in_sun) {
        std::tuple<std::shared_ptr<mesh::Mesh>, std::shared_ptr<material::Material>, std::shared_ptr<material::Depth>>& mshmtr = meshes[mesh_id];
        std::get<0>(mshmtr)->bind();
        std::get<2>(mshmtr)->bind(nullptr);
        std::get<0>(mshmtr)->draw();
    }
}

const std::map<gearoenix::core::Id, std::shared_ptr<gearoenix::render::model::Model>>& gearoenix::render::model::Model::get_children() const
{
    return children;
}

const std::map<gearoenix::core::Id,
    std::tuple<std::shared_ptr<gearoenix::render::mesh::Mesh>,
        std::shared_ptr<gearoenix::render::material::Material>,
        std::shared_ptr<gearoenix::render::material::Depth>>>&
gearoenix::render::model::Model::get_meshes() const
{
    return meshes;
}

const gearoenix::math::Mat4x4& gearoenix::render::model::Model::get_m() const
{
    return m;
}

const gearoenix::math::Mat4x4& gearoenix::render::model::Model::get_mvp() const
{
    return mvp;
}

const gearoenix::math::Mat4x4& gearoenix::render::model::Model::get_sun_mvp() const
{
    return sunmvp;
}

gearoenix::render::model::Model::RenderModel::Type gearoenix::render::model::Model::get_render_model_type() const
{
    return render_model_type;
}

bool gearoenix::render::model::Model::hit(const math::Ray3& r, core::Real& d)
{
    //std::lock_guard<std::mutex> lg(locker);
    if (nullptr == collider)
        return false;
    return collider->hit(r, d);
}

bool gearoenix::render::model::Model::get_is_in_camera() const
{
    return is_in_camera;
}

const std::shared_ptr<gearoenix::physics::collider::Collider>& gearoenix::render::model::Model::get_collider() const
{
    return collider;
}

gearoenix::core::Real gearoenix::render::model::Model::get_distance_from_camera() const
{
    return distcam;
}
