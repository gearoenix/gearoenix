#include "rnd-mdl-static.hpp"
#include "../../core/cr-end-caller.hpp"
#include "../../system/sys-file.hpp"
#include "../camera/rnd-cmr-camera.hpp"
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

void gearoenix::render::model::Static::draw(const std::shared_ptr<camera::Camera>&)
{
    LOGF("Static model does not implement this function.");
}

void gearoenix::render::model::Static::draw(const std::shared_ptr<camera::Camera>& cam, const Uniform& pu)
{
    for (Model* m : children) {
        m->draw(cam, pu);
    }
}
