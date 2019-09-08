#include "phs-cld-collider.hpp"
#include "../../system/stream/sys-stm-stream.hpp"
#include "../../system/sys-log.hpp"
#include "../phs-transformation.hpp"
#include "phs-cld-mesh.hpp"


gearoenix::physics::collider::Collider* gearoenix::physics::collider::Collider::read(system::stream::Stream* const f) noexcept
{
    Type collider_type_id;
    f->read(collider_type_id);
    switch (collider_type_id) {
    case Type::Ghost:
        return nullptr;
    case Type::Mesh:
        return new Mesh(f);
    default:
        GXUNEXPECTED
    }
}

std::optional<gearoenix::core::Real> gearoenix::physics::collider::Collider::hit(const math::Ray3& r, const core::Real d_min) const noexcept {
    return box.hit(r, d_min);
}
