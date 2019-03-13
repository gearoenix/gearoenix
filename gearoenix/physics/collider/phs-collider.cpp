#include "phs-collider.hpp"
#include "../../system/stream/sys-stm-stream.hpp"
#include "../../system/sys-log.hpp"
#include "phs-collider-mesh.hpp"

gearoenix::physics::collider::Collider::Collider(Type t)
    : t(t)
{
}

gearoenix::physics::collider::Collider::~Collider() {}

gearoenix::physics::collider::Collider* gearoenix::physics::collider::Collider::read(const std::shared_ptr<system::stream::Stream> &f)
{
    core::Id collider_type_id;
    f->read(collider_type_id);
    switch (collider_type_id) {
    case Type::GHOST:
        return nullptr;
    case Type::MESH:
        return new Mesh(f);
    default:
        GXUNEXPECTED;
    }
}
