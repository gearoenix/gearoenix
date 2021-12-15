#ifndef GEAROENIX_RENDER_VERTEX_HPP
#define GEAROENIX_RENDER_VERTEX_HPP
#include "../math/gx-math-vector-4d.hpp"

namespace gearoenix::render {
struct PbrVertex final {
    math::Vec3<float> position;
    math::Vec3<float> normal;
    math::Vec4<float> tangent;
    math::Vec2<float> uv;

    PbrVertex() noexcept = default;
    PbrVertex(const math::Vec3<float>& position, const math::Vec3<float>& normal, const math::Vec4<float>& tangent, const math::Vec2<float>& uv) noexcept;
    explicit PbrVertex(platform::stream::Stream& f) noexcept;
    void read(platform::stream::Stream& f) noexcept;
    void set_position(float x, float y, float z) noexcept;
    void set_uv(float x, float y) noexcept;

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
