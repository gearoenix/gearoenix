#ifndef GEAROENIX_MATH_VERTEX_HPP
#define GEAROENIX_MATH_VERTEX_HPP
#include "gx-math-vector-4d.hpp"

namespace gearoenix::math {
struct BasicVertex {
    Vec3<float> position;
    Vec3<float> normal;
    Vec4<float> tangent;
    Vec2<float> uv;

    BasicVertex() noexcept = default;
    BasicVertex(const Vec3<float>& position, const Vec3<float>& normal, const Vec4<float>& tangent, const Vec2<float>& uv) noexcept;
    explicit BasicVertex(system::stream::Stream* f) noexcept;
    void read(system::stream::Stream* f) noexcept;

    friend std::ostream& operator<<(std::ostream& os, const BasicVertex& v) noexcept
    {
        os << "\n\"Vertex\"\n{\n\t\"position\": " << v.position
           << ",\n\t\"normal\": " << v.normal
           << ",\n\t\"tangent\": " << v.tangent
           << ",\n\t\"uv\": " << v.uv << "\n}";
        return os;
    }
};
}
#endif
