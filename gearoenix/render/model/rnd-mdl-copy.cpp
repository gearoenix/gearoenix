#include "rnd-mdl-copy.hpp"
#include "../../core/asset/cr-asset-manager.hpp"
#include "../../core/cr-end-caller.hpp"
#include "../../math/math-matrix.hpp"
#include "../../system/sys-app.hpp"
#include "../../system/sys-file.hpp"
#include "../camera/rnd-cmr-camera.hpp"
#include "../rnd-engine.hpp"
#include "../scene/rnd-scn-scene.hpp"
#include "rnd-mdl-root-static.hpp"

gearoenix::render::model::Copy::Copy(system::File* f, Engine* e, std::shared_ptr<core::EndCaller> c)
{
    m.read(f);
    core::Id rsid;
    f->read(rsid);
    unsigned int cp = f->tell();
    rs = std::static_pointer_cast<RootStatic>(e->get_system_application()->get_asset_manager()->get_model(rsid, c));
    f->seek(cp);
}

void gearoenix::render::model::Copy::commit(const scene::Scene* s)
{
    mvp = s->get_current_camera()->get_view_projection() * m;
    rs->commit(s, this);
}

void gearoenix::render::model::Copy::commit(const scene::Scene* s, const Model*)
{
    commit(s);
}

void gearoenix::render::model::Copy::draw()
{
    UNIMPLEMENTED;
}
