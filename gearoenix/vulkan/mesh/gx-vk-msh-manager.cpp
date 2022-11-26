#include "gx-vk-msh-manager.hpp"
#ifdef GX_RENDER_VULKAN_ENABLED
#include "../../core/allocator/gx-cr-alc-range.hpp"
#include "../../core/ecs/gx-cr-ecs-world.hpp"
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
#include "gx-vk-msh-mesh.hpp"
#include <memory>

constexpr static const std::size_t default_init_mesh_descriptor_count = 200;
constexpr static const std::size_t default_init_material_descriptor_count = 100;
constexpr static const std::size_t default_init_2d_texture_descriptor_count = 500;
constexpr static const std::size_t default_init_cube_texture_descriptor_count = 10;

void gearoenix::vulkan::mesh::Manager::create_accel_after_vertices_ready(
    std::string&& name,
    const std::size_t vertices_count,
    const std::size_t indices_count,
    core::sync::EndCaller&& c,
    std::shared_ptr<Mesh>&& result) noexcept
{
    auto& dev = vk_e.get_logical_device();
    auto& cmd_mgr = vk_e.get_command_manager();
    const auto vk_dev = dev.get_vulkan_data();

    const auto [vba, iba] = result->get_buffers_address();

    VkAccelerationStructureGeometryKHR geo_info;
    GX_SET_ZERO(geo_info);
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
    trs_info.vertexStride = sizeof(render::PbrVertex);

    VkAccelerationStructureBuildRangeInfoKHR rng_info;
    GX_SET_ZERO(rng_info);
    rng_info.primitiveCount = static_cast<std::uint32_t>(indices_count / 3);

    VkAccelerationStructureBuildGeometryInfoKHR bge_info;
    GX_SET_ZERO(bge_info);
    bge_info.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_BUILD_GEOMETRY_INFO_KHR;
    bge_info.flags = VK_BUILD_ACCELERATION_STRUCTURE_PREFER_FAST_TRACE_BIT_KHR | VK_BUILD_ACCELERATION_STRUCTURE_ALLOW_COMPACTION_BIT_KHR;
    bge_info.geometryCount = 1;
    bge_info.pGeometries = &geo_info;
    bge_info.mode = VK_BUILD_ACCELERATION_STRUCTURE_MODE_BUILD_KHR;
    bge_info.type = VK_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL_KHR;

    VkAccelerationStructureBuildSizesInfoKHR bsz_info;
    GX_SET_ZERO(bsz_info);
    bsz_info.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_BUILD_SIZES_INFO_KHR;
    vkGetAccelerationStructureBuildSizesKHR(
        vk_dev,
        VK_ACCELERATION_STRUCTURE_BUILD_TYPE_DEVICE_KHR,
        &bge_info, &rng_info.primitiveCount, &bsz_info);

    result->accel_buff = std::move(vk_e.get_buffer_manager().create_static(static_cast<std::size_t>(bsz_info.accelerationStructureSize)));
    GX_CHECK_NOT_EQUAL_D(nullptr, result->accel_buff);

    VkAccelerationStructureCreateInfoKHR asc_info;
    GX_SET_ZERO(asc_info);
    asc_info.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_CREATE_INFO_KHR;
    asc_info.type = VK_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL_KHR;
    asc_info.size = bsz_info.accelerationStructureSize;
    asc_info.buffer = result->accel_buff->get_vulkan_data();
    asc_info.offset = result->accel_buff->get_allocator()->get_offset();

    GX_VK_CHK(vkCreateAccelerationStructureKHR(vk_dev, &asc_info, nullptr, &result->vulkan_data));
    GX_VK_MARK(name + "-blas-temp", result->vulkan_data, vk_e.get_logical_device());

    bge_info.dstAccelerationStructure = result->vulkan_data;

    auto scratch_buf = vk_e.get_buffer_manager().create_static(static_cast<std::size_t>(bsz_info.buildScratchSize));
    GX_CHECK_NOT_EQUAL_D(nullptr, scratch_buf);
    const auto scratch_address = scratch_buf->get_device_address();
    bge_info.scratchData.deviceAddress = scratch_address;

    std::shared_ptr<query::Pool> query_pool(new query::Pool(
        dev, VK_QUERY_TYPE_ACCELERATION_STRUCTURE_COMPACTED_SIZE_KHR));

    auto cmd = std::make_shared<command::Buffer>(std::move(cmd_mgr.create(command::Type::Primary)));
    GX_VK_MARK(name + "-blas-temp-cmd", cmd->get_vulkan_data(), vk_e.get_logical_device());
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
    waiter_queue->submit(*cmd, *fence);
    waiter->push([this, name = std::move(name), cmd = std::move(cmd), fence = std::move(fence), c = std::move(c), result = std::move(result), query_pool = std::move(query_pool)]() mutable noexcept {
        create_accel_after_query_ready(std::move(name), std::move(fence), std::move(c), std::move(result), std::move(query_pool));
        waiter->push([cmd = std::move(cmd)] {});
    });
}

