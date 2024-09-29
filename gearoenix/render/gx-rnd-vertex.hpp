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

    constexpr PbrVertex()
        : position(0.0f)
        , normal(0.0f, 0.0f, 1.0f)
        , tangent(0.0f, 1.0f, 0.0f, 1.0)
        , uv(0.0f)
    {
    }

    constexpr PbrVertex(const math::Vec3<float>& position,
        const math::Vec3<float>& normal,
        const math::Vec4<float>& tangent,
        const math::Vec2<float>& uv)
        : position(position)
        , normal(normal)
        , tangent(tangent)
        , uv(uv)
    {
    }

    explicit PbrVertex(platform::stream::Stream& f);
    void read(platform::stream::Stream& f);
    friend std::ostream& operator<<(std::ostream& os, const PbrVertex& v);
};

struct PbrVertexAnimated final {
    PbrVertex base;
    math::Vec4<float> bone_weights;
    math::Vec4<float> bone_indices;
};

typedef std::vector<PbrVertex> PbrVertices;
typedef std::vector<PbrVertexAnimated> PbrAnimatedVertices;

typedef std::variant<PbrVertices, PbrAnimatedVertices> Vertices;

constexpr std::size_t PBR_VERTEX_VARIANT_TYPE_INDEX = 0;
constexpr std::size_t PBR_VERTEX_ANIMATED_VARIANT_TYPE_INDEX = 1;

[[nodiscard]] const void* get_data(const Vertices& vertices);
[[nodiscard]] std::size_t get_element_size(const Vertices& vertices);
[[nodiscard]] bool has_bone_weights(const Vertices& vertices);
[[nodiscard]] bool has_bone_indices(const Vertices& vertices);
[[nodiscard]] math::Vec3<float>& get_position(Vertices& vertices, std::uint32_t index);
[[nodiscard]] math::Vec3<float>& get_normal(Vertices& vertices, std::uint32_t index);
[[nodiscard]] math::Vec4<float>& get_tangent(Vertices& vertices, std::uint32_t index);
[[nodiscard]] math::Vec2<float>& get_uv(Vertices& vertices, std::uint32_t index);
void calculate_tangents(PbrVertices& vertices, const std::vector<std::uint32_t>& indices);
}

namespace gearoenix::core {
[[nodiscard]] std::size_t count(const render::Vertices& vertices);
[[nodiscard]] std::size_t bytes_count(const render::Vertices& vertices);
}

#endif
