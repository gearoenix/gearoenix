#include "rnd-mdl-model.hpp"
#include "../../system/sys-file.hpp"
#include "rnd-mdl-copy.hpp"
#include "rnd-mdl-dynamic.hpp"
#include "rnd-mdl-root-static.hpp"
#include "rnd-mdl-static.hpp"

gearoenix::render::model::Model::Model() {}
gearoenix::render::model::Model::~Model() {}

gearoenix::render::model::Model* gearoenix::render::model::Model::read_child(system::File* f, Engine* e, std::shared_ptr<core::EndCaller> c)
{
    if (f->read<bool>())
        return new Copy(f, e, c);
    else if (f->read<bool>())
        return new Dynamic(f, e, c);
    else
        return new Static(f, e, c);
}

gearoenix::render::model::Model* gearoenix::render::model::Model::read(system::File* f, Engine* e, std::shared_ptr<core::EndCaller> c)
{
    if (f->read<bool>())
        return new Copy(f, e, c);
    else if (f->read<bool>())
        return new Dynamic(f, e, c);
    else
        return new RootStatic(f, e, c);
}

const gearoenix::math::Mat4x4& gearoenix::render::model::Model::get_m() const
{
    return m;
}

const gearoenix::math::Mat4x4& gearoenix::render::model::Model::get_mvp() const
{
    return mvp;
}
