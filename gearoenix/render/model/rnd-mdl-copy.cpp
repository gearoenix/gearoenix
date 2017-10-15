#include "rnd-mdl-copy.hpp"
#include "../../core/asset/cr-asset-manager.hpp"
#include "../../core/cr-end-caller.hpp"
#include "../../math/math-matrix.hpp"
#include "../../system/sys-app.hpp"
#include "../../system/sys-file.hpp"
#include "../rnd-engine.hpp"
#include "rnd-mdl-root-static.hpp"
#include "rnd-mdl-uniform.hpp"

gearoenix::render::model::Copy::Copy(system::File* f, Engine* e, std::shared_ptr<core::EndCaller> c)
{
    u.get_m().read(f);
    core::Id rsid;
    f->read(rsid);
    unsigned int cp = f->tell();
    rs = std::static_pointer_cast<RootStatic>(e->get_system_application()->get_asset_manager()->get_model(rsid, c));
    f->seek(cp);
}