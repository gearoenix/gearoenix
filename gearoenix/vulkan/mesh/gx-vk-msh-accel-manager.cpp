#include "gx-vk-msh-accel-manager.hpp"
#ifdef GX_RENDER_VULKAN_ENABLED
#include "../../core/ecs/gx-cr-ecs-world.hpp"
#include "../../core/gx-cr-allocator.hpp"
#include "../../core/macro/gx-cr-mcr-zeroer.hpp"
#include "../../core/sync/gx-cr-sync-work-waiter.hpp"
#include "../../physics/gx-phs-transformation.hpp"
#include "../buffer/gx-vk-buf-buffer.hpp"
#include "../engine/gx-vk-eng-engine.hpp"
#include "../gx-vk-check.hpp"
#include "../gx-vk-imgui-manager.hpp"
#include "../gx-vk-marker.hpp"
#include "../query/gx-vk-qry-pool.hpp"
#include "../queue/gx-vk-qu-queue.hpp"
#include "../sync/gx-vk-sync-fence.hpp"
#include "gx-vk-msh-accel-component.hpp"
#include "gx-vk-msh-accel.hpp"
#include <thread>

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
    c.set_data(Accel::construct(e, vertices, indices, end));
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
    geo_info.flags = VK_GEOMETRY_OPAQUE_BIT_KHR;

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
    asc_info.type = VK_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL_KHR;

    GX_VK_CHK(vkCreateAccelerationStructureKHR(vk_dev, &asc_info, nullptr, &result->vulkan_data))

    mark(name + "-blas", result->vulkan_data, e.get_logical_device());

    bge_info.dstAccelerationStructure = result->vulkan_data;

    auto scratch_buf = e.get_buffer_manager().create_static(static_cast<std::size_t>(bsz_info.buildScratchSize));
    GX_CHECK_NOT_EQUAL_D(nullptr, scratch_buf)
    const auto scratch_address = scratch_buf->get_device_address();
    bge_info.scratchData.deviceAddress = scratch_address;

    std::shared_ptr<query::Pool> query_pool(new query::Pool(
        dev, VK_QUERY_TYPE_ACCELERATION_STRUCTURE_COMPACTED_SIZE_KHR));

    auto cmd = std::make_shared<command::Buffer>(std::move(cmd_mgr.create(command::Type::Primary)));
    cmd->begin();
    cmd->build_acceleration_structure(bge_info, rng_info);
    cmd->end();
    std::shared_ptr<sync::Fence> fence(new sync::Fence(dev));
    e.get_graphic_queue()->submit(*cmd, *fence);

    accel_creation_waiter->push([this,
                                    cmd = std::move(cmd),
                                    fence = std::move(fence),
                                    c = std::move(c),
                                    result = std::move(result),
                                    query_pool = std::move(query_pool)]() mutable noexcept {
        create_accel_after_blas_ready(
            std::move(cmd), std::move(fence), std::move(c), std::move(result), std::move(query_pool));
    });

    name.clear();
}

void gearoenix::vulkan::mesh::AccelManager::create_accel_after_blas_ready(
    std::shared_ptr<command::Buffer> creation_cmd,
    std::shared_ptr<sync::Fence> fence,
    core::sync::EndCaller<render::mesh::Mesh> c,
    std::shared_ptr<Accel> result,
    std::shared_ptr<query::Pool> query_pool) noexcept
{
    fence->wait();
    auto& dev = e.get_logical_device();
    auto& cmd_mgr = e.get_command_manager();
    auto cmd = std::make_shared<command::Buffer>(std::move(cmd_mgr.create(command::Type::Primary)));
    cmd->begin();
    query_pool->issue_acceleration_structure_compacted_size(*cmd, result->vulkan_data);
    cmd->end();
    fence = std::make_shared<sync::Fence>(dev);
    e.get_graphic_queue()->submit(*cmd, *fence);
    accel_creation_waiter->push([this,
                                    cmd = std::move(cmd),
                                    fence = std::move(fence),
                                    query_pool = std::move(query_pool),
                                    result = std::move(result),
                                    c = std::move(c)]() mutable noexcept {
        create_accel_after_query_ready(
            std::move(cmd), std::move(fence), std::move(c), std::move(result), std::move(query_pool));
    });
    accel_creator->push([cmd = std::move(creation_cmd)] {});
}

void gearoenix::vulkan::mesh::AccelManager::create_accel_after_query_ready(
    std::shared_ptr<command::Buffer> query_cmd,
    std::shared_ptr<sync::Fence> fence,
    core::sync::EndCaller<render::mesh::Mesh> c,
    std::shared_ptr<Accel> result,
    std::shared_ptr<query::Pool> query_pool) noexcept
{
    VkAccelerationStructureKHR old_accel = result->vulkan_data;

    auto& dev = e.get_logical_device();
    auto& cmd_mgr = e.get_command_manager();
    const auto vk_dev = dev.get_vulkan_data();

    fence->wait();
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
    asc_info.type = VK_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL_KHR;

    GX_VK_CHK(vkCreateAccelerationStructureKHR(vk_dev, &asc_info, nullptr, &result->vulkan_data))

    auto cmd = std::make_shared<command::Buffer>(std::move(cmd_mgr.create(command::Type::Primary)));
    cmd->begin();
    VkCopyAccelerationStructureInfoKHR copy_info;
    GX_SET_ZERO(copy_info)
    copy_info.sType = VK_STRUCTURE_TYPE_COPY_ACCELERATION_STRUCTURE_INFO_KHR;
    copy_info.src = old_accel;
    copy_info.dst = result->vulkan_data;
    copy_info.mode = VK_COPY_ACCELERATION_STRUCTURE_MODE_COMPACT_KHR;
    vkCmdCopyAccelerationStructureKHR(cmd->get_vulkan_data(), &copy_info);
    cmd->end();
    fence = std::make_shared<sync::Fence>(dev);
    e.get_graphic_queue()->submit(*cmd, *fence);
    accel_creation_waiter->push([this,
                                    cmd = std::move(cmd),
                                    fence = std::move(fence),
                                    c = std::move(c),
                                    result = std::move(result)]() mutable noexcept {
        fence->wait();
        create_accel_after_blas_copy(std::move(c), std::move(result));
    });
}

