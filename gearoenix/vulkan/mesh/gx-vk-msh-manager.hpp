#ifndef GEAROENIX_VULKAN_MESH_MANAGER_HPP
#define GEAROENIX_VULKAN_MESH_MANAGER_HPP
#include "../../render/gx-rnd-build-configuration.hpp"
#ifdef GX_RENDER_VULKAN_ENABLED
#include "../../core/macro/gx-cr-mcr-getter-setter.hpp"
#include "../../core/sync/gx-cr-sync-end-caller.hpp"
#include "../../math/gx-math-vertex.hpp"

namespace gearoenix::vulkan::engine {
struct Engine;
}

namespace gearoenix::render::mesh {
struct Mesh;
}

namespace gearoenix::vulkan::mesh {
struct Manager final {
    GX_GET_RRF_PRV(engine::Engine, e)

public:
    explicit Manager(engine::Engine& e) noexcept;
    ~Manager() noexcept;
    void create(
        std::vector<math::BasicVertex> vertices,
        std::vector<std::uint32_t> indices,
        core::sync::EndCaller<render::mesh::Mesh>& c) noexcept;
};
}

#endif
#endif