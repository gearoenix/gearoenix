#include "phs-collider.hpp"
#include "../system/sys-file.hpp"
#include "../system/sys-log.hpp"
#include "phs-collider-mesh.hpp"

gearoenix::physics::collider::Collider::Collider(Type t)
    : t(t)
{
}

gearoenix::physics::collider::Collider* gearoenix::physics::collider::Collider::read(system::File* in)
{
    core::Id collider_type_id;
    in->read(collider_type_id);
    switch (collider_type_id) {
    case Type::GHOST:
        return nullptr;
    case Type::MESH:
        return new Mesh(in);
    default:
        UNEXPECTED;
    }
}