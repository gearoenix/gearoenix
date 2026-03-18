#include "gx-rnd-vertex.hpp"

gearoenix::render::PbrVertex::PbrVertex(platform::stream::Stream& f) { read(f); }

void gearoenix::render::PbrVertex::read(platform::stream::Stream& f)
{
    position.read(f);
    normal.read(f);
    tangent.read(f);
    uv.read(f);
}

std::ostream& operator<<(std::ostream& os, const gearoenix::render::PbrVertex& v)
{
    os << R"({ "type": "PbrVertex", "position": )" << v.position << ", \"normal\": " << v.normal << ", \"tangent\": " << v.tangent << ", \"uv\": " << v.uv << "}";
    return os;
}

const void* gearoenix::render::get_data(const Vertices& vertices)
{
    switch (vertices.index()) {
    case PBR_VERTEX_VARIANT_TYPE_INDEX:
        return std::get<std::vector<PbrVertex>>(vertices).data();
    case PBR_VERTEX_ANIMATED_VARIANT_TYPE_INDEX:
        return std::get<std::vector<PbrVertexAnimated>>(vertices).data();
    default:
        GX_UNEXPECTED;
    }
}

std::uint64_t gearoenix::render::get_element_size(const Vertices& vertices)
{
    switch (vertices.index()) {
    case PBR_VERTEX_VARIANT_TYPE_INDEX:
        return sizeof(PbrVertex);
    case PBR_VERTEX_ANIMATED_VARIANT_TYPE_INDEX:
        return sizeof(PbrVertexAnimated);
    default:
        GX_UNEXPECTED;
    }
}

bool gearoenix::render::has_bone_weights(const Vertices& vertices)
{
    switch (vertices.index()) {
    case PBR_VERTEX_VARIANT_TYPE_INDEX:
        return false;
    case PBR_VERTEX_ANIMATED_VARIANT_TYPE_INDEX:
        return true;
    default:
        GX_UNEXPECTED;
    }
}

bool gearoenix::render::has_bone_indices(const Vertices& vertices)
{
    switch (vertices.index()) {
    case PBR_VERTEX_VARIANT_TYPE_INDEX:
        return false;
    case PBR_VERTEX_ANIMATED_VARIANT_TYPE_INDEX:
        return true;
    default:
        GX_UNEXPECTED;
    }
}

gearoenix::math::Vec3<float>& gearoenix::render::get_position(Vertices& vertices, const std::uint32_t index)
{
    switch (vertices.index()) {
    case PBR_VERTEX_VARIANT_TYPE_INDEX:
        return std::get<PBR_VERTEX_VARIANT_TYPE_INDEX>(vertices)[index].position;
    case PBR_VERTEX_ANIMATED_VARIANT_TYPE_INDEX:
        return std::get<PBR_VERTEX_ANIMATED_VARIANT_TYPE_INDEX>(vertices)[index].base.position;
    default:
        GX_UNEXPECTED;
    }
}

gearoenix::math::Vec3<float>& gearoenix::render::get_normal(Vertices& vertices, const std::uint32_t index)
{
    switch (vertices.index()) {
    case PBR_VERTEX_VARIANT_TYPE_INDEX:
        return std::get<PBR_VERTEX_VARIANT_TYPE_INDEX>(vertices)[index].normal;
    case PBR_VERTEX_ANIMATED_VARIANT_TYPE_INDEX:
        return std::get<PBR_VERTEX_ANIMATED_VARIANT_TYPE_INDEX>(vertices)[index].base.normal;
    default:
        GX_UNEXPECTED;
    }
}

gearoenix::math::Vec4<float>& gearoenix::render::get_tangent(Vertices& vertices, const std::uint32_t index)
{
    switch (vertices.index()) {
    case PBR_VERTEX_VARIANT_TYPE_INDEX:
        return std::get<PBR_VERTEX_VARIANT_TYPE_INDEX>(vertices)[index].tangent;
    case PBR_VERTEX_ANIMATED_VARIANT_TYPE_INDEX:
        return std::get<PBR_VERTEX_ANIMATED_VARIANT_TYPE_INDEX>(vertices)[index].base.tangent;
    default:
        GX_UNEXPECTED;
    }
}

gearoenix::math::Vec2<float>& gearoenix::render::get_uv(Vertices& vertices, const std::uint32_t index)
{
    switch (vertices.index()) {
    case PBR_VERTEX_VARIANT_TYPE_INDEX:
        return std::get<PBR_VERTEX_VARIANT_TYPE_INDEX>(vertices)[index].uv;
    case PBR_VERTEX_ANIMATED_VARIANT_TYPE_INDEX:
        return std::get<PBR_VERTEX_ANIMATED_VARIANT_TYPE_INDEX>(vertices)[index].base.uv;
    default:
        GX_UNEXPECTED;
    }
}

