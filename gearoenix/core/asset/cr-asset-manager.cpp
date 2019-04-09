#include "cr-asset-manager.hpp"
#include "../../audio/au-manager.hpp"
#include "../../physics/constraint/phs-cns-manager.hpp"
#include "../../render/camera/rnd-cmr-manager.hpp"
#include "../../render/engine/rnd-eng-engine.hpp"
#include "../../render/font/rnd-fnt-manager.hpp"
#include "../../render/light/rnd-lt-manager.hpp"
#include "../../render/mesh/rnd-msh-manager.hpp"
#include "../../render/model/rnd-mdl-manager.hpp"
#include "../../render/scene/rnd-scn-manager.hpp"
#include "../../render/skybox/rnd-sky-manager.hpp"
#include "../../render/texture/rnd-txt-manager.hpp"
#include "../../system/stream/sys-stm-asset.hpp"
#include "../../system/sys-app.hpp"
#include "../cache/cr-cache-file.hpp"
#include "../cr-static.hpp"

std::atomic<gearoenix::core::Id> gearoenix::core::asset::Manager::last_id(0);

gearoenix::core::asset::Manager::Manager(const std::shared_ptr<system::Application>& sys_app, const std::string& name)
    : sys_app(sys_app)
    , render_engine(sys_app->get_render_engine())
    , file(std::shared_ptr<system::stream::Asset>(new system::stream::Asset(sys_app, name)))
{
    std::shared_ptr<system::stream::Stream> s = std::static_pointer_cast<system::stream::Stream>(file);
    last_id.store(s->read<Id>());
    core::Count off;

#define GXHELPER(a, n)                                                                     \
    off = s->tell();                                                                       \
    s = std::shared_ptr<system::stream::Stream>(new system::stream::Asset(sys_app, name)); \
    s->seek(off);                                                                          \
    a##_manager = std::make_shared<n::Manager>(s, render_engine);

    GXHELPER(camera, render::camera);
    GXHELPER(audio, audio);
    GXHELPER(light, render::light);
    GXHELPER(texture, render::texture);
    GXHELPER(font, render::font);
    GXHELPER(mesh, render::mesh);
    GXHELPER(model, render::model);
    GXHELPER(skybox, render::skybox);
    GXHELPER(constraint, physics::constraint);
    GXHELPER(scene, render::scene);
#undef GXHELPER
}

const std::shared_ptr<gearoenix::system::stream::Asset>& gearoenix::core::asset::Manager::get_file() const
{
    return file;
}

const std::shared_ptr<gearoenix::render::camera::Manager>& gearoenix::core::asset::Manager::get_camera_manager() const
{
    return camera_manager;
}

const std::shared_ptr<gearoenix::audio::Manager>& gearoenix::core::asset::Manager::get_audio_manager() const
{
    return audio_manager;
}

const std::shared_ptr<gearoenix::render::light::Manager>& gearoenix::core::asset::Manager::get_light_manager() const
{
    return light_manager;
}

const std::shared_ptr<gearoenix::render::texture::Manager>& gearoenix::core::asset::Manager::get_texture_manager() const
{
    return texture_manager;
}

const std::shared_ptr<gearoenix::render::font::Manager>& gearoenix::core::asset::Manager::get_font_manager() const
{
    return font_manager;
}

const std::shared_ptr<gearoenix::render::mesh::Manager>& gearoenix::core::asset::Manager::get_mesh_manager() const
{
    return mesh_manager;
}

const std::shared_ptr<gearoenix::render::model::Manager>& gearoenix::core::asset::Manager::get_model_manager() const
{
    return model_manager;
}

const std::shared_ptr<gearoenix::render::skybox::Manager>& gearoenix::core::asset::Manager::get_skybox_manager() const
{
    return skybox_manager;
}

const std::shared_ptr<gearoenix::physics::constraint::Manager>& gearoenix::core::asset::Manager::get_constraint_manager() const
{
    return constraint_manager;
}


const std::shared_ptr<gearoenix::render::scene::Manager>& gearoenix::core::asset::Manager::get_scene_manager() const
{
	return scene_manager;
}

gearoenix::core::Id gearoenix::core::asset::Manager::create_id()
{
    return last_id.fetch_add(1);
}
