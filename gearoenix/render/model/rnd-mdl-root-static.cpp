#include "rnd-mdl-root-static.hpp"
#include "../../core/cr-end-caller.hpp"
#include "../../system/sys-file.hpp"
#include "../mesh/rnd-msh-occ.hpp"

gearoenix::render::model::RootStatic::RootStatic(system::File* f, Engine* e, std::shared_ptr<core::EndCaller> c)
    : occmesh(new mesh::Occ(f, e, c))
{
    core::Count cc;
    f->read(cc);
    children.resize(cc);
    for (core::Count i = 0; i < cc; ++i)
        children[i] = Model::read_child(f, e, c);
}

gearoenix::render::model::RootStatic::~RootStatic()
{
    for (Model* m : children)
        delete m;
    children.clear();
    delete occmesh;
}
