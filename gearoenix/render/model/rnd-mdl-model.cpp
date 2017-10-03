#include "rnd-mdl-model.hpp"
#include "../../system/sys-file.hpp"

gearoenix::render::model::Model::Model() {}

gearoenix::render::model::Model* gearoenix::render::model::Model::read(system::File* f, Engine* e, core::EndCaller* c)
{
    //    if(f->read<bool>()) return new CopyModel(f, e, c);
    //    else if(f->read<bool>()) return new DynamicModel(f, e, c);
    //    else return new RootStaticModel(f, e, c);
    return nullptr;
}
