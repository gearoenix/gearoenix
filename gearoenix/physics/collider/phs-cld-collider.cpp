#include "phs-cld-collider.hpp"
#include "../../system/stream/sys-stm-stream.hpp"
#include "../../system/sys-log.hpp"
#include "../phs-transformation.hpp"
#include "phs-cld-mesh.hpp"

gearoenix::physics::collider::Collider::Collider(const Type t) noexcept : collider_type(t) {}

gearoenix::physics::collider::Collider::~Collider() {
    transformation = nullptr;
}

gearoenix::physics::collider::Collider* gearoenix::physics::collider::Collider::read(system::stream::Stream* const f) noexcept
{
    Type collider_type_id;
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

std::optional<gearoenix::core::Real> gearoenix::physics::collider::Collider::hit(const math::Ray3& r, const core::Real d_min) const noexcept {
    auto i = hit(r);
    if(i.has_value() && i.value() < d_min) return i;
    return std::nullopt; 
}
