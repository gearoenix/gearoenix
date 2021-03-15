#ifndef GEAROENIX_VULKAN_MESH_ACCEL_MANAGER_HPP
#define GEAROENIX_VULKAN_MESH_ACCEL_MANAGER_HPP
#include "gx-vk-msh-manager.hpp"
#ifdef GX_RENDER_VULKAN_ENABLED
#include "../gx-vk-loader.hpp"

namespace gearoenix::physics {
struct Transformation;
}

namespace gearoenix::vulkan::buffer {
struct Buffer;
struct Uniform;
}

namespace gearoenix::vulkan::descriptor {
struct Set;
}

namespace gearoenix::vulkan::mesh {
struct Accel;
struct AccelComponent;
struct AccelManager final : public Manager {
private:
    struct MeshData final {
        VkAccelerationStructureInstanceKHR instance;
        VkDescriptorBufferInfo mesh_descriptor_write_info;
        VkDescriptorBufferInfo vertex_descriptor_write_info;
        VkDescriptorBufferInfo index_descriptor_write_info;
    };

    struct Kernel final {
        std::vector<MeshData> mesh_info;
        std::size_t latest_uniform_buffer_for_mesh_info = 0;
        std::vector<std::shared_ptr<buffer::Uniform>> uniform_buffers_for_mesh_descriptors;
        std::vector<std::pair<std::size_t, std::size_t>> material_mesh;
    };

    std::size_t max_instances_size = 0;

    struct Frame final {
        std::shared_ptr<buffer::Buffer> instances_cpu;
        std::shared_ptr<buffer::Buffer> instances_gpu;
        std::shared_ptr<command::Buffer> cmd;
        std::shared_ptr<buffer::Buffer> tlas_buff;
        std::shared_ptr<buffer::Buffer> tlas_scratch_buff;
        VkAccelerationStructureKHR tlas_vulkan_data = nullptr;
        std::shared_ptr<descriptor::Set> descriptor_set;
    };

    std::unique_ptr<core::sync::WorkWaiter> accel_creator;
    std::unique_ptr<core::sync::WorkWaiter> accel_creation_waiter;
    std::vector<Kernel> kernels;
    std::vector<Frame> frames;
    std::vector<VkAccelerationStructureInstanceKHR> instances;
    std::vector<VkDescriptorSetLayoutBinding> descriptor_bindings;
    std::vector<VkDescriptorBufferInfo> mesh_descriptor_write_info;
    std::vector<VkDescriptorBufferInfo> vertex_descriptor_write_info;
    std::vector<VkDescriptorBufferInfo> index_descriptor_write_info;
    std::vector<VkDescriptorBufferInfo> material_descriptor_write_info;
    std::vector<VkDescriptorImageInfo> texture_descriptor_write_info;
    std::vector<VkDescriptorImageInfo> cube_texture_descriptor_write_info;
    std::vector<std::pair<std::size_t, std::size_t>> material_mesh;
    std::vector<std::pair<std::size_t, std::size_t>> texture_material;
    std::vector<std::pair<std::size_t, std::size_t>> cube_texture_material;

    void create_accel(
        const std::string& name,
        const std::vector<math::BasicVertex>& vertices,
        const std::vector<std::uint32_t>& indices,
        core::sync::EndCaller<render::mesh::Mesh>& c) noexcept;
    void create_accel_after_vertices_ready(
        std::string name,
        std::size_t vertices_count,
        std::size_t indices_count,
        core::sync::EndCaller<render::mesh::Mesh> c,
        std::shared_ptr<Accel> result) noexcept;
    void create_accel_after_query_ready(
        std::string name,
        std::shared_ptr<sync::Fence> fence,
        core::sync::EndCaller<render::mesh::Mesh> c,
        std::shared_ptr<Accel> result,
        std::shared_ptr<query::Pool> query_pool) noexcept;
    void create_accel_after_blas_copy(
        core::sync::EndCaller<render::mesh::Mesh> c,
        std::shared_ptr<Accel> result) noexcept;

    void update_instances() noexcept;
    void update_instances_system(
        AccelComponent& accel_com,
        const physics::Transformation& tran,
        unsigned int kernel_index) noexcept;
    void update_instances_buffers() noexcept;

public:
    explicit AccelManager(engine::Engine& e) noexcept;
    ~AccelManager() noexcept final;
    void create(
        const std::string& name,
        const std::vector<math::BasicVertex>& vertices,
        const std::vector<std::uint32_t>& indices,
        core::sync::EndCaller<render::mesh::Mesh>& c) noexcept final;
    void update() noexcept final;
};
}

#endif
#endif