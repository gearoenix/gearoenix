#include "rnd-scn-scene.hpp"
#include "../../audio/au-audio.hpp"
#include "../../core/asset/cr-asset-manager.hpp"
#include "../../core/sync/cr-sync-end-caller.hpp"
#include "../../physics/body/phs-bd-body.hpp"
#include "../../physics/constraint/phs-cns-constraint.hpp"
#include "../../system/stream/sys-stm-stream.hpp"
#include "../../system/sys-app.hpp"
#include "../camera/rnd-cmr-camera.hpp"
#include "../camera/rnd-cmr-orthographic.hpp"
#include "../light/rnd-lt-light.hpp"
#include "../light/rnd-lt-sun.hpp"
#include "../material/rnd-mat-depth.hpp"
#include "../material/rnd-mat-material.hpp"
#include "../mesh/rnd-msh-mesh.hpp"
#include "../model/rnd-mdl-model.hpp"
#include "../rnd-engine.hpp"
#include "../shader/rnd-shd-shader.hpp"
#include "../skybox/rnd-sky-skybox.hpp"
#include "rnd-scn-ui.hpp"

void gearoenix::render::scene::Scene::add_model(const std::shared_ptr<model::Model>& m)
{
    all_models[m->get_asset_id()] = m;
    const std::map<core::Id, std::shared_ptr<model::Model>>& children = m->get_children();
    for (const std::pair<core::Id, std::shared_ptr<model::Model>>& child : children) {
        add_model(child.second);
    }
    const std::map<core::Id, std::tuple<std::shared_ptr<mesh::Mesh>, std::shared_ptr<material::Material>, std::shared_ptr<material::Depth>>>& meshes = m->get_meshes();
    for (const std::pair<core::Id, std::tuple<std::shared_ptr<mesh::Mesh>, std::shared_ptr<material::Material>, std::shared_ptr<material::Depth>>>& mp : meshes) {
        core::Id msh = mp.first;
        const std::shared_ptr<material::Material>& mtr = std::get<1>(mp.second);
        core::Id shdid = mtr->get_shader_id();
        if (shader::Shader::is_shadow_caster(shdid)) {
            core::Id shdcstid = shader::Shader::get_shadow_caster_shader_id(shdid);
            shadow_caster_models[shdcstid][m->get_asset_id()].insert(msh);
        }
        if (shader::Shader::is_transparent(shdid)) {
            transparent_models[m->get_asset_id()].insert(msh);
        } else {
            opaque_models[shdid][m->get_asset_id()].insert(msh);
        }
    }
}

gearoenix::render::scene::Scene::Scene(core::Id my_id, SceneType t, system::stream::Stream* f, Engine* e, core::sync::EndCaller<core::sync::EndCallerIgnore> c)
    : core::asset::Asset(my_id, core::asset::Asset::AssetType::SCENE)
    , scene_type(t)
    , render_engine(e)
{
    ambient_light.read(f);
    core::asset::Manager* amgr = e->get_system_application()->get_asset_manager();
    std::vector<core::Id> camera_ids;
    f->read(camera_ids);
    cam_id = camera_ids[0];
    std::vector<core::Id> audio_ids;
    f->read(audio_ids);
    std::vector<core::Id> light_ids;
    f->read(light_ids);
    sun_id = light_ids[0];
    std::vector<core::Id> model_ids;
    f->read(model_ids);
    const bool has_skybox = f->read_bool();
    const core::Id skybox_id = has_skybox ? f->read<core::Id>() : 0;
    std::vector<core::Id> constraint_ids;
    f->read(constraint_ids);
    const bool has_griding = f->read_bool();
    const core::Real up_bound = has_griding ? f->read<core::Real>() : 0.0f;
    const core::Real down_bound = has_griding ? f->read<core::Real>() : 0.0f;
    const core::Real left_bound = has_griding ? f->read<core::Real>() : 0.0f;
    const core::Real right_bound = has_griding ? f->read<core::Real>() : 0.0f;
    const core::Real front_bound = has_griding ? f->read<core::Real>() : 0.0f;
    const core::Real back_bound = has_griding ? f->read<core::Real>() : 0.0f;
    const std::uint16_t grid_x_count = has_griding ? f->read<std::uint16_t>() : 0;
    const std::uint16_t grid_y_count = has_griding ? f->read<std::uint16_t>() : 0;
    const std::uint16_t grid_z_count = has_griding ? f->read<std::uint16_t>() : 0;
    for (const core::Id i : camera_ids)
        cameras[i] = amgr->get_camera(i);
    for (const core::Id i : audio_ids)
        audios[i] = amgr->get_audio(i);
    for (const core::Id i : light_ids)
        lights[i] = amgr->get_light(i);
    for (const core::Id i : model_ids) {
        add_model(i, c);
    }
    for (const core::Id cons_id : constraint_ids) {
        add_constraint(amgr->get_constriants(cons_id, core::sync::EndCaller<physics::constraint::Constraint>([c](std::shared_ptr<physics::constraint::Constraint>) -> void {})));
    }
    skybox = has_skybox ? amgr->get_skybox(skybox_id, core::sync::EndCaller<skybox::Skybox>([c](std::shared_ptr<skybox::Skybox>) -> void {})) : nullptr;
}

