#ifndef GEAROENIX_RENDER_MESH_MESH_HPP
#define GEAROENIX_RENDER_MESH_MESH_HPP
#include "../../core/asset/gx-cr-asset.hpp"
#include "../../core/gx-cr-static.hpp"
#include "../../core/gx-cr-types.hpp"
#include "../../core/sync/gx-cr-sync-end-caller.hpp"
#include "../../math/gx-math-aabb.hpp"
#include "../../math/gx-math-vertex.hpp"
#include "gx-rnd-msh-type.hpp"

namespace gearoenix::system::stream {
class Stream;
}

namespace gearoenix::render::engine {
class Engine;
}

namespace gearoenix::render::buffer {
class Buffer;
}

namespace gearoenix::render::mesh {
/// This class right now have only one implementation and in future if it got another
/// implementation it will become an interface (virtual function haver)
class Mesh final : public core::asset::Asset {
    GX_GET_CVAL_PRT(Type, mesh_type)
    GX_GET_CREF_PRT(math::Aabb3, box)
    GX_GET_CREF_PRT(std::shared_ptr<buffer::Buffer>, vertex_buffer)
    GX_GET_CREF_PRT(std::shared_ptr<buffer::Buffer>, index_buffer)
protected:
    Mesh(core::Id my_id, std::string name, Type mesh_type) noexcept;

public:
    Mesh(
        core::Id my_id,
        std::string name,
        system::stream::Stream* f,
        engine::Engine* e,
        const core::sync::EndCaller<core::sync::EndCallerIgnore>& c) noexcept;
    Mesh(
        core::Id my_id,
        std::string name,
        const std::vector<math::BasicVertex>& vertices,
        const std::vector<std::uint32_t>& indices,
        const math::Aabb3& box,
        engine::Engine* e,
        const core::sync::EndCaller<core::sync::EndCallerIgnore>& c) noexcept;
    ~Mesh() noexcept final;
    void set_vertices(
        engine::Engine* e,
        const std::vector<math::BasicVertex>& vertices,
        const std::vector<std::uint32_t>& indices,
        const math::Aabb3& box,
        const core::sync::EndCaller<core::sync::EndCallerIgnore>& c) noexcept;
};
}
#endif
