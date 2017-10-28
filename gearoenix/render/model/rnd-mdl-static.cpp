#include "rnd-mdl-static.hpp"
#include "../../core/cr-end-caller.hpp"
#include "../../system/sys-file.hpp"
#include "../camera/rnd-cmr-camera.hpp"
#include "../mesh/rnd-msh-mesh.hpp"
#include "../scene/rnd-scn-scene.hpp"

gearoenix::render::model::Static::Static(system::File* f, Engine* e, std::shared_ptr<core::EndCaller> c)
    : mesh(new mesh::Mesh(f, e, c))
{
    //    LOGE("location: " << f->tell());
    core::Count cc;
    f->read(cc);
    children.resize((size_t)cc);
    for (core::Count i = 0; i < cc; ++i)
        children[(size_t)i] = Model::read_child(f, e, c);
}

void gearoenix::render::model::Static::commit(const scene::Scene*)
{
    UNEXPECTED;
}

void gearoenix::render::model::Static::commit(const scene::Scene* s, const Model* parent)
{
    mesh->commit(s, parent);
    for (Model* m : children) {
        m->commit(s, parent);
    }
}

void gearoenix::render::model::Static::draw()
{
    mesh->draw();
}
