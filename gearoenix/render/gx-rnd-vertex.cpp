#include "gx-rnd-vertex.hpp"

gearoenix::render::PbrVertex::PbrVertex(platform::stream::Stream& f) noexcept
{
    read(f);
}

void gearoenix::render::PbrVertex::read(platform::stream::Stream& f) noexcept
{
    position.read(f);
    normal.read(f);
    tangent.read(f);
    uv.read(f);
}

std::ostream& gearoenix::render::operator<<(std::ostream& os, const gearoenix::render::PbrVertex& v) noexcept
{
    os << R"({ "type": "PbrVertex", "position": )" << v.position
       << ", \"normal\": " << v.normal
       << ", \"tangent\": " << v.tangent
       << ", \"uv\": " << v.uv << "}";
    return os;
}

const void* gearoenix::render::get_data(const render::Vertices& vertices) noexcept
{
    switch (vertices.index()) {
    case 0:
        return std::get<std::vector<PbrVertex>>(vertices).data();
    case 1:
        return std::get<std::vector<PbrVertexAnimated>>(vertices).data();
    default:
        GX_UNEXPECTED;
    }
}

std::size_t gearoenix::render::get_element_size(const render::Vertices& vertices) noexcept
{
    switch (vertices.index()) {
    case 0:
        return sizeof(PbrVertex);
    case 1:
        return sizeof(PbrVertexAnimated);
    default:
        GX_UNEXPECTED;
    }
}

bool gearoenix::render::has_bone_weights(const render::Vertices& vertices) noexcept
{
    switch (vertices.index()) {
    case 0:
        return false;
    case 1:
        return true;
    default:
        GX_UNEXPECTED;
    }
}

bool gearoenix::render::has_bone_indices(const render::Vertices& vertices) noexcept
{
    switch (vertices.index()) {
    case 0:
        return false;
    case 1:
        return true;
    default:
        GX_UNEXPECTED;
    }
}

std::size_t gearoenix::core::count(const render::Vertices& vertices) noexcept
{
    switch (vertices.index()) {
    case 0:
        return std::get<std::vector<render::PbrVertex>>(vertices).size();
    case 1:
        return std::get<std::vector<render::PbrVertexAnimated>>(vertices).size();
    default:
        GX_UNEXPECTED;
    }
}

std::size_t gearoenix::core::bytes_count(const render::Vertices& vertices) noexcept
{
    switch (vertices.index()) {
    case 0:
        return std::get<std::vector<render::PbrVertex>>(vertices).size() * sizeof(render::PbrVertex);
    case 1:
        return std::get<std::vector<render::PbrVertexAnimated>>(vertices).size() * sizeof(render::PbrVertexAnimated);
    default:
        GX_UNEXPECTED;
    }
}