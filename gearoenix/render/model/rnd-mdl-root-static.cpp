#include "rnd-mdl-root-static.hpp"
#include "../../core/cr-end-caller.hpp"
#include "../../system/sys-file.hpp"
#include "../camera/rnd-cmr-camera.hpp"
#include "../mesh/rnd-msh-occ.hpp"
#include "../scene/rnd-scn-scene.hpp"

gearoenix::render::model::RootStatic::RootStatic(system::File* f, Engine* e, std::shared_ptr<core::EndCaller> c)
    : occmesh(new mesh::Occ(f, e, c))
{
    core::Count cc;
    f->read(cc);
    children.resize((size_t)cc);
    for (core::Count i = 0; i < cc; ++i)
        children[(size_t)i] = Model::read_child(f, e, c);
}

gearoenix::render::model::RootStatic::~RootStatic()
{
    for (Model* m : children)
        delete m;
    children.clear();
    delete occmesh;
}

void gearoenix::render::model::RootStatic::commit(const scene::Scene* s)
{
    mvp = s->get_current_camera()->get_view_projection();
    for (Model* m : children) {
        m->commit(s, this);
    }
}

void gearoenix::render::model::RootStatic::commit(const scene::Scene* s, const Model* parent)
{
    for (Model* m : children) {
        m->commit(s, parent);
    }
}

void gearoenix::render::model::RootStatic::draw()
{
    TODO;
    for (Model* m : children) {
        m->draw();
    }
}
