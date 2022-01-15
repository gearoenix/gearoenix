#ifndef GEAROENIX_RENDER_VERTEX_HPP
#define GEAROENIX_RENDER_VERTEX_HPP
#include "../math/gx-math-vector-4d.hpp"

namespace gearoenix::render {
struct PbrVertex final {
    math::Vec3<float> position;
    math::Vec3<float> normal;
    math::Vec4<float> tangent;
    math::Vec2<float> uv;

    constexpr PbrVertex() noexcept = default;

    constexpr PbrVertex(const math::Vec3<float>& position,
        const math::Vec3<float>& normal,
        const math::Vec4<float>& tangent,
        const math::Vec2<float>& uv) noexcept
        : position(position)
        , normal(normal)
        , tangent(tangent)
        , uv(uv)
    {
    }

    explicit PbrVertex(platform::stream::Stream& f) noexcept;
    void read(platform::stream::Stream& f) noexcept;

    constexpr void set_position(const float x, const float y, const float z) noexcept
    {
        position.x = x;
        position.y = y;
        position.z = z;
    }

    constexpr void set_uv(const float x, const float y) noexcept
    {
        uv.x = x;
        uv.y = y;
    }

    friend std::ostream& operator<<(std::ostream& os, const PbrVertex& v) noexcept
    {
        os << R"({ "type": "PbrVertex", "position": )" << v.position
           << ", \"normal\": " << v.normal
           << ", \"tangent\": " << v.tangent
           << ", \"uv\": " << v.uv << "}";
        return os;
    }
};
}
#endif
