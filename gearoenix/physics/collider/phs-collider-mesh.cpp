#include "phs-collider-mesh.hpp"
#include "../../math/math-matrix.hpp"
#include "../../math/math-ray.hpp"
#include "../../system/stream/sys-stm-stream.hpp"
#include "../../system/sys-log.hpp"

gearoenix::physics::collider::Mesh::Mesh(const std::shared_ptr<system::stream::Stream>& f)
    : Collider(Type::MESH)
{
    const core::Count cnt = f->read<core::Count>();
    vertices.resize(cnt);
    for (core::Count i = 0; i < cnt; ++i)
        vertices[i].read(f);
    f->read(indices);
    ts.resize(indices.size());
}

gearoenix::physics::collider::Mesh::~Mesh()
{
}

void gearoenix::physics::collider::Mesh::update(const math::Mat4x4& m)
{
    const size_t vscnt = vertices.size();
    std::vector<math::Vec3> transformed_vertices(vscnt);
    for (size_t i = 0; i < vscnt; ++i) {
        transformed_vertices[i] = m * vertices[i];
    }
    box.reset(transformed_vertices[0]);
    const size_t cnt = indices.size();
    for (std::uint16_t i = 0; i < cnt; ++i) {
        const math::Vec3& p1 = transformed_vertices[indices[i]];
        ++i;
        const math::Vec3& p2 = transformed_vertices[indices[i]];
        ++i;
        const math::Vec3& p3 = transformed_vertices[indices[i]];
        box.put(p1);
        box.put(p2);
        box.put(p3);
        ts[i] = math::Triangle3(p1, p2, p3);
    }
}

bool gearoenix::physics::collider::Mesh::hit(const math::Ray3& r, core::Real& d) const
{
    math::Vec2 fs;
    return hit(r, d, fs);
}

bool gearoenix::physics::collider::Mesh::hit(const math::Ray3& r, core::Real& d, math::Vec2& fs) const
{
    core::Real tmpd = d;
    if (box.test(r, tmpd)) {
        bool result = false;
        for (const math::Triangle3& t : ts) {
            math::Vec3 mfs;
            if (t.intersect(r, d, mfs)) {
                fs = mfs.xy();
                d = mfs[0];
                result = true;
            }
        }
        return result;
    }
    return false;
}

void gearoenix::physics::collider::Mesh::print() const
{
    GXLOGI("Begin of collider print:");
    for (const math::Triangle3& t : ts) {
        t.print();
    }
    GXLOGI("End of collider print.");
}
