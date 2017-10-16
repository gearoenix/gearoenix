#include "rnd-scn-scene.hpp"
#include "../../audio/au-audio.hpp"
#include "../../core/asset/cr-asset-manager.hpp"
#include "../../core/cr-end-caller.hpp"
#include "../../system/sys-app.hpp"
#include "../../system/sys-file.hpp"
#include "../camera/rnd-cmr-camera.hpp"
#include "../light/rnd-lt-light.hpp"
#include "../model/rnd-mdl-model.hpp"
#include "../rnd-engine.hpp"

gearoenix::render::scene::Scene::Scene(system::File* f, Engine* e, std::shared_ptr<core::EndCaller> c)
    : e(e)
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
    for (size_t i = 0; i < camera_ids.size(); ++i)
        cameras[i] = amgr->get_camera(camera_ids[i]);
    for (size_t i = 0; i < audio_ids.size(); ++i)
        audios[i] = amgr->get_audio(audio_ids[i]);
    for (size_t i = 0; i < light_ids.size(); ++i)
        lights[i] = amgr->get_light(light_ids[i]);
    for (size_t i = 0; i < model_ids.size(); ++i)
        models[i] = amgr->get_model(model_ids[i], c);
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

void gearoenix::render::scene::Scene::draw()
{
    const std::shared_ptr<camera::Camera>& cam = cameras[curcam];
    for (std::shared_ptr<model::Model>& m : models) {
        m->draw(cam);
    }
}
