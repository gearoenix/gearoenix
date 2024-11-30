#ifndef GEAROENIX_VULKAN_MESH_ACCEL_MANAGER_HPP
#define GEAROENIX_VULKAN_MESH_ACCEL_MANAGER_HPP
#include "../../render/gx-rnd-build-configuration.hpp"
#ifdef GX_RENDER_VULKAN_ENABLED
#include "../../render/mesh/gx-rnd-msh-manager.hpp"
#include "../gx-vk-loader.hpp"

namespace gearoenix::core::sync {
struct WorkWaiter;
}

namespace gearoenix::physics {
struct Transformation;
}

namespace gearoenix::vulkan::buffer {
struct Buffer;
struct Uniform;
}

namespace gearoenix::vulkan::command {
struct Buffer;
}

namespace gearoenix::vulkan::descriptor {
struct Set;
}

namespace gearoenix::vulkan::engine {
struct Engine;
}

namespace gearoenix::vulkan::pipeline {
struct Pipeline;
}

namespace gearoenix::vulkan::queue {
struct Queue;
}

namespace gearoenix::vulkan::query {
struct Pool;
}

namespace gearoenix::vulkan::sync {
struct Fence;
}

namespace gearoenix::vulkan::mesh {
struct Mesh;
struct Manager final : public render::mesh::Manager {
    GX_GET_RRF_PRT(engine::Engine, vk_e);

private:
    struct MeshData final {
        VkAccelerationStructureInstanceKHR instance;
        VkDescriptorBufferInfo vertex_descriptor_write_info;
        VkDescriptorBufferInfo index_descriptor_write_info;
        VkDescriptorBufferInfo mesh_descriptor_write_info;
    };

    /// TODO: tlas must move to the camera info
    //    struct Frame final {
    //        std::shared_ptr<buffer::Buffer> instances_cpu;
    //        std::shared_ptr<buffer::Buffer> instances_gpu;
    //        std::shared_ptr<command::Buffer> cmd;
    //        std::shared_ptr<buffer::Buffer> tlas_buff;
    //        std::shared_ptr<buffer::Buffer> tlas_scratch_buff;
    //        VkAccelerationStructureKHR tlas_vulkan_data = nullptr;
    //        std::shared_ptr<descriptor::Set> descriptor_set;
    //        std::shared_ptr<pipeline::Pipeline> pipeline;
    //    };

    std::unique_ptr<core::sync::WorkWaiter> waiter;
    std::unique_ptr<queue::Queue> waiter_queue;
    std::vector<VkAccelerationStructureInstanceKHR> instances;
    std::vector<VkDescriptorSetLayoutBinding> descriptor_bindings;
    std::vector<VkDescriptorBufferInfo> mesh_descriptor_write_info;
    std::vector<VkDescriptorBufferInfo> vertex_descriptor_write_info;
    std::vector<VkDescriptorBufferInfo> index_descriptor_write_info;

    void create_accel_after_vertices_ready(
        std::string&& name,
        std::uint64_t vertices_count,
        std::uint64_t indices_count,
        core::job::EndCaller&& c,
        std::shared_ptr<Mesh>&& result);

    void create_accel_after_query_ready(
        std::string&& name,
        std::shared_ptr<sync::Fence>&& fence,
        core::job::EndCaller&& c,
        std::shared_ptr<Mesh>&& result,
        std::shared_ptr<query::Pool>&& query_pool);

    void create_accel_after_blas_copy(
        core::job::EndCaller&& c,
        std::shared_ptr<Mesh>&& result);

    [[nodiscard]] std::shared_ptr<render::mesh::Mesh> build(
        std::string&& name,
        render::Vertices&& vertices,
        std::vector<std::uint32_t>&& indices,
        math::Aabb3<double>&& occlusion_box,
        const core::job::EndCaller& end_callback) final;

public:
    explicit Manager(engine::Engine& e);
    ~Manager() final;
    void update();
};
}

#endif
#endif