gearoenix::render::scene::Scene::~Scene()
{
    cameras.clear();
    audios.clear();
    lights.clear();
    root_models.clear();
}

gearoenix::render::scene::Scene* gearoenix::render::scene::Scene::read(
    core::Id my_id,
    system::stream::Stream* f, Engine* e, core::sync::EndCaller<core::sync::EndCallerIgnore> c)
{
    core::Id t;
    f->read(t);
    switch (t) {
    case SceneType::GAME:
        return new Scene(my_id, SceneType::GAME, f, e, c);
    case SceneType::UI:
        return new Ui(my_id, f, e, c);
    default:
        UNEXPECTED;
    }
}

const std::map<gearoenix::core::Id, std::weak_ptr<gearoenix::render::model::Model>>& gearoenix::render::scene::Scene::get_all_models() const
{
    return all_models;
}

const std::map<gearoenix::core::Id, std::shared_ptr<gearoenix::physics::constraint::Constraint>>& gearoenix::render::scene::Scene::get_all_root_constraints() const
{
    return root_constraints;
}

void gearoenix::render::scene::Scene::cast_shadow()
{
    if (renderable) {
        for (std::pair<const core::Id, std::map<core::Id, std::set<core::Id>>>& pshd : shadow_caster_models) {
            for (std::pair<const core::Id, std::set<core::Id>>& pmdl : pshd.second) {
                for (const core::Id mshid : pmdl.second) {
                    if (std::shared_ptr<model::Model> mdl = all_models[pmdl.first].lock()) {
                        mdl->cast_shadow(mshid);
                    }
                }
            }
        }
    }
}

void gearoenix::render::scene::Scene::draw_sky()
{
    if (nullptr == skybox)
        return;
    skybox->update(this);
    skybox->draw();
}

void gearoenix::render::scene::Scene::draw(texture::Texture2D* shadow_texture)
{
    if (!renderable)
        return;
    for (std::pair<const core::Id, std::map<core::Id, std::set<core::Id>>>& pshd : opaque_models) {
        for (std::pair<const core::Id, std::set<core::Id>>& pmdl : pshd.second) {
            if (std::shared_ptr<model::Model> mdl = all_models[pmdl.first].lock()) {
                for (const core::Id mshid : pmdl.second) {
                    mdl->draw(mshid, shadow_texture);
                }
            }
        }
    }
    std::map<core::Real, std::map<core::Id, std::set<core::Id>>, std::greater<core::Real>> sorted_models;
    for (std::pair<const core::Id, std::set<core::Id>>& pmdl : transparent_models) {
        if (std::shared_ptr<model::Model> mdl = all_models[pmdl.first].lock()) {
            if (mdl->get_is_in_camera()) {
                const core::Real camdis = mdl->get_distance_from_camera();
                if (camdis > 0.0f) {
                    sorted_models[camdis][pmdl.first] = pmdl.second;
                }
            }
        }
    }
    for (std::pair<const core::Real, std::map<core::Id, std::set<core::Id>>>& pdis : sorted_models) {
        for (std::pair<const core::Id, std::set<core::Id>>& pmdl : pdis.second) {
            if (std::shared_ptr<model::Model> mdl = all_models[pmdl.first].lock()) {
                for (const core::Id mshid : pmdl.second) {
                    mdl->draw(mshid, shadow_texture);
                }
            }
        }
    }
}

const std::shared_ptr<gearoenix::render::camera::Camera>& gearoenix::render::scene::Scene::get_current_camera() const
{
    //    return const_cast<camera::Camera*>(reinterpret_cast<const camera::Camera*>(reinterpret_cast<light::Sun*>(lights[0].get())->get_camera()));
    auto search = cameras.find(cam_id);
#ifdef DEBUG_MODE
    if (search == cameras.end())
        UNEXPECTED;
#endif
    return search->second;
}

