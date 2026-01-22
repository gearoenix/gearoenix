#pragma once
#include "../../render/gx-rnd-build-configuration.hpp"
#if GX_RENDER_VULKAN_ENABLED
#include "../../render/mesh/gx-rnd-msh-manager.hpp"

namespace gearoenix::vulkan::mesh {
struct Mesh;
struct Manager final : render::mesh::Manager, core::Singleton<Manager> {
    Manager();
    ~Manager() override;
    void build(std::string&& name, render::Vertices&& vertices, std::vector<std::uint32_t>&& indices, const math::Aabb3<double>& occlusion_box, core::job::EndCallerShared<render::mesh::Buffer>&& end_callback) override;
    void build(std::shared_ptr<render::mesh::Buffer>&& buffer, std::shared_ptr<render::material::Material>&& material, core::job::EndCallerShared<render::mesh::Mesh>&& end_callback) override;
};
}

#endif