void gearoenix::vulkan::mesh::AccelManager::create_accel_after_blas_copy(
    core::sync::EndCaller<render::mesh::Mesh>,
    std::shared_ptr<Accel> result) noexcept
{
    result->initialize_blas();
}

void gearoenix::vulkan::mesh::AccelManager::update_instances() noexcept
{
    e.get_world()->parallel_system<AccelComponent, physics::Transformation>(
        [this](core::ecs::Entity::id_t, AccelComponent& accel_com, physics::Transformation& tran, const unsigned int kernel_index) noexcept {
            update_instances_system(accel_com, tran, kernel_index);
        });
    instances.clear();
    for (auto& kernel : kernels) {
        for (const auto& kernel_instance : kernel.instances) {
            instances.push_back(kernel_instance);
        }
        kernel.instances.clear();
    }
}

void gearoenix::vulkan::mesh::AccelManager::update_instances_system(
    AccelComponent& accel_com,
    const physics::Transformation& tran,
    const unsigned int kernel_index) noexcept
{
    auto& ins = kernels[kernel_index].instances;
    VkAccelerationStructureInstanceKHR info;
    GX_SET_ZERO(info)
    info.accelerationStructureReference = accel_com.get_acceleration_address();
    info.flags = VK_GEOMETRY_INSTANCE_TRIANGLE_FACING_CULL_DISABLE_BIT_KHR;
    const auto transform = tran.get_matrix().transposed();
    std::memcpy(&info.transform, transform.data, sizeof(info.transform));
    ins.push_back(info);
}

void gearoenix::vulkan::mesh::AccelManager::update_instances_buffers() noexcept
{
    auto& frame = frames[e.get_frame_number()];
    auto& dev = e.get_logical_device();
    auto vk_dev = dev.get_vulkan_data();
    frame.instances_size = instances.size() * sizeof(VkAccelerationStructureInstanceKHR);
    const auto& previous_frame = frames[e.get_previous_frame_number()];
    auto& buff_mgr = e.get_buffer_manager();
    if (nullptr != previous_frame.instances_gpu && previous_frame.instances_gpu->get_allocator()->get_size() >= frame.instances_size) {
        frame.instances_gpu = previous_frame.instances_gpu;
    } else {
        frame.instances_gpu = buff_mgr.create_static(frame.instances_size);
    }
    if (nullptr == frame.instances_cpu || frame.instances_cpu->get_allocator()->get_size() < frame.instances_size) {
        frame.instances_cpu = buff_mgr.create_staging(frame.instances_size);
    }
    frame.instances_cpu->write(instances.data(), frame.instances_size);

    frame.cmd->copy(*frame.instances_cpu, *frame.instances_gpu);
    frame.cmd->barrier(
        *frame.instances_gpu,
        { VK_ACCESS_TRANSFER_WRITE_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT },
        { VK_ACCESS_ACCELERATION_STRUCTURE_WRITE_BIT_KHR, VK_PIPELINE_STAGE_ACCELERATION_STRUCTURE_BUILD_BIT_KHR });

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
    bg_info.srcAccelerationStructure = VK_NULL_HANDLE;

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
    mark("tlas-" + std::to_string(e.get_frame_number_from_start()), frame.tlas_vulkan_data, dev);

    bg_info.dstAccelerationStructure = frame.tlas_vulkan_data;
    bg_info.scratchData.deviceAddress = frame.tlas_scratch_buff->get_device_address();

    VkAccelerationStructureBuildRangeInfoKHR bo_info;
    GX_SET_ZERO(bo_info)
    bo_info.primitiveCount = instances_count;

    frame.cmd->build_acceleration_structure(bg_info, bo_info);
}

gearoenix::vulkan::mesh::AccelManager::AccelManager(engine::Engine& e) noexcept
    : Manager(e)
    , accel_creator(new core::sync::WorkWaiter())
    , accel_creation_waiter(new core::sync::WorkWaiter())
    , kernels(std::thread::hardware_concurrency())
    , frames(e.get_swapchain().get_image_views().size())
{
    auto cmds = e.get_graphic_queue()->place_node_between(
        queue::Queue::START_FRAME,
        NODE_NAME,
        VK_PIPELINE_STAGE_ACCELERATION_STRUCTURE_BUILD_BIT_KHR,
        ImGuiManager::NODE_NAME);
    for (std::size_t frame_index = 0; frame_index < frames.size(); ++frame_index) {
        frames[frame_index].cmd = std::move(cmds[frame_index]);
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
    if (!instances.empty())
        update_instances_buffers();
    cmd->end();
}

#endif