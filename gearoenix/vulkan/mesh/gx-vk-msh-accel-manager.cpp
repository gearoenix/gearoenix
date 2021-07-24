#include "gx-vk-msh-accel-manager.hpp"
#ifdef GX_RENDER_VULKAN_ENABLED
#include "../../core/ecs/gx-cr-ecs-world.hpp"
#include "../../core/gx-cr-allocator.hpp"
#include "../../core/macro/gx-cr-mcr-zeroer.hpp"
#include "../../core/sync/gx-cr-sync-work-waiter.hpp"
#include "../../physics/gx-phs-transformation.hpp"
#include "../buffer/gx-vk-buf-buffer.hpp"
#include "../descriptor/gx-vk-des-bindings-data.hpp"
#include "../descriptor/gx-vk-des-set.hpp"
#include "../engine/gx-vk-eng-engine.hpp"
#include "../gx-vk-check.hpp"
#include "../gx-vk-framebuffer.hpp"
#include "../gx-vk-imgui-manager.hpp"
#include "../gx-vk-marker.hpp"
#include "../query/gx-vk-qry-pool.hpp"
#include "../queue/gx-vk-qu-queue.hpp"
#include "../shader/gx-vk-shd-bindings.hpp"
#include "../sync/gx-vk-sync-fence.hpp"
#include "gx-vk-msh-accel-component.hpp"
#include "gx-vk-msh-accel.hpp"
#include <thread>

constexpr static const std::size_t default_init_mesh_descriptor_count = 200;
constexpr static const std::size_t default_init_material_descriptor_count = 100;
constexpr static const std::size_t default_init_2d_texture_descriptor_count = 500;
constexpr static const std::size_t default_init_cube_texture_descriptor_count = 10;

void gearoenix::vulkan::mesh::AccelManager::create_accel(
    const std::string& name,
    const std::vector<math::BasicVertex>& vertices,
    const std::vector<std::uint32_t>& indices,
    core::sync::EndCaller<render::mesh::Mesh>& c) noexcept
{
    core::sync::EndCaller<Accel> end([this,
                                         name = name,
                                         c = c,
                                         vertices_count = vertices.size(),
                                         indices_count = indices.size()](const auto& result) mutable noexcept {
        accel_creator->push([this,
                                name = std::move(name),
                                c = std::move(c),
                                vertices_count,
                                indices_count,
                                result]() mutable noexcept {
            create_accel_after_vertices_ready(std::move(name), vertices_count, indices_count, std::move(c), std::move(result));
        });
    });
    c.set_data(Accel::construct(e, name, vertices, indices, end));
}

