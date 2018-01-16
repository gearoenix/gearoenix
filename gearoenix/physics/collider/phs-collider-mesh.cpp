#include "phs-collider-mesh.hpp"
#include "../../math/math-matrix.hpp"
#include "../../system/sys-file.hpp"

gearoenix::physics::collider::Mesh::Mesh(system::File* in)
    : Collider(Type::MESH)
{
    const core::Count cnt = in->read<core::Count>();
    ps.resize(cnt);
    ts.resize(cnt);
    for (unsigned int i = 0; i < cnt; ++i) {
        for (unsigned int j = 0; j < 3; ++j) {
            ps[i].p[j].read(in);
        }
    }
}

gearoenix::physics::collider::Mesh::~Mesh()
{
}

void gearoenix::physics::collider::Mesh::update(const math::Mat4x4& m)
{
    box.reset(ps[0].p[0]);
    const size_t cnt = ps.size();
    for (size_t i = 0; i < cnt; ++i) {
        math::Vec3 p1 = m * (ps[i].p[0]);
        math::Vec3 p2 = m * (ps[i].p[1]);
        math::Vec3 p3 = m * (ps[i].p[2]);
        box.put(p1);
        box.put(p2);
        box.put(p3);
        ts[i] = math::Triangle3(p1, p2, p3);
    }
}