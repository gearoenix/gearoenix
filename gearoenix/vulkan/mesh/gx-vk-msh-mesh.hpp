#pragma once
#include "../../render/gx-rnd-build-configuration.hpp"
#if GX_RENDER_VULKAN_ENABLED
#include "../../core/job/gx-cr-job-end-caller.hpp"
#include "../../render/mesh/gx-rnd-msh-mesh.hpp"

namespace gearoenix::vulkan::material {
struct Material;
}

namespace gearoenix::vulkan::mesh {
struct Buffer;
struct Mesh final : render::mesh::Mesh {
    GX_GET_CREF_PRV(std::shared_ptr<Buffer>, gapi_buffer);
    GX_GET_CREF_PRV(std::shared_ptr<material::Material>, gapi_material);

public:
    Mesh(std::string&& name, std::shared_ptr<render::mesh::Buffer>&& buffer, std::shared_ptr<render::material::Material>&& material);
    static void construct(
        std::string&& name,
        std::shared_ptr<render::mesh::Buffer>&& buffer,
        std::shared_ptr<render::material::Material>&& material,
        const core::job::EndCallerShared<render::mesh::Mesh>& end_callback);
    ~Mesh() override;
    Mesh(const Mesh&) = delete;
};
}

#endif