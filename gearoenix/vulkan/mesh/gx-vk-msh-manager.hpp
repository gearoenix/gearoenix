#ifndef GEAROENIX_VULKAN_MESH_MANAGER_HPP
#define GEAROENIX_VULKAN_MESH_MANAGER_HPP
#include "../../render/gx-rnd-build-configuration.hpp"
#ifdef GX_RENDER_VULKAN_ENABLED
#include "../../core/macro/gx-cr-mcr-getter-setter.hpp"
#include "../../core/sync/gx-cr-sync-end-caller.hpp"
#include "../../math/gx-math-vertex.hpp"

namespace gearoenix::core::sync {
struct WorkWaiter;
}

namespace gearoenix::vulkan::command {
struct Buffer;
}

namespace gearoenix::vulkan::engine {
struct Engine;
}

namespace gearoenix::render::mesh {
struct Mesh;
}

namespace gearoenix::vulkan::query {
struct Pool;
}

namespace gearoenix::vulkan::sync {
struct Fence;
}

namespace gearoenix::vulkan::mesh {
struct Accel;
struct Manager {
    constexpr static const auto* const NODE_NAME = "mesh-manager";
    GX_GET_RRF_PRT(engine::Engine, e)

protected:
    explicit Manager(engine::Engine& e) noexcept;

public:
    [[nodiscard]] static std::shared_ptr<Manager> construct(engine::Engine& e) noexcept;
    virtual ~Manager() noexcept;
    virtual void create(
        const std::string& name,
        const std::vector<math::BasicVertex>& vertices,
        const std::vector<std::uint32_t>& indices,
        core::sync::EndCaller<render::mesh::Mesh>& c) noexcept = 0;
    virtual void update() noexcept = 0;
};
}

#endif
#endif