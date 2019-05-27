#include "phs-collider.hpp"
#include "../../system/stream/sys-stm-stream.hpp"
#include "../../system/sys-log.hpp"
#include "phs-collider-mesh.hpp"

gearoenix::physics::collider::Collider::Collider(Type t) noexcept
    : t(t)
{
}

gearoenix::physics::collider::Collider* gearoenix::physics::collider::Collider::read(system::stream::Stream*const f) noexcept
{
    core::Id collider_type_id;
    f->read(collider_type_id);
    switch (collider_type_id) {
    case Type::GHOST:
        return nullptr;
    case Type::MESH:
        return new Mesh(f);
    default:
        GXUNEXPECTED
    }
}
