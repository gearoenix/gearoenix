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

gearoenix::core::Id gearoenix::core::asset::Manager::create_id()
{
    return last_id.fetch_add(1);
}