void gearoenix::vulkan::mesh::AccelManager::create_accel_after_vertices_ready(
    std::string name,
    const std::size_t vertices_count,
    const std::size_t indices_count,
    core::sync::EndCaller<render::mesh::Mesh> c,
    std::shared_ptr<Accel> result) noexcept
{
    auto& dev = e.get_logical_device();
    auto& cmd_mgr = e.get_command_manager();
    const auto vk_dev = dev.get_vulkan_data();

    const auto [vba, iba] = result->get_buffers_address();

    VkAccelerationStructureGeometryKHR geo_info;
    GX_SET_ZERO(geo_info)
    geo_info.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_KHR;
    geo_info.geometryType = VK_GEOMETRY_TYPE_TRIANGLES_KHR;
    geo_info.flags = VK_GEOMETRY_OPAQUE_BIT_KHR | VK_GEOMETRY_NO_DUPLICATE_ANY_HIT_INVOCATION_BIT_KHR;

    auto& trs_info = geo_info.geometry.triangles;
    trs_info.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_TRIANGLES_DATA_KHR;
    trs_info.indexData.deviceAddress = iba;
    trs_info.indexType = VK_INDEX_TYPE_UINT32;
    trs_info.maxVertex = static_cast<std::uint32_t>(vertices_count);
    trs_info.vertexData.deviceAddress = vba;
    trs_info.vertexFormat = VK_FORMAT_R32G32B32_SFLOAT;
    trs_info.vertexStride = sizeof(math::BasicVertex);

    VkAccelerationStructureBuildRangeInfoKHR rng_info;
    GX_SET_ZERO(rng_info)
    rng_info.primitiveCount = static_cast<std::uint32_t>(indices_count / 3);

    VkAccelerationStructureBuildGeometryInfoKHR bge_info;
    GX_SET_ZERO(bge_info)
    bge_info.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_BUILD_GEOMETRY_INFO_KHR;
    bge_info.flags = VK_BUILD_ACCELERATION_STRUCTURE_PREFER_FAST_TRACE_BIT_KHR | VK_BUILD_ACCELERATION_STRUCTURE_ALLOW_COMPACTION_BIT_KHR;
    bge_info.geometryCount = 1;
    bge_info.pGeometries = &geo_info;
    bge_info.mode = VK_BUILD_ACCELERATION_STRUCTURE_MODE_BUILD_KHR;
    bge_info.type = VK_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL_KHR;

    VkAccelerationStructureBuildSizesInfoKHR bsz_info;
    GX_SET_ZERO(bsz_info)
    bsz_info.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_BUILD_SIZES_INFO_KHR;
    vkGetAccelerationStructureBuildSizesKHR(
        vk_dev,
        VK_ACCELERATION_STRUCTURE_BUILD_TYPE_DEVICE_KHR,
        &bge_info, &rng_info.primitiveCount, &bsz_info);

    result->accel_buff = std::move(e.get_buffer_manager().create_static(static_cast<std::size_t>(bsz_info.accelerationStructureSize)));
    GX_CHECK_NOT_EQUAL_D(nullptr, result->accel_buff)

    VkAccelerationStructureCreateInfoKHR asc_info;
    GX_SET_ZERO(asc_info)
    asc_info.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_CREATE_INFO_KHR;
    asc_info.type = VK_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL_KHR;
    asc_info.size = bsz_info.accelerationStructureSize;
    asc_info.buffer = result->accel_buff->get_vulkan_data();
    asc_info.offset = result->accel_buff->get_allocator()->get_offset();

    GX_VK_CHK(vkCreateAccelerationStructureKHR(vk_dev, &asc_info, nullptr, &result->vulkan_data))
    GX_VK_MARK(name + "-blas-temp", result->vulkan_data, e.get_logical_device())

    bge_info.dstAccelerationStructure = result->vulkan_data;

    auto scratch_buf = e.get_buffer_manager().create_static(static_cast<std::size_t>(bsz_info.buildScratchSize));
    GX_CHECK_NOT_EQUAL_D(nullptr, scratch_buf)
    const auto scratch_address = scratch_buf->get_device_address();
    bge_info.scratchData.deviceAddress = scratch_address;

    std::shared_ptr<query::Pool> query_pool(new query::Pool(
        dev, VK_QUERY_TYPE_ACCELERATION_STRUCTURE_COMPACTED_SIZE_KHR));

    auto cmd = std::make_shared<command::Buffer>(std::move(cmd_mgr.create(command::Type::Primary)));
    GX_VK_MARK(name + "-blas-temp-cmd", cmd->get_vulkan_data(), e.get_logical_device())
    cmd->begin();
    const auto* const rng_info_p = &rng_info;
    const auto* const* const rng_info_pp = &rng_info_p;
    cmd->build_acceleration_structure(bge_info, rng_info_pp);
    cmd->barrier(*result->accel_buff,
        { VK_ACCESS_ACCELERATION_STRUCTURE_WRITE_BIT_KHR, VK_PIPELINE_STAGE_ACCELERATION_STRUCTURE_BUILD_BIT_KHR },
        { VK_ACCESS_ACCELERATION_STRUCTURE_READ_BIT_KHR, VK_PIPELINE_STAGE_ACCELERATION_STRUCTURE_BUILD_BIT_KHR });
    query_pool->issue_acceleration_structure_compacted_size(*cmd, result->vulkan_data);
    cmd->end();
    std::shared_ptr<sync::Fence> fence(new sync::Fence(dev));
    e.get_graphic_queue()->submit(*cmd, *fence);

    accel_creation_waiter->push([this,
                                    name = std::move(name),
                                    cmd = std::move(cmd),
                                    fence = std::move(fence),
                                    c = std::move(c),
                                    result = std::move(result),
                                    query_pool = std::move(query_pool)]() mutable noexcept {
        create_accel_after_query_ready(std::move(name), std::move(fence), std::move(c), std::move(result), std::move(query_pool));
        accel_creator->push([cmd = std::move(cmd)] {});
    });
}

