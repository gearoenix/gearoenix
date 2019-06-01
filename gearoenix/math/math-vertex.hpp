#ifndef GEAROENIX_MATH_VERTEX_HPP
#define GEAROENIX_MATH_VERTEX_HPP
#include "math-vector.hpp"

namespace gearoenix::math {
struct BasicVertex {
    Vec3 position;
    Vec3 normal;
    Vec4 tangent;
    Vec2 uv;

    BasicVertex() noexcept = default;
    BasicVertex(const Vec3& position, const Vec3& normal, const Vec4& tangent, const Vec2& uv) noexcept;
    explicit BasicVertex(system::stream::Stream* f) noexcept;
    void read(system::stream::Stream* f) noexcept;

    friend std::ostream& operator<<(std::ostream& os, const BasicVertex& v) noexcept
    {
        os << "\nVertex\n{\n\tposition: " << v.position
           << ",\n\tnormal: " << v.normal
           << ",\n\ttangent: " << v.tangent
           << ",\n\tuv: " << v.uv << "\n}";
        return os;
    }
};
}
#endif
