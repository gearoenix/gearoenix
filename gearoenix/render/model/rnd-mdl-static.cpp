#include "rnd-mdl-static.hpp"
#include "../../physics/collider/phs-collider.hpp"

gearoenix::render::model::Static::Static(
    core::Id my_id,
    system::stream::Stream* f,
    Engine* e,
    core::sync::EndCaller<core::sync::EndCallerIgnore> c)
    : Model(my_id, RenderModel::STATIC, f, e, c)
{
    initialize_axis(m);
    if (nullptr != collider) {
        collider->update(m);
    }
    moccloc = m * occloc;
}

gearoenix::render::model::Static::~Static()
{
}

// Untransformable---------------------------------------------------------
void gearoenix::render::model::Static::get_location(math::Vec3& l) const
{
    m.get_location(l);
}
