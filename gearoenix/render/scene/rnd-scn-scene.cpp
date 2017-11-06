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
#include "../material/rnd-mat-material.hpp"
#include "../mesh/rnd-msh-mesh.hpp"
#include "../model/rnd-mdl-model.hpp"
#include "../rnd-engine.hpp"
#include "../shader/rnd-shd-shader.hpp"

void gearoenix::render::scene::Scene::add_model(core::Id id, model::Model* m)
{
    mesh::Mesh* draw_mesh = m->get_draw_mesh();
    if (nullptr != draw_mesh) {
        material::Material* mat = draw_mesh->get_material();
        if (mat->is_shadow_caster()) {
            core::Id scid = mat->get_shadow_caster_id();
            shadow_caster_models[scid].push_back(m);
        }
        core::Id shid = mat->get_shader_id();
        if (mat->is_transparent()) {
            transparent_models[shid].push_back(m);
        } else {
            opaque_models[shid].push_back(m);
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
    models.resize(model_ids.size());
    ambient_light.read(f);
    for (size_t i = 0; i < camera_ids.size(); ++i)
        cameras[i] = amgr->get_camera(camera_ids[i]);
    for (size_t i = 0; i < audio_ids.size(); ++i)
        audios[i] = amgr->get_audio(audio_ids[i]);
    for (size_t i = 0; i < light_ids.size(); ++i)
        lights[i] = amgr->get_light(light_ids[i]);
    for (size_t i = 0; i < model_ids.size(); ++i) {
        models[i] = amgr->get_model(model_ids[i], c);
        add_model(model_ids[i], models[i].get());
        const std::vector<model::Model*>& cm = m->get_children();
    }
}

gearoenix::render::scene::Scene::~Scene()
{
    cameras.clear();
    audios.clear();
    lights.clear();
    models.clear();
}

gearoenix::render::scene::Scene* gearoenix::render::scene::Scene::read(
    system::File* f, Engine* e, std::shared_ptr<core::EndCaller> c)
{
    return new Scene(f, e, c);
}

void gearoenix::render::scene::Scene::commit()
{
    for (std::shared_ptr<model::Model>& m : models) {
        m->commit(this);
    }
}

void gearoenix::render::scene::Scene::cast_shadow()
{
}

void gearoenix::render::scene::Scene::draw(texture::Texture2D* shadow_texture)
{
    if (renderable) {
        for (std::shared_ptr<model::Model>& m : models) {
            m->draw(shadow_texture);
        }
    }
}

const gearoenix::render::camera::Camera* gearoenix::render::scene::Scene::get_current_camera() const
{
    //    return reinterpret_cast<const light::Sun*>(lights[0].get())->get_camera();
    return cameras[curcam].get();
}

gearoenix::render::camera::Camera* gearoenix::render::scene::Scene::get_current_camera()
{
    //    return reinterpret_cast<light::Sun*>(lights[0].get())->get_camera();
    return cameras[curcam].get();
}

const gearoenix::math::Vec3& gearoenix::render::scene::Scene::get_ambient_light() const
{
    return ambient_light;
}

const gearoenix::render::light::Sun* gearoenix::render::scene::Scene::get_sun() const
{
    return reinterpret_cast<light::Sun*>(lights[0].get());
}

void gearoenix::render::scene::Scene::set_renderable(bool b)
{
    renderable = b;
}