void gearoenix::render::calculate_tangents(PbrVertices& vertices, const std::vector<std::uint32_t>& indices)
{
    const auto vertices_count = vertices.size();

    std::vector<math::Vec3<float>> tan_a(vertices_count);
    std::vector<math::Vec3<float>> tan_b(vertices_count);

    constexpr auto min_sq_len = math::Numeric::epsilon<float> * math::Numeric::epsilon<float>;

    const auto index_count = indices.size();
    for (std::size_t i = 0; i + 2 < index_count; i += 3) {
        const auto i0 = indices[i];
        const auto i1 = indices[i + 1];
        const auto i2 = indices[i + 2];

        const auto& pos0 = vertices[i0].position;
        const auto& pos1 = vertices[i1].position;
        const auto& pos2 = vertices[i2].position;

        const auto edge1 = pos1 - pos0;
        const auto edge2 = pos2 - pos0;

        math::Vec3<float> tangent;
        math::Vec3<float> bitangent;

        const auto& tex0 = vertices[i0].uv;
        const auto& tex1 = vertices[i1].uv;
        const auto& tex2 = vertices[i2].uv;

        const auto uv1 = tex1 - tex0;
        const auto uv2 = tex2 - tex0;

        if (const auto dom_r = uv1.x * uv2.y - uv1.y * uv2.x; std::abs(dom_r) > math::Numeric::epsilon<float>) {
            const auto r = 1.0f / dom_r;
            tangent = math::Vec3(
                (edge1.x * uv2.y - edge2.x * uv1.y) * r,
                (edge1.y * uv2.y - edge2.y * uv1.y) * r,
                (edge1.z * uv2.y - edge2.z * uv1.y) * r);
            bitangent = math::Vec3(
                (edge1.x * uv2.x - edge2.x * uv1.x) * r,
                (edge1.y * uv2.x - edge2.y * uv1.x) * r,
                (edge1.z * uv2.x - edge2.z * uv1.x) * r);
        } else if (const auto face_normal = edge1.cross(edge2); face_normal.square_length() > min_sq_len) {
            tangent = edge1.square_length() >= edge2.square_length() ? edge1 : edge2;
            bitangent = face_normal.cross(tangent);
        } else {
            const auto& n0 = vertices[i0].normal;
            const auto perp = std::abs(n0.x) < 0.9f ? math::Vec3(1.0f, 0.0f, 0.0f) : math::Vec3(0.0f, 1.0f, 0.0f);
            tangent = n0.cross(perp);
            bitangent = n0.cross(tangent);
        }

        tan_a[i0] += tangent;
        tan_a[i1] += tangent;
        tan_a[i2] += tangent;

        tan_b[i0] += bitangent;
        tan_b[i1] += bitangent;
        tan_b[i2] += bitangent;
    }

    for (std::size_t i = 0; i < vertices_count; ++i) {
        const auto& n = vertices[i].normal;
        const auto& t0 = tan_a[i];
        const auto& t1 = tan_b[i];

        auto t = t0 - (n * n.dot(t0));
        auto w = 1.0f;
        if (const auto len_sq = t.square_length(); len_sq <= min_sq_len) {
            t = n.cross(std::abs(n.x) < 0.9f ? math::Vec3(1.0f, 0.0f, 0.0f) : math::Vec3(0.0f, 1.0f, 0.0f)).normalised();
        } else {
            t /= std::sqrt(len_sq);
            w = n.cross(t0).dot(t1) < 0.0f ? -1.0f : 1.0f;
        }
        vertices[i].tangent = math::Vec4(t.x, t.y, t.z, w);
    }
}

std::uint64_t gearoenix::core::count(const render::Vertices& vertices)
{
    switch (vertices.index()) {
    case render::PBR_VERTEX_VARIANT_TYPE_INDEX:
        return std::get<std::vector<render::PbrVertex>>(vertices).size();
    case render::PBR_VERTEX_ANIMATED_VARIANT_TYPE_INDEX:
        return std::get<std::vector<render::PbrVertexAnimated>>(vertices).size();
    default:
        GX_UNEXPECTED;
    }
}

std::uint64_t gearoenix::core::bytes_count(const render::Vertices& vertices)
{
    switch (vertices.index()) {
    case render::PBR_VERTEX_VARIANT_TYPE_INDEX:
        return std::get<std::vector<render::PbrVertex>>(vertices).size() * sizeof(render::PbrVertex);
    case render::PBR_VERTEX_ANIMATED_VARIANT_TYPE_INDEX:
        return std::get<std::vector<render::PbrVertexAnimated>>(vertices).size() * sizeof(render::PbrVertexAnimated);
    default:
        GX_UNEXPECTED;
    }
}