void gearoenix::vulkan::mesh::AccelManager::create_accel_after_query_ready(
    std::string name,
    std::shared_ptr<sync::Fence> fence,
    core::sync::EndCaller<render::mesh::Mesh> c,
    std::shared_ptr<Accel> result,
    std::shared_ptr<query::Pool> query_pool) noexcept
{
    VkAccelerationStructureKHR old_accel = result->vulkan_data;
    auto old_accel_buff = std::move(result->accel_buff);

    auto& dev = e.get_logical_device();
    auto& cmd_mgr = e.get_command_manager();
    const auto vk_dev = dev.get_vulkan_data();

    fence->wait();
    fence->reset();

    const auto compact_size = query_pool->get_acceleration_structure_compacted_size();

    result->accel_buff = std::move(e.get_buffer_manager().create_static(static_cast<std::size_t>(compact_size)));
    GX_CHECK_NOT_EQUAL_D(nullptr, result->accel_buff)

    VkAccelerationStructureCreateInfoKHR asc_info;
    GX_SET_ZERO(asc_info)
    asc_info.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_CREATE_INFO_KHR;
    asc_info.type = VK_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL_KHR;
    asc_info.size = compact_size;
    asc_info.buffer = result->accel_buff->get_vulkan_data();
    asc_info.offset = result->accel_buff->get_allocator()->get_offset();

    GX_VK_CHK(vkCreateAccelerationStructureKHR(vk_dev, &asc_info, nullptr, &result->vulkan_data))
    GX_VK_MARK(name + "-blas", result->vulkan_data, e.get_logical_device())

    auto cmd = std::make_shared<command::Buffer>(std::move(cmd_mgr.create(command::Type::Primary)));
    GX_VK_MARK(name + "-blas-cmd", cmd->get_vulkan_data(), e.get_logical_device())
    cmd->begin();
    VkCopyAccelerationStructureInfoKHR copy_info;
    GX_SET_ZERO(copy_info)
    copy_info.sType = VK_STRUCTURE_TYPE_COPY_ACCELERATION_STRUCTURE_INFO_KHR;
    copy_info.src = old_accel;
    copy_info.dst = result->vulkan_data;
    copy_info.mode = VK_COPY_ACCELERATION_STRUCTURE_MODE_COMPACT_KHR;
    vkCmdCopyAccelerationStructureKHR(cmd->get_vulkan_data(), &copy_info);
    cmd->end();

    e.get_graphic_queue()->submit(*cmd, *fence);
    accel_creation_waiter->push([this,
                                    old_accel = old_accel,
                                    old_accel_buff = std::move(old_accel_buff),
                                    vk_dev = vk_dev,
                                    cmd = std::move(cmd),
                                    fence = std::move(fence),
                                    c = std::move(c),
                                    result = std::move(result)]() mutable noexcept {
        fence->wait();
        vkDestroyAccelerationStructureKHR(vk_dev, old_accel, nullptr);
        create_accel_after_blas_copy(std::move(c), std::move(result));
    });

    name.clear();
}

void gearoenix::vulkan::mesh::AccelManager::create_accel_after_blas_copy(
    core::sync::EndCaller<render::mesh::Mesh>,
    std::shared_ptr<Accel> result) noexcept
{
    result->initialize_blas();
}