void gearoenix::vulkan::mesh::Manager::create_accel_after_query_ready(
    std::string&& name,
    std::shared_ptr<sync::Fence>&& fence,
    core::sync::EndCaller&& c,
    std::shared_ptr<Mesh>&& result,
    std::shared_ptr<query::Pool>&& query_pool) noexcept
{
    VkAccelerationStructureKHR old_accel = result->vulkan_data;
    auto old_accel_buff = std::move(result->accel_buff);

    auto& dev = vk_e.get_logical_device();
    auto& cmd_mgr = vk_e.get_command_manager();
    const auto vk_dev = dev.get_vulkan_data();

    fence->wait();
    fence->reset();

    const auto compact_size = query_pool->get_acceleration_structure_compacted_size();

    result->accel_buff = std::move(vk_e.get_buffer_manager().create_static(static_cast<std::size_t>(compact_size)));
    GX_CHECK_NOT_EQUAL_D(nullptr, result->accel_buff);

    VkAccelerationStructureCreateInfoKHR asc_info;
    GX_SET_ZERO(asc_info);
    asc_info.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_CREATE_INFO_KHR;
    asc_info.type = VK_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL_KHR;
    asc_info.size = compact_size;
    asc_info.buffer = result->accel_buff->get_vulkan_data();
    asc_info.offset = result->accel_buff->get_allocator()->get_offset();

    GX_VK_CHK(vkCreateAccelerationStructureKHR(vk_dev, &asc_info, nullptr, &result->vulkan_data));
    GX_VK_MARK(name + "-blas", result->vulkan_data, vk_e.get_logical_device());

    auto cmd = std::make_shared<command::Buffer>(std::move(cmd_mgr.create(command::Type::Primary)));
    GX_VK_MARK(name + "-blas-cmd", cmd->get_vulkan_data(), vk_e.get_logical_device());
    cmd->begin();
    VkCopyAccelerationStructureInfoKHR copy_info;
    GX_SET_ZERO(copy_info);
    copy_info.sType = VK_STRUCTURE_TYPE_COPY_ACCELERATION_STRUCTURE_INFO_KHR;
    copy_info.src = old_accel;
    copy_info.dst = result->vulkan_data;
    copy_info.mode = VK_COPY_ACCELERATION_STRUCTURE_MODE_COMPACT_KHR;
    vkCmdCopyAccelerationStructureKHR(cmd->get_vulkan_data(), &copy_info);
    cmd->end();
    waiter_queue->submit(*cmd, *fence);
    waiter->push(
        [this,
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

void gearoenix::vulkan::mesh::Manager::create_accel_after_blas_copy(
    [[maybe_unused]] core::sync::EndCaller&& c,
    std::shared_ptr<Mesh>&& result) noexcept
{
    result->initialize_blas();
}

std::shared_ptr<gearoenix::render::mesh::Mesh> gearoenix::vulkan::mesh::Manager::build(
    std::string&& name,
    render::Vertices&& vertices,
    std::vector<std::uint32_t>&& indices,
    math::Aabb3<double>&& occlusion_box,
    const core::sync::EndCaller& end_callback) noexcept
{
    std::shared_ptr<std::shared_ptr<Mesh>> result(new std::shared_ptr<Mesh>());
    core::sync::EndCaller end([this, name = name, c = end_callback, vertices_count = core::count(vertices), indices_count = indices.size(), result]() mutable noexcept -> void {
        waiter->push([this, name = std::move(name), c = std::move(c), vertices_count, indices_count, result]() mutable noexcept {
            create_accel_after_vertices_ready(std::move(name), vertices_count, indices_count, std::move(c), std::move(*result));
        });
    });
    *result = std::make_shared<Mesh>(vk_e, name, vertices, indices, std::move(occlusion_box), end);
    return *result;
}

gearoenix::vulkan::mesh::Manager::Manager(engine::Engine& e) noexcept
    : render::mesh::Manager(e)
    , vk_e(e)
    , waiter(new core::sync::WorkWaiter())
    , descriptor_bindings(GX_VK_BIND_RAY_MAX)
    , mesh_descriptor_write_info(default_init_mesh_descriptor_count)
    , vertex_descriptor_write_info(default_init_mesh_descriptor_count)
    , index_descriptor_write_info(default_init_mesh_descriptor_count)
{
    waiter->push([this] {
        waiter_queue = std::make_unique<queue::Queue>(vk_e);
    });

    GX_SET_VECTOR_ZERO(mesh_descriptor_write_info);
    GX_SET_VECTOR_ZERO(vertex_descriptor_write_info);
    GX_SET_VECTOR_ZERO(index_descriptor_write_info);

    auto& vertices_bindings = descriptor_bindings[GX_VK_BIND_RAY_VERTICES];
    GX_SET_ZERO(vertices_bindings);
    vertices_bindings.binding = GX_VK_BIND_RAY_VERTICES;
    vertices_bindings.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    vertices_bindings.descriptorCount = default_init_mesh_descriptor_count;
    vertices_bindings.stageFlags = VK_SHADER_STAGE_RAYGEN_BIT_KHR | VK_SHADER_STAGE_ANY_HIT_BIT_KHR | VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR;

    auto& indices_bindings = descriptor_bindings[GX_VK_BIND_RAY_INDICES];
    GX_SET_ZERO(indices_bindings);
    indices_bindings.binding = GX_VK_BIND_RAY_INDICES;
    indices_bindings.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    indices_bindings.descriptorCount = default_init_mesh_descriptor_count;
    indices_bindings.stageFlags = VK_SHADER_STAGE_RAYGEN_BIT_KHR | VK_SHADER_STAGE_ANY_HIT_BIT_KHR | VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR;

    auto& materials_bindings = descriptor_bindings[GX_VK_BIND_RAY_MATERIALS];
    GX_SET_ZERO(materials_bindings);
    materials_bindings.binding = GX_VK_BIND_RAY_MATERIALS;
    materials_bindings.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    materials_bindings.descriptorCount = default_init_material_descriptor_count;
    materials_bindings.stageFlags = VK_SHADER_STAGE_RAYGEN_BIT_KHR | VK_SHADER_STAGE_ANY_HIT_BIT_KHR | VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR;

    auto& meshes_bindings = descriptor_bindings[GX_VK_BIND_RAY_MESH];
    GX_SET_ZERO(meshes_bindings);
    meshes_bindings.binding = GX_VK_BIND_RAY_MESH;
    meshes_bindings.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    meshes_bindings.descriptorCount = default_init_mesh_descriptor_count;
    meshes_bindings.stageFlags = VK_SHADER_STAGE_RAYGEN_BIT_KHR | VK_SHADER_STAGE_ANY_HIT_BIT_KHR | VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR;

    auto& tlas_bindings = descriptor_bindings[GX_VK_BIND_RAY_TLAS];
    GX_SET_ZERO(tlas_bindings);
    tlas_bindings.binding = GX_VK_BIND_RAY_TLAS;
    tlas_bindings.descriptorType = VK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_KHR;
    tlas_bindings.descriptorCount = 1;
    tlas_bindings.stageFlags = VK_SHADER_STAGE_RAYGEN_BIT_KHR | VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR;

    auto& textures_bindings = descriptor_bindings[GX_VK_BIND_RAY_2D_TEXTURES];
    GX_SET_ZERO(textures_bindings);
    textures_bindings.binding = GX_VK_BIND_RAY_2D_TEXTURES;
    textures_bindings.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    textures_bindings.descriptorCount = default_init_2d_texture_descriptor_count;
    textures_bindings.stageFlags = VK_SHADER_STAGE_RAYGEN_BIT_KHR | VK_SHADER_STAGE_ANY_HIT_BIT_KHR | VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR;

    auto& cube_textures_bindings = descriptor_bindings[GX_VK_BIND_RAY_CUBE_TEXTURES];
    GX_SET_ZERO(cube_textures_bindings);
    cube_textures_bindings.binding = GX_VK_BIND_RAY_CUBE_TEXTURES;
    cube_textures_bindings.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    cube_textures_bindings.descriptorCount = default_init_cube_texture_descriptor_count;
    cube_textures_bindings.stageFlags = VK_SHADER_STAGE_RAYGEN_BIT_KHR | VK_SHADER_STAGE_ANY_HIT_BIT_KHR | VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR;

    auto& out_img_bindings = descriptor_bindings[GX_VK_BIND_RAY_OUT_IMAGE];
    GX_SET_ZERO(out_img_bindings);
    out_img_bindings.binding = GX_VK_BIND_RAY_OUT_IMAGE;
    out_img_bindings.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
    out_img_bindings.descriptorCount = 1;
    out_img_bindings.stageFlags = VK_SHADER_STAGE_RAYGEN_BIT_KHR | VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR;

    auto& camera_bindings = descriptor_bindings[GX_VK_BIND_RAY_CAMERA];
    GX_SET_ZERO(camera_bindings);
    camera_bindings.binding = GX_VK_BIND_RAY_CAMERA;
    camera_bindings.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
    camera_bindings.descriptorCount = 1;
    camera_bindings.stageFlags = VK_SHADER_STAGE_RAYGEN_BIT_KHR | VK_SHADER_STAGE_ANY_HIT_BIT_KHR;
}

gearoenix::vulkan::mesh::Manager::~Manager() noexcept = default;

void gearoenix::vulkan::mesh::Manager::update() noexcept
{
}

#endif
