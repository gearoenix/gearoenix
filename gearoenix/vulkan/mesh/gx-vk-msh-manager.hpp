#ifndef GEAROENIX_VULKAN_MESH_MANAGER_HPP
#define GEAROENIX_VULKAN_MESH_MANAGER_HPP
#include "../../render/gx-rnd-build-configuration.hpp"
#ifdef GX_RENDER_VULKAN_ENABLED
#include "../../render/mesh/gx-rnd-msh-manager.hpp"

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
struct Manager : public render::mesh::Manager {
    constexpr static const char SUBMIT_NODE_NAME[] = "mesh-manager";
    GX_GET_RRF_PRT(engine::Engine, vk_e)

protected:
    explicit Manager(engine::Engine& e) noexcept;

public:
    [[nodiscard]] static std::shared_ptr<Manager> construct(engine::Engine& e) noexcept;
    ~Manager() noexcept override = default;
    virtual void update() noexcept = 0;
};
}

#endif
#endif