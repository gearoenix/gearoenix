#include "phs-cld-mesh.hpp"
#include "../../math/math-matrix.hpp"
#include "../../math/math-ray.hpp"
#include "../../system/stream/sys-stm-stream.hpp"
#include "../../system/sys-log.hpp"

gearoenix::physics::collider::Mesh::Mesh(system::stream::Stream* const f) noexcept
    : Collider(Type::Mesh)
{
    const auto cnt = f->read<core::Count>();
    vertices.resize(cnt);
    for (core::Count i = 0; i < cnt; ++i)
        vertices[i].read(f);
    f->read(indices);
    ts.resize(indices.size());
}

// void gearoenix::physics::collider::Mesh::update(const math::Mat4x4& m) noexcept
// {
//     const size_t vscnt = vertices.size();
//     std::vector<math::Vec3> transformed_vertices(vscnt);
//     for (size_t i = 0; i < vscnt; ++i) {
//         transformed_vertices[i] = m * vertices[i];
//     }
//     box.reset(transformed_vertices[0]);
//     const size_t cnt = indices.size();
//     for (std::uint16_t i = 0; i < cnt; ++i) {
//         const math::Vec3& p1 = transformed_vertices[indices[i]];
//         ++i;
//         const math::Vec3& p2 = transformed_vertices[indices[i]];
//         ++i;
//         const math::Vec3& p3 = transformed_vertices[indices[i]];
//         box.put(p1);
//         box.put(p2);
//         box.put(p3);
//         ts[i] = math::Triangle3(p1, p2, p3);
//     }
// }

std::optional<gearoenix::core::Real> gearoenix::physics::collider::Mesh::hit(const math::Ray3&, const core::Real) const noexcept
{
    // first collider base hit (aabb)
    // second loop over traingles (triangles count must be low)
    GXUNIMPLEMENTED
}

// std::optional<std::pair<gearoenix::core::Real, gearoenix::math::Vec2>> gearoenix::physics::collider::Mesh::hit(const math::Ray3& r) const noexcept
// {
//     core::Real tmpd = d;
//     if (box.test(r, tmpd)) {
//         bool result = false;
//         for (const math::Triangle3& t : ts) {
//             math::Vec3 mfs;
//             if (t.intersect(r, d, mfs)) {
//                 fs = mfs.xy();
//                 d = mfs[0];
//                 result = true;
//             }
//         }
//         return result;
//     }
//     return std::nullopt;
// }
