#include "rnd-mdl-static.hpp"
#include "../../core/cr-end-caller.hpp"
#include "../../system/sys-file.hpp"
#include "../mesh/rnd-msh-mesh.hpp"

gearoenix::render::model::Static::Static(system::File* f, Engine* e, std::shared_ptr<core::EndCaller> c)
    : mesh(new mesh::Mesh(f, e, c))
{
    //    LOGE("location: " << f->tell());
    core::Count cc;
    f->read(cc);
    children.resize(cc);
    for (core::Count i = 0; i < cc; ++i)
        children[i] = Model::read_child(f, e, c);
}