// This is mainly for performance reason
static VkBuffer gpu_buffer_vulkan_data = nullptr;

void gearoenix::vulkan::mesh::AccelManager::update_instances() noexcept
{
    gpu_buffer_vulkan_data = e.get_buffer_manager().get_gpu_root_buffer()->get_vulkan_data();
    e.get_world()->parallel_system<AccelComponent, physics::Transformation>(
        [this](core::ecs::Entity::id_t, AccelComponent& accel_com, physics::Transformation& tran, const unsigned int kernel_index) noexcept {
            update_instances_system(accel_com, tran, kernel_index);
        });
    instances.clear();
    for (auto& kernel : kernels) {
        auto& kernel_meshes_info = kernel.mesh_info;
        for (const auto& kernel_mesh_info : kernel_meshes_info) {
            instances.push_back(kernel_mesh_info.instance);
        }
    }
    if (instances.size() > mesh_descriptor_write_info.size()) {
        // TODO: update bindings info
        // TODO: new descriptor set must be created for this frame and next frames
        // TODO: (int frames_needs_new_des_set = frames_count)
        // TODO: a better increase strategy and remove following lines
        mesh_descriptor_write_info.resize(instances.size());
        vertex_descriptor_write_info.resize(instances.size());
        index_descriptor_write_info.resize(instances.size());
    }
    std::size_t index = 0;
    for (auto& kernel : kernels) {
        auto& kernel_meshes_info = kernel.mesh_info;
        for (const auto& kernel_mesh_info : kernel_meshes_info) {
            mesh_descriptor_write_info[index] = kernel_mesh_info.mesh_descriptor_write_info;
            vertex_descriptor_write_info[index] = kernel_mesh_info.vertex_descriptor_write_info;
            index_descriptor_write_info[index] = kernel_mesh_info.index_descriptor_write_info;
            ++index;
            // TODO: fill the mesh_descriptor_write_info
            // TODO: fill uniform buffers of des_descriptors
            // TODO: fill material to mesh vector
            // TODO: sort material to mesh vector
        }
    }
    const auto max_dwi_count = static_cast<std::size_t>(descriptor_bindings[GX_VK_BIND_RAY_VERTICES].descriptorCount);
    for (; index < max_dwi_count; ++index) {
        GX_SET_ZERO(mesh_descriptor_write_info[index])
        GX_SET_ZERO(vertex_descriptor_write_info[index])
        GX_SET_ZERO(index_descriptor_write_info[index])
    }
    for (auto& kernel : kernels) {
        kernel.mesh_info.clear();
    }
}

void gearoenix::vulkan::mesh::AccelManager::update_instances_system(
    AccelComponent& accel_com,
    const physics::Transformation& tran,
    const unsigned int kernel_index) noexcept
{
    auto& meshes_info = kernels[kernel_index].mesh_info;
    meshes_info.emplace_back();
    auto& mesh_info = meshes_info.back();
    auto& info = mesh_info.instance;
    GX_SET_ZERO(info)
    info.accelerationStructureReference = accel_com.get_acceleration_address();
    info.flags = VK_GEOMETRY_INSTANCE_TRIANGLE_FACING_CULL_DISABLE_BIT_KHR;
    auto& matrix = info.transform.matrix;
    for (int i = 0; i < 3; ++i)
        for (int j = 0; j < 4; ++j)
            matrix[i][j] = static_cast<float>(tran.get_matrix().data[j][i]);
    auto& vertex_info = mesh_info.vertex_descriptor_write_info;
    vertex_info.offset = accel_com.get_vertex_buffer_offset();
    vertex_info.range = accel_com.get_vertex_buffer_size();
    vertex_info.buffer = gpu_buffer_vulkan_data;
    auto& index_info = mesh_info.index_descriptor_write_info;
    index_info.offset = accel_com.get_index_buffer_offset();
    index_info.range = accel_com.get_index_buffer_size();
    index_info.buffer = gpu_buffer_vulkan_data;
    // TODO: remove following line
    GX_SET_ZERO(mesh_info.mesh_descriptor_write_info)
    // TODO: fill the mesh_descriptor_write_info
    // TODO: fill uniform buffers of des_descriptors
    // TODO: fill material to mesh vector
    // TODO: sort material to mesh vector
}

