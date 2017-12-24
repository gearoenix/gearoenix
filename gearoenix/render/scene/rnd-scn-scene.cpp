#include "rnd-scn-scene.hpp"
#include "../../audio/au-audio.hpp"
#include "../../core/asset/cr-asset-manager.hpp"
#include "../../core/cr-end-caller.hpp"
#include "../../system/sys-app.hpp"
#include "../../system/sys-file.hpp"
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

void gearoenix::render::scene::Scene::add_model(core::Id id, std::shared_ptr<model::Model> m)
{
    all_models[id] = m;
    const std::map<core::Id, std::shared_ptr<model::Model>>& children = m->get_children();
    for (const std::pair<core::Id, std::shared_ptr<model::Model>>& child : children) {
        add_model(child.first, child.second);
    }
    const std::map<core::Id, std::tuple<std::shared_ptr<mesh::Mesh>, std::shared_ptr<material::Material>, std::shared_ptr<material::Depth>>>& meshes = m->get_meshes();
    for (const std::pair<core::Id, std::tuple<std::shared_ptr<mesh::Mesh>, std::shared_ptr<material::Material>, std::shared_ptr<material::Depth>>>& mp : meshes) {
        core::Id msh = mp.first;
        const std::shared_ptr<material::Material>& mtr = std::get<1>(mp.second);
        core::Id shdid = mtr->get_shader_id();
        if (shader::Shader::is_shadow_caster(shdid)) {
            core::Id shdcstid = shader::Shader::get_shadow_caster_shader_id(shdid);
            shadow_caster_models[shdcstid][id].insert(msh);
        }
        if (shader::Shader::is_transparent(shdid)) {
            transparent_models[shdid][id].insert(msh);
        } else {
            opaque_models[shdid][id].insert(msh);
        }
    }
}

gearoenix::render::scene::Scene::Scene(system::File* f, Engine* e, std::shared_ptr<core::EndCaller> c)
//: e(e)
{
    core::asset::Manager* amgr = e->get_system_application()->get_asset_manager();
    std::vector<core::Id> camera_ids;
    f->read(camera_ids);
    cameras.resize(camera_ids.size());
    std::vector<core::Id> audio_ids;
    f->read(audio_ids);
    audios.resize(audio_ids.size());
    std::vector<core::Id> light_ids;
    f->read(light_ids);
    lights.resize(light_ids.size());
    std::vector<core::Id> model_ids;
    f->read(model_ids);
    ambient_light.read(f);
    for (size_t i = 0; i < camera_ids.size(); ++i)
        cameras[i] = amgr->get_camera(camera_ids[i]);
    for (size_t i = 0; i < audio_ids.size(); ++i)
        audios[i] = amgr->get_audio(audio_ids[i]);
    for (size_t i = 0; i < light_ids.size(); ++i)
        lights[i] = amgr->get_light(light_ids[i]);
    for (size_t i = 0; i < model_ids.size(); ++i) {
        root_models[model_ids[i]] = amgr->get_model(model_ids[i], c);
        add_model(model_ids[i], root_models[i]);
    }
}

gearoenix::render::scene::Scene::~Scene()
{
    cameras.clear();
    audios.clear();
    lights.clear();
    root_models.clear();
}

gearoenix::render::scene::Scene* gearoenix::render::scene::Scene::read(
    system::File* f, Engine* e, std::shared_ptr<core::EndCaller> c)
{
    return new Scene(f, e, c);
}

const std::map<gearoenix::core::Id, std::weak_ptr<gearoenix::render::model::Model>>& gearoenix::render::scene::Scene::get_all_models() const
{
    return all_models;
}

void gearoenix::render::scene::Scene::commit()
{
    for (std::pair<const core::Id, std::shared_ptr<model::Model>>& p : root_models) {
        std::shared_ptr<model::Model>& m = p.second;
        m->commit(this);
    }
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

void gearoenix::render::scene::Scene::draw(texture::Texture2D* shadow_texture)
{
    if (renderable) {
        for (std::pair<const core::Id, std::map<core::Id, std::set<core::Id>>>& pshd : opaque_models) {
            for (std::pair<const core::Id, std::set<core::Id>>& pmdl : pshd.second) {
                for (const core::Id mshid : pmdl.second) {
                    if (std::shared_ptr<model::Model> mdl = all_models[pmdl.first].lock()) {
                        mdl->draw(mshid, shadow_texture);
                    }
                }
            }
        }
        //GXLOGE("TODO it needs to render transparent object with order of their depth.");
    }
}

const gearoenix::render::camera::Camera* gearoenix::render::scene::Scene::get_current_camera() const
{
    //return reinterpret_cast<const light::Sun*>(lights[0].get())->get_camera();
    return cameras[curcam].get();
}

gearoenix::render::camera::Camera* gearoenix::render::scene::Scene::get_current_camera()
{
    //return const_cast<camera::Camera*>(reinterpret_cast<const camera::Camera*>(reinterpret_cast<light::Sun*>(lights[0].get())->get_camera()));
    return cameras[curcam].get();
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
    return reinterpret_cast<light::Sun*>(lights[0].get());
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