const gearoenix::math::Vec3& gearoenix::render::scene::Scene::get_ambient_light() const
{
    return ambient_light;
}

bool gearoenix::render::scene::Scene::get_ambient_light_changed() const
{
    return ambient_light_changed;
}

const gearoenix::render::light::Sun* gearoenix::render::scene::Scene::get_sun() const
{
    const std::map<core::Id, std::shared_ptr<light::Light>>::const_iterator c = lights.find(sun_id);
    if (c == lights.end())
        UNEXPECTED;
    std::shared_ptr<light::Sun> sun = std::static_pointer_cast<light::Sun>(c->second);
    return sun.get();
}

void gearoenix::render::scene::Scene::set_renderable(bool b)
{
    renderable = b;
}

void gearoenix::render::scene::Scene::clean()
{
    if (all_models_needs_cleaning) {
        all_models_needs_cleaning = false;
        for (auto iter = all_models.begin(); iter != all_models.end(); ++iter) {
            if (iter->second.expired()) {
                iter = all_models.erase(iter);
            }
        }
    }
}

void gearoenix::render::scene::Scene::on_event(core::event::Event& e)
{
    for (const std::pair<core::Id, std::shared_ptr<camera::Camera>>& id_cam : cameras) {
        const std::shared_ptr<camera::Camera>& cam = id_cam.second;
        cam->on_event(e);
    }
    for (const std::pair<core::Id, std::shared_ptr<physics::constraint::Constraint>>& con : root_constraints) {
        con.second->on_event(e);
    }
}

bool gearoenix::render::scene::Scene::is_renderable() const
{
    return renderable;
}

void gearoenix::render::scene::Scene::add_mesh(core::Id mesh_id, core::Id model_id, std::shared_ptr<material::Material> mat, std::shared_ptr<material::Material> dp)
{
    if (nullptr != dp) {
        shadow_caster_models[dp->get_shader_id()][model_id].insert(mesh_id);
    }
    const core::Id mat_id = mat->get_shader_id();
    if (shader::Shader::is_transparent(mat_id)) {
        transparent_models[model_id].insert(mesh_id);
    } else {
        opaque_models[mat_id][model_id].insert(mesh_id);
    }
}

void gearoenix::render::scene::Scene::add_model(core::Id model_id, core::sync::EndCaller<core::sync::EndCallerIgnore> c)
{
    core::asset::Manager* amgr = render_engine->get_system_application()->get_asset_manager();
    amgr->get_model(
        model_id,
        core::sync::EndCaller<model::Model>(
            [c, model_id, this](std::shared_ptr<model::Model> mdl) -> void {
#ifdef DEBUG_MODE
                if (root_models.find(model_id) != root_models.end())
                    UNEXPECTED;
#endif
                root_models[model_id] = mdl;
                add_model(mdl);
            }));
}

std::weak_ptr<gearoenix::render::model::Model> gearoenix::render::scene::Scene::get_model(core::Id model_id)
{
    std::map<core::Id, std::weak_ptr<model::Model>>::iterator search = all_models.find(model_id);
    if (all_models.end() == search)
        return std::shared_ptr<model::Model>(nullptr);
    return search->second;
}

void gearoenix::render::scene::Scene::add_constraint(const std::shared_ptr<physics::constraint::Constraint>& cns)
{
#ifdef DEBUG_MODE
    std::map<core::Id, std::shared_ptr<physics::constraint::Constraint>>::iterator search = root_constraints.find(cns->get_asset_id());
    if (root_constraints.end() != search)
        UNEXPECTED;
#endif
    root_constraints[cns->get_asset_id()] = cns;
    const std::vector<std::pair<core::Id, std::shared_ptr<model::Model>>> models = cns->get_all_models();
    for (const std::pair<const core::Id, const std::shared_ptr<model::Model>>& model : models) {
        add_model(model.second);
    }
    const std::vector<std::shared_ptr<physics::body::Body>> bodies = cns->get_all_bodies();
    for (const std::shared_ptr<physics::body::Body>& b : bodies) {
        all_bodies[b->get_asset_id()] = b;
    }
}

void gearoenix::render::scene::Scene::add_body(const std::shared_ptr<physics::body::Body>& b)
{
#ifdef DEBUG_MODE
    std::map<core::Id, std::shared_ptr<physics::body::Body>>::iterator search = root_bodies.find(b->get_asset_id());
    if (root_bodies.end() != search)
        UNEXPECTED;
#endif
    root_bodies[b->get_asset_id()] = b;
}