void gearoenix::vulkan::mesh::AccelManager::update_instances_buffers() noexcept
{
    if (instances.empty())
        return;

    auto& frame = frames[e.get_frame_number()];
    auto& dev = e.get_logical_device();
    auto vk_dev = dev.get_vulkan_data();
    const auto instances_size = instances.size() * sizeof(VkAccelerationStructureInstanceKHR);
    const bool needs_new_instances_buffer = (max_instances_size < instances_size);
    max_instances_size = needs_new_instances_buffer ? instances_size : max_instances_size;
    const auto& previous_frame = frames[e.get_previous_frame_number()];
    auto& buff_mgr = e.get_buffer_manager();
    if (nullptr == previous_frame.instances_gpu || needs_new_instances_buffer) {
        frame.instances_gpu = buff_mgr.create_static(instances_size);
    } else {
        frame.instances_gpu = previous_frame.instances_gpu;
    }
    if (nullptr == frame.instances_cpu || needs_new_instances_buffer) {
        frame.instances_cpu = buff_mgr.create_staging(instances_size);
    }
    frame.instances_cpu->write(instances.data(), instances_size);

    frame.cmd->copy(*frame.instances_cpu, *frame.instances_gpu);
    frame.cmd->barrier(
        *frame.instances_gpu,
        { VK_ACCESS_TRANSFER_WRITE_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT },
        { VK_ACCESS_ACCELERATION_STRUCTURE_READ_BIT_KHR, VK_PIPELINE_STAGE_ACCELERATION_STRUCTURE_BUILD_BIT_KHR });

    const auto instances_device_address = frame.instances_gpu->get_device_address();

    VkAccelerationStructureGeometryInstancesDataKHR instances_vk;
    GX_SET_ZERO(instances_vk)
    instances_vk.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_INSTANCES_DATA_KHR;
    instances_vk.arrayOfPointers = VK_FALSE;
    instances_vk.data.deviceAddress = instances_device_address;

    VkAccelerationStructureGeometryKHR tlas_geo;
    GX_SET_ZERO(tlas_geo)
    tlas_geo.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_KHR;
    tlas_geo.geometryType = VK_GEOMETRY_TYPE_INSTANCES_KHR;
    tlas_geo.geometry.instances = instances_vk;

    VkAccelerationStructureBuildGeometryInfoKHR bg_info;
    GX_SET_ZERO(bg_info)
    bg_info.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_BUILD_GEOMETRY_INFO_KHR;
    bg_info.flags = VK_BUILD_ACCELERATION_STRUCTURE_PREFER_FAST_TRACE_BIT_KHR;
    bg_info.geometryCount = 1;
    bg_info.pGeometries = &tlas_geo;
    bg_info.mode = VK_BUILD_ACCELERATION_STRUCTURE_MODE_BUILD_KHR;
    bg_info.type = VK_ACCELERATION_STRUCTURE_TYPE_TOP_LEVEL_KHR;

    const auto instances_count = static_cast<std::uint32_t>(instances.size());
    VkAccelerationStructureBuildSizesInfoKHR sz_info;
    GX_SET_ZERO(sz_info)
    sz_info.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_BUILD_SIZES_INFO_KHR;
    vkGetAccelerationStructureBuildSizesKHR(
        vk_dev, VK_ACCELERATION_STRUCTURE_BUILD_TYPE_DEVICE_KHR, &bg_info, &instances_count, &sz_info);

    if (nullptr != frame.tlas_vulkan_data) {
        vkDestroyAccelerationStructureKHR(vk_dev, frame.tlas_vulkan_data, nullptr);
        frame.tlas_vulkan_data = nullptr;
    }

    if (nullptr != previous_frame.tlas_buff && previous_frame.tlas_buff->get_allocator()->get_size() >= sz_info.accelerationStructureSize) {
        frame.tlas_buff = previous_frame.tlas_buff;
    } else {
        frame.tlas_buff = buff_mgr.create_static(sz_info.accelerationStructureSize);
    }

    if (nullptr != previous_frame.tlas_scratch_buff && previous_frame.tlas_scratch_buff->get_allocator()->get_size() >= sz_info.buildScratchSize) {
        frame.tlas_scratch_buff = previous_frame.tlas_scratch_buff;
    } else {
        frame.tlas_scratch_buff = buff_mgr.create_static(sz_info.buildScratchSize);
    }

    VkAccelerationStructureCreateInfoKHR create_info;
    GX_SET_ZERO(create_info)
    create_info.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_CREATE_INFO_KHR;
    create_info.type = VK_ACCELERATION_STRUCTURE_TYPE_TOP_LEVEL_KHR;
    create_info.size = sz_info.accelerationStructureSize;
    create_info.buffer = frame.tlas_buff->get_vulkan_data();
    create_info.offset = frame.tlas_buff->get_allocator()->get_offset();

    GX_VK_CHK(vkCreateAccelerationStructureKHR(vk_dev, &create_info, nullptr, &frame.tlas_vulkan_data))
    GX_VK_MARK("tlas-" + std::to_string(e.get_frame_number_from_start()), frame.tlas_vulkan_data, dev)

    bg_info.dstAccelerationStructure = frame.tlas_vulkan_data;
    bg_info.scratchData.deviceAddress = frame.tlas_scratch_buff->get_device_address();

    VkAccelerationStructureBuildRangeInfoKHR rng_info;
    GX_SET_ZERO(rng_info)
    rng_info.primitiveCount = instances_count;

    const auto* const rng_info_p = &rng_info;
    const auto* const* const rng_info_pp = &rng_info_p;
    frame.cmd->build_acceleration_structure(bg_info, rng_info_pp);

    // TODO: check if the descriptor bindings needs any change (max numbers)
    // TODO: create a new descriptor set based on the numbers of meshes materials and textures

    auto descriptor_set = frame.descriptor_set->get_vulkan_data();
    VkWriteDescriptorSet des_write_info[GX_VK_BIND_RAY_MAX];
    GX_SET_ARRAY_ZERO(des_write_info)
    for (auto i = 0; i < GX_VK_BIND_RAY_MAX; ++i) {
        auto& wi = des_write_info[i];
        wi.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        const auto& b = descriptor_bindings[i];
        wi.descriptorCount = b.descriptorType;
        wi.descriptorType = b.descriptorType;
        wi.dstBinding = b.binding;
        wi.dstSet = descriptor_set;
    }

    des_write_info[GX_VK_BIND_RAY_VERTICES].pBufferInfo = vertex_descriptor_write_info.data();
    des_write_info[GX_VK_BIND_RAY_INDICES].pBufferInfo = index_descriptor_write_info.data();
    des_write_info[GX_VK_BIND_RAY_MESH].pBufferInfo = mesh_descriptor_write_info.data();
    des_write_info[GX_VK_BIND_RAY_MATERIALS].pBufferInfo = material_descriptor_write_info.data();
    des_write_info[GX_VK_BIND_RAY_2D_TEXTURES].pImageInfo = texture_descriptor_write_info.data();
    des_write_info[GX_VK_BIND_RAY_CUBE_TEXTURES].pImageInfo = cube_texture_descriptor_write_info.data();

    VkWriteDescriptorSetAccelerationStructureKHR tlas_wdi;
    GX_SET_ZERO(tlas_wdi)
    tlas_wdi.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET_ACCELERATION_STRUCTURE_KHR;
    tlas_wdi.accelerationStructureCount = 1;
    tlas_wdi.pAccelerationStructures = &frame.tlas_vulkan_data;
    des_write_info[GX_VK_BIND_RAY_TLAS].pNext = &tlas_wdi;

    VkDescriptorImageInfo out_img_wdi;
    GX_SET_ZERO(out_img_wdi)
    out_img_wdi.imageLayout = VK_IMAGE_LAYOUT_GENERAL;
    out_img_wdi.imageView = e.get_current_framebuffer().get_view()->get_vulkan_data();
    des_write_info[GX_VK_BIND_RAY_OUT_IMAGE].pImageInfo = &out_img_wdi;

    frame.descriptor_set->write(des_write_info, GX_VK_BIND_RAY_MAX);
}

