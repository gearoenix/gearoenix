#ifndef GEAROENIX_RENDER_VERTEX_HPP
#define GEAROENIX_RENDER_VERTEX_HPP
#include "../math/gx-math-vector-4d.hpp"
#include <variant>
#include <vector>

namespace gearoenix::render {
struct PbrVertex final {
    math::Vec3<float> position;
    math::Vec3<float> normal;
    math::Vec4<float> tangent;
    math::Vec2<float> uv;

    constexpr PbrVertex() noexcept
        : position(0.0f)
        , normal(0.0f, 0.0f, 1.0f)
        , tangent(0.0f, 1.0f, 0.0f, 1.0)
        , uv(0.0f)
    {
    }

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
    friend std::ostream& operator<<(std::ostream& os, const PbrVertex& v) noexcept;
};

struct PbrVertexAnimated final {
    PbrVertex base;
    math::Vec4<float> bone_weights;
    math::Vec4<float> bone_indices;
};

typedef std::variant<std::vector<PbrVertex>, std::vector<PbrVertexAnimated>> Vertices;

[[nodiscard]] const void* get_data(const render::Vertices& vertices) noexcept;
[[nodiscard]] std::size_t get_element_size(const render::Vertices& vertices) noexcept;
[[nodiscard]] bool has_bone_weights(const render::Vertices& vertices) noexcept;
[[nodiscard]] bool has_bone_indices(const render::Vertices& vertices) noexcept;
}

namespace gearoenix::core {
[[nodiscard]] std::size_t count(const render::Vertices& vertices) noexcept;
[[nodiscard]] std::size_t bytes_count(const render::Vertices& vertices) noexcept;
}

#endif
