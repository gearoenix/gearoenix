#include "rnd-mdl-dynamic.hpp"
#include "../../core/cr-end-caller.hpp"
#include "../../system/sys-file.hpp"
#include "../camera/rnd-cmr-camera.hpp"
#include "../mesh/rnd-msh-occ.hpp"

gearoenix::render::model::Dynamic::Dynamic(system::File* f, Engine* e, std::shared_ptr<core::EndCaller> c)
{
    u.get_m().read(f);
    //    LOGE("location: " << f->tell());
    occmesh = new mesh::Occ(f, e, c);
    core::Count cc;
    f->read(cc);
    //    LOGE("location: " << f->tell());
    children.resize(cc);
    //    LOGE("location: " << f->tell());
    for (core::Count i = 0; i < cc; ++i)
        children[i] = Model::read_child(f, e, c);
}

void gearoenix::render::model::Dynamic::draw(const std::shared_ptr<camera::Camera>& cam)
{
    u.get_mvp() = cam->get_view_projection() * u.get_m();
    for (Model* m : children) {
        m->draw(cam, u);
    }
}

void gearoenix::render::model::Dynamic::draw(const std::shared_ptr<camera::Camera>& cam, const Uniform&)
{
    draw(cam);
}