gearoenix::vulkan::mesh::AccelManager::AccelManager(engine::Engine& e) noexcept
    : Manager(e)
    , accel_creator(new core::sync::WorkWaiter())
    , accel_creation_waiter(new core::sync::WorkWaiter())
    , kernels(std::thread::hardware_concurrency())
    , frames(e.get_swapchain().get_image_views().size())
    , descriptor_bindings(GX_VK_BIND_RAY_MAX)
    , mesh_descriptor_write_info(default_init_mesh_descriptor_count)
    , vertex_descriptor_write_info(default_init_mesh_descriptor_count)
    , index_descriptor_write_info(default_init_mesh_descriptor_count)
    , material_descriptor_write_info(default_init_material_descriptor_count)
    , texture_descriptor_write_info(default_init_2d_texture_descriptor_count)
    , cube_texture_descriptor_write_info(default_init_cube_texture_descriptor_count)
{
    GX_SET_VECTOR_ZERO(mesh_descriptor_write_info)
    GX_SET_VECTOR_ZERO(vertex_descriptor_write_info)
    GX_SET_VECTOR_ZERO(index_descriptor_write_info)
    GX_SET_VECTOR_ZERO(material_descriptor_write_info)
    GX_SET_VECTOR_ZERO(texture_descriptor_write_info)
    GX_SET_VECTOR_ZERO(cube_texture_descriptor_write_info)

    auto& vertices_bindings = descriptor_bindings[GX_VK_BIND_RAY_VERTICES];
    GX_SET_ZERO(vertices_bindings)
    vertices_bindings.binding = GX_VK_BIND_RAY_VERTICES;
    vertices_bindings.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    vertices_bindings.descriptorCount = default_init_mesh_descriptor_count;
    vertices_bindings.stageFlags = VK_SHADER_STAGE_RAYGEN_BIT_KHR | VK_SHADER_STAGE_ANY_HIT_BIT_KHR;

    auto& indices_bindings = descriptor_bindings[GX_VK_BIND_RAY_INDICES];
    GX_SET_ZERO(indices_bindings)
    indices_bindings.binding = GX_VK_BIND_RAY_INDICES;
    indices_bindings.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    indices_bindings.descriptorCount = default_init_mesh_descriptor_count;
    indices_bindings.stageFlags = VK_SHADER_STAGE_RAYGEN_BIT_KHR | VK_SHADER_STAGE_ANY_HIT_BIT_KHR;

    auto& materials_bindings = descriptor_bindings[GX_VK_BIND_RAY_MATERIALS];
    GX_SET_ZERO(materials_bindings)
    materials_bindings.binding = GX_VK_BIND_RAY_MATERIALS;
    materials_bindings.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    materials_bindings.descriptorCount = default_init_material_descriptor_count;
    materials_bindings.stageFlags = VK_SHADER_STAGE_RAYGEN_BIT_KHR | VK_SHADER_STAGE_ANY_HIT_BIT_KHR;

    auto& meshes_bindings = descriptor_bindings[GX_VK_BIND_RAY_MESH];
    GX_SET_ZERO(meshes_bindings)
    meshes_bindings.binding = GX_VK_BIND_RAY_MESH;
    meshes_bindings.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    meshes_bindings.descriptorCount = default_init_mesh_descriptor_count;
    meshes_bindings.stageFlags = VK_SHADER_STAGE_RAYGEN_BIT_KHR | VK_SHADER_STAGE_ANY_HIT_BIT_KHR;

    auto& tlas_bindings = descriptor_bindings[GX_VK_BIND_RAY_TLAS];
    GX_SET_ZERO(tlas_bindings)
    tlas_bindings.binding = GX_VK_BIND_RAY_TLAS;
    tlas_bindings.descriptorType = VK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_KHR;
    tlas_bindings.descriptorCount = 1;
    tlas_bindings.stageFlags = VK_SHADER_STAGE_RAYGEN_BIT_KHR | VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR;

    auto& textures_bindings = descriptor_bindings[GX_VK_BIND_RAY_2D_TEXTURES];
    GX_SET_ZERO(textures_bindings)
    textures_bindings.binding = GX_VK_BIND_RAY_2D_TEXTURES;
    textures_bindings.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    textures_bindings.descriptorCount = default_init_2d_texture_descriptor_count;
    textures_bindings.stageFlags = VK_SHADER_STAGE_RAYGEN_BIT_KHR | VK_SHADER_STAGE_ANY_HIT_BIT_KHR;

    auto& cube_textures_bindings = descriptor_bindings[GX_VK_BIND_RAY_CUBE_TEXTURES];
    GX_SET_ZERO(cube_textures_bindings)
    cube_textures_bindings.binding = GX_VK_BIND_RAY_CUBE_TEXTURES;
    cube_textures_bindings.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    cube_textures_bindings.descriptorCount = default_init_cube_texture_descriptor_count;
    cube_textures_bindings.stageFlags = VK_SHADER_STAGE_RAYGEN_BIT_KHR | VK_SHADER_STAGE_ANY_HIT_BIT_KHR;

    auto& out_img_bindings = descriptor_bindings[GX_VK_BIND_RAY_OUT_IMAGE];
    GX_SET_ZERO(out_img_bindings)
    out_img_bindings.binding = GX_VK_BIND_RAY_OUT_IMAGE;
    out_img_bindings.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
    out_img_bindings.descriptorCount = 1;
    out_img_bindings.stageFlags = VK_SHADER_STAGE_RAYGEN_BIT_KHR | VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR;

    auto cmds = e.get_graphic_queue()->place_node_between(
        queue::Queue::START_FRAME,
        NODE_NAME,
        VK_PIPELINE_STAGE_ACCELERATION_STRUCTURE_BUILD_BIT_KHR,
        ImGuiManager::NODE_NAME);
    auto& des_mgr = e.get_descriptor_manager();
    for (std::size_t frame_index = 0; frame_index < frames.size(); ++frame_index) {
        auto& frame = frames[frame_index];
        frame.cmd = std::move(cmds[frame_index]);
        frame.tlas_vulkan_data = nullptr;
        frame.descriptor_set = des_mgr.create_set(descriptor_bindings);
        frame.pipeline = e.get_pipeline_manager().create_ray_tracing_pbr(des_mgr.get_bindings_data(descriptor_bindings)->get_layout());
    }
}

gearoenix::vulkan::mesh::AccelManager::~AccelManager() noexcept = default;

void gearoenix::vulkan::mesh::AccelManager::create(
    const std::string& name,
    const std::vector<math::BasicVertex>& vertices,
    const std::vector<std::uint32_t>& indices,
    core::sync::EndCaller<render::mesh::Mesh>& c) noexcept
{
    create_accel(name, vertices, indices, c);
}

void gearoenix::vulkan::mesh::AccelManager::update() noexcept
{
    update_instances();
    auto& frame = frames[e.get_frame_number()];
    auto& cmd = frame.cmd;
    cmd->begin();
    update_instances_buffers();
    cmd->end();
}

#endif