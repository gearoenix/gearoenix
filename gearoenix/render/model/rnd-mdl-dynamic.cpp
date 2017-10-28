#include "rnd-mdl-dynamic.hpp"
#include "../../core/cr-end-caller.hpp"
#include "../../system/sys-file.hpp"
#include "../camera/rnd-cmr-camera.hpp"
#include "../mesh/rnd-msh-occ.hpp"
#include "../scene/rnd-scn-scene.hpp"

gearoenix::render::model::Dynamic::Dynamic(system::File* f, Engine* e, std::shared_ptr<core::EndCaller> c)
{
    m.read(f);
    //    LOGE("location: " << f->tell());
    occmesh = new mesh::Occ(f, e, c);
    core::Count cc;
    f->read(cc);
    //    LOGE("location: " << f->tell());
    children.resize((size_t)cc);
    //    LOGE("location: " << f->tell());
    for (core::Count i = 0; i < cc; ++i)
        children[(size_t)i] = Model::read_child(f, e, c);
}

void gearoenix::render::model::Dynamic::commit(const scene::Scene* s)
{
    mvp = s->get_current_camera()->get_view_projection() * m;
    for (Model* m : children) {
        m->commit(s, this);
    }
}

void gearoenix::render::model::Dynamic::commit(const scene::Scene* s, const Model*)
{
    commit(s);
}

void gearoenix::render::model::Dynamic::draw()
{
    for (Model* m : children) {
        m->draw();
    }
}
