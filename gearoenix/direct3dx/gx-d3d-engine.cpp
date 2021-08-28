#include "gx-d3d-engine.hpp"
#ifdef GX_RENDER_DIRECT3DX_ENABLED
#include "../core/ecs/gx-cr-ecs-world.hpp"
#include "../core/macro/gx-cr-mcr-counter.hpp"
#include "../core/macro/gx-cr-mcr-zeroer.hpp"
#include "../platform/gx-plt-application.hpp"
#include "gx-d3d-adapter.hpp"
#include "gx-d3d-check.hpp"
#include "gx-d3d-device.hpp"
#include "gx-d3d-queue.hpp"
#include "gx-d3d-shader.hpp"
#include "gx-d3d-swapchain.hpp"
#include <d3dx12.h>
#include <dxgidebug.h>
#include <fstream>
#include <string>

constexpr static const wchar_t* const GX_HIT_GROUP_NAME = L"GxHitGroup";
constexpr static const wchar_t* const GX_RAYGEN_SHADER_NAME = L"GxRaygenShader";
constexpr static const wchar_t* const GX_CLOSEST_HIT_SHADER_NAME = L"GxClosestHitShader";
constexpr static const wchar_t* const GX_MISS_SHADER_NAME = L"GxMissShader";

gearoenix::direct3dx::Engine::Engine(platform::Application& platform_application) noexcept
    : render::engine::Engine(render::engine::Type::Direct3DX, platform_application)
    , platform_application(platform_application)
{
    device_lost_handle();
    GX_TODO
}

void gearoenix::direct3dx::Engine::device_lost_handle(const int failed_tries) noexcept
{
    GX_ASSERT(failed_tries < 3)

    swapchain = nullptr;
    queue = nullptr;
    device = nullptr;
    adapter = nullptr;

    {
        Microsoft::WRL::ComPtr<IDXGIDebug1> debug;
        if (SUCCEEDED(DXGIGetDebugInterface1(0, IID_PPV_ARGS(&debug)))) {
            debug->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_FLAGS(DXGI_DEBUG_RLO_SUMMARY | DXGI_DEBUG_RLO_IGNORE_INTERNAL));
        }
    }

    adapter = std::make_shared<Adapter>();
    device = std::make_shared<Device>(adapter);
    queue = std::make_shared<Queue>(device);
    swapchain = std::make_shared<Swapchain>(queue);

    window_resized(failed_tries);
}

void gearoenix::direct3dx::Engine::window_resized(int failed_tries) noexcept
{
    if (swapchain->set_window_size(platform_application))
        device_lost_handle(++failed_tries);
    initialize_ray_tracing_resources();
}

void gearoenix::direct3dx::Engine::initialize_ray_tracing_resources() noexcept
{
    // Create root signatures for the shaders.
    create_root_signatures();
    // Create a raytracing pipeline state object which defines the binding of shaders, state and resources to be used during raytracing.
    create_raytracing_pipeline_state_object();
    // Create a heap for descriptors.
    create_descriptor_heap();
    // Build geometry to be used in the sample.
    build_geometry();
    // Build raytracing acceleration structures from the generated geometry.
    build_acceleration_structures();
    // Build shader tables, which define shaders and their local root arguments.
    build_shader_tables();
    // Create an output 2D texture to store the raytracing result to.
    create_raytracing_output_resource();
}

enum struct GlobalRootSignatureParams {
    OutputViewSlot = 0,
    AccelerationStructureSlot,
    Count,
};

enum struct LocalRootSignatureParams {
    ViewportConstantSlot = 0,
    Count
};

void gearoenix::direct3dx::Engine::create_root_signatures() noexcept
{
    // Global Root Signature
    // This is a root signature that is shared across all raytracing shaders invoked during a DispatchRays() call.
    {
        CD3DX12_DESCRIPTOR_RANGE uav_descriptor;
        uav_descriptor.Init(D3D12_DESCRIPTOR_RANGE_TYPE_UAV, 1, 0);
        CD3DX12_ROOT_PARAMETER root_parameters[static_cast<std::size_t>(GlobalRootSignatureParams::Count)];
        root_parameters[static_cast<std::size_t>(GlobalRootSignatureParams::OutputViewSlot)].InitAsDescriptorTable(1, &uav_descriptor);
        root_parameters[static_cast<std::size_t>(GlobalRootSignatureParams::AccelerationStructureSlot)].InitAsShaderResourceView(0);
        CD3DX12_ROOT_SIGNATURE_DESC global_root_signature_desc(ARRAYSIZE(root_parameters), root_parameters);
        serialize_and_create_raytracing_root_signature(global_root_signature_desc, raytracing_global_root_signature);
    }
    // Local Root Signature
    // This is a root signature that enables a shader to have unique arguments that come from shader tables.
    {
        CD3DX12_ROOT_PARAMETER root_parameters[static_cast<std::size_t>(LocalRootSignatureParams::Count)];
        root_parameters[static_cast<std::size_t>(LocalRootSignatureParams::ViewportConstantSlot)].InitAsConstants(sizeof(RayGenConstantBuffer), 0, 0);
        CD3DX12_ROOT_SIGNATURE_DESC local_root_signature_desc(GX_COUNT_OF(root_parameters), root_parameters);
        local_root_signature_desc.Flags = D3D12_ROOT_SIGNATURE_FLAG_LOCAL_ROOT_SIGNATURE;
        serialize_and_create_raytracing_root_signature(local_root_signature_desc, raytracing_local_root_signature);
    }
}

void gearoenix::direct3dx::Engine::serialize_and_create_raytracing_root_signature(
    const D3D12_ROOT_SIGNATURE_DESC& desc, Microsoft::WRL::ComPtr<ID3D12RootSignature>& root_sig) noexcept
{
    auto* const dev = device->get_device().Get();
    Microsoft::WRL::ComPtr<ID3DBlob> blob;
    Microsoft::WRL::ComPtr<ID3DBlob> error;
    if (FAILED(D3D12SerializeRootSignature(&desc, D3D_ROOT_SIGNATURE_VERSION_1, &blob, &error)) && nullptr != error)
        GX_LOG_F(static_cast<char*>(error->GetBufferPointer()))
    GX_D3D_CHECK(dev->CreateRootSignature(1, blob->GetBufferPointer(), blob->GetBufferSize(), IID_PPV_ARGS(&root_sig)))
}

std::vector<char> read_shader_file(const std::string& file_name) noexcept
{
    std::ifstream file(file_name, std::ios::in | std::ios::binary);
    if (!file.is_open())
        GX_UNEXPECTED
    file.seekg(0, std::ios::end);
    std::vector<char> shader_data(file.tellg());
    file.seekg(0);
    file.read(shader_data.data(), static_cast<std::streamsize>(shader_data.size()));
    return shader_data;
}

// Create a raytracing pipeline state object (RTPSO).
// An RTPSO represents a full set of shaders reachable by a DispatchRays() call,
// with all configuration options resolved, such as local signatures and other state.
void gearoenix::direct3dx::Engine::create_raytracing_pipeline_state_object() noexcept
{
    // Create 7 subobjects that combine into a RTPSO:
    // Subobjects need to be associated with DXIL exports (i.e. shaders) either by way of default or explicit associations.
    // Default association applies to every exported shader entrypoint that doesn't have any of the same type of
    // subobject associated with it.
    // This simple sample utilizes default shader association except for local root signature subobject
    // which has an explicit association specified purely for demonstration purposes.
    // 1 - DXIL library
    // 1 - Triangle hit group
    // 1 - Shader config
    // 2 - Local root signature and association
    // 1 - Global root signature
    // 1 - Pipeline config
    CD3DX12_STATE_OBJECT_DESC raytracing_pipeline { D3D12_STATE_OBJECT_TYPE_RAYTRACING_PIPELINE };
    // DXIL library
    // This contains the shaders and their entry-points for the state object.
    // Since shaders are not considered a subobject, they need to be passed in via DXIL library subobjects.
    auto lib = raytracing_pipeline.CreateSubobject<CD3DX12_DXIL_LIBRARY_SUBOBJECT>();
    static const auto lib_file_content = read_shader_file("../../../../assets/pbr.cso");
    D3D12_SHADER_BYTECODE lib_dxil = CD3DX12_SHADER_BYTECODE(lib_file_content.data(), lib_file_content.size());
    lib->SetDXILLibrary(&lib_dxil);
    // Define which shader exports to surface from the library.
    // If no shader exports are defined for a DXIL library subobject, all shaders will be surfaced.
    // In this sample, this could be omitted for convenience since the sample uses all shaders in the library.
    {
        lib->DefineExport(GX_RAYGEN_SHADER_NAME);
        lib->DefineExport(GX_CLOSEST_HIT_SHADER_NAME);
        lib->DefineExport(GX_MISS_SHADER_NAME);
    }
    // Triangle hit group
    // A hit group specifies the closest hit, any hit and intersection shaders to be executed when a ray intersects the
    // geometry's triangle/AABB.
    // In this sample, we only use triangle geometry with the closest hit shader, so others are not set.
    auto hit_group = raytracing_pipeline.CreateSubobject<CD3DX12_HIT_GROUP_SUBOBJECT>();
    hit_group->SetClosestHitShaderImport(GX_CLOSEST_HIT_SHADER_NAME);
    hit_group->SetHitGroupExport(GX_HIT_GROUP_NAME);
    hit_group->SetHitGroupType(D3D12_HIT_GROUP_TYPE_TRIANGLES);
    // Shader config
    // Defines the maximum sizes in bytes for the ray payload and attribute structure.
    auto shader_config = raytracing_pipeline.CreateSubobject<CD3DX12_RAYTRACING_SHADER_CONFIG_SUBOBJECT>();
    const UINT payload_size = 4 * sizeof(float); // float4 color
    const UINT attribute_size = 2 * sizeof(float); // float2 barycentrics
    shader_config->Config(payload_size, attribute_size);
    // Local root signature and shader association
    create_local_root_signature_subobjects(raytracing_pipeline);
    // This is a root signature that enables a shader to have unique arguments that come from shader tables.

    // Global root signature
    // This is a root signature that is shared across all raytracing shaders invoked during a DispatchRays() call.
    auto global_root_signature = raytracing_pipeline.CreateSubobject<CD3DX12_GLOBAL_ROOT_SIGNATURE_SUBOBJECT>();
    global_root_signature->SetRootSignature(raytracing_global_root_signature.Get());

    // Pipeline config
    // Defines the maximum TraceRay() recursion depth.
    auto pipeline_config = raytracing_pipeline.CreateSubobject<CD3DX12_RAYTRACING_PIPELINE_CONFIG_SUBOBJECT>();
    // PERFORMANCE TIP: Set max recursion depth as low as needed
    // as drivers may apply optimization strategies for low recursion depths.
    const UINT max_recursion_depth = 1; // ~ primary rays only.
    pipeline_config->Config(max_recursion_depth);

    // Create the state object.
    GX_D3D_CHECK(device->get_device()->CreateStateObject(raytracing_pipeline, IID_PPV_ARGS(&state_object)))
}

// Local root signature and shader association
// This is a root signature that enables a shader to have unique arguments that come from shader tables.
void gearoenix::direct3dx::Engine::create_local_root_signature_subobjects(CD3DX12_STATE_OBJECT_DESC& raytracing_pipeline) const noexcept
{
    // Hit group and miss shaders in this sample are not using a local root signature and thus one is not associated with them.

    // Local root signature to be used in a ray gen shader.
    {
        auto local_root_signature = raytracing_pipeline.CreateSubobject<CD3DX12_LOCAL_ROOT_SIGNATURE_SUBOBJECT>();
        local_root_signature->SetRootSignature(raytracing_local_root_signature.Get());
        // Shader association
        auto root_signature_association = raytracing_pipeline.CreateSubobject<CD3DX12_SUBOBJECT_TO_EXPORTS_ASSOCIATION_SUBOBJECT>();
        root_signature_association->SetSubobjectToAssociate(*local_root_signature);
        root_signature_association->AddExport(GX_RAYGEN_SHADER_NAME);
    }
}

void gearoenix::direct3dx::Engine::create_descriptor_heap() noexcept
{
    auto* const dev = device->get_device().Get();
    D3D12_DESCRIPTOR_HEAP_DESC descriptor_heap_desc;
    GX_SET_ZERO(descriptor_heap_desc)
    // Allocate a heap for a single descriptor:
    // 1 - raytracing output texture UAV
    descriptor_heap_desc.NumDescriptors = 1;
    descriptor_heap_desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
    descriptor_heap_desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
    descriptor_heap_desc.NodeMask = 0;
    dev->CreateDescriptorHeap(&descriptor_heap_desc, IID_PPV_ARGS(&descriptor_heap));
    descriptor_heap->SetName(L"descriptor_heap");
    descriptor_size = dev->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
}

typedef UINT16 GxIndex;

struct GxVertex final {
    float x = -1.0f;
    float y = -1.0f;
    float z = -1.0f;
};

void gearoenix::direct3dx::Engine::build_geometry() noexcept
{
    const GxIndex indices[] { 0, 1, 2 };
    const float depth_value = 1.0;
    const float offset = 0.7f;
    const GxVertex vertices[] {
        { 0, -offset, depth_value },
        { -offset, offset, depth_value },
        { offset, offset, depth_value },
    };
    allocate_upload_buffer(vertices, sizeof(vertices), vertex_buffer, L"vertex_buffer");
    allocate_upload_buffer(indices, sizeof(indices), index_buffer, L"index_buffer");
}

void gearoenix::direct3dx::Engine::allocate_upload_buffer(
    const void* const data,
    const UINT64 data_size,
    Microsoft::WRL::ComPtr<ID3D12Resource>& resource,
    const wchar_t* const resource_name) noexcept
{
    auto* const dev = device->get_device().Get();
    auto upload_heap_properties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
    auto buffer_desc = CD3DX12_RESOURCE_DESC::Buffer(data_size);
    GX_D3D_CHECK(dev->CreateCommittedResource(
        &upload_heap_properties,
        D3D12_HEAP_FLAG_NONE,
        &buffer_desc,
        D3D12_RESOURCE_STATE_GENERIC_READ,
        nullptr,
        IID_PPV_ARGS(&resource)))
    resource->SetName(resource_name);
    void* pMappedData;
    resource->Map(0, nullptr, &pMappedData);
    std::memcpy(pMappedData, data, data_size);
    resource->Unmap(0, nullptr);
}

void gearoenix::direct3dx::Engine::allocate_uav_buffer(
    const UINT64 buffer_size,
    Microsoft::WRL::ComPtr<ID3D12Resource>& resource,
    const D3D12_RESOURCE_STATES initial_resource_state,
    const wchar_t* const resource_name) noexcept
{
    auto* const dev = device->get_device().Get();
    auto upload_heap_properties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
    auto buffer_desc = CD3DX12_RESOURCE_DESC::Buffer(buffer_size, D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS);
    GX_D3D_CHECK(dev->CreateCommittedResource(
        &upload_heap_properties,
        D3D12_HEAP_FLAG_NONE,
        &buffer_desc,
        initial_resource_state,
        nullptr,
        IID_PPV_ARGS(&resource)))
    resource->SetName(resource_name);
}

void gearoenix::direct3dx::Engine::build_acceleration_structures() noexcept
{
    auto* const dev = device->get_device().Get();
    auto* const command_list = swapchain->get_command_list().Get();
    auto* const command_queue = queue->get_command_queue().Get();
    auto* const command_allocator = swapchain->get_current_command_allocator().Get();
    // Reset the command list for the acceleration structure construction.
    command_list->Reset(command_allocator, nullptr);

    D3D12_RAYTRACING_GEOMETRY_DESC geometry_desc;
    GX_SET_ZERO(geometry_desc)
    geometry_desc.Type = D3D12_RAYTRACING_GEOMETRY_TYPE_TRIANGLES;
    geometry_desc.Triangles.IndexBuffer = index_buffer->GetGPUVirtualAddress();
    geometry_desc.Triangles.IndexCount = static_cast<UINT>(index_buffer->GetDesc().Width) / sizeof(GxIndex);
    geometry_desc.Triangles.IndexFormat = DXGI_FORMAT_R16_UINT;
    geometry_desc.Triangles.Transform3x4 = 0;
    geometry_desc.Triangles.VertexFormat = DXGI_FORMAT_R32G32B32_FLOAT;
    geometry_desc.Triangles.VertexCount = static_cast<UINT>(vertex_buffer->GetDesc().Width) / sizeof(GxVertex);
    geometry_desc.Triangles.VertexBuffer.StartAddress = vertex_buffer->GetGPUVirtualAddress();
    geometry_desc.Triangles.VertexBuffer.StrideInBytes = sizeof(GxVertex);
    // Mark the geometry as opaque.
    // PERFORMANCE TIP: mark geometry as opaque whenever applicable as it can enable important ray processing optimizations.
    // Note: When rays encounter opaque geometry an any hit shader will not be executed whether it is present or not.
    geometry_desc.Flags = D3D12_RAYTRACING_GEOMETRY_FLAG_OPAQUE;

    // Get required sizes for an acceleration structure.
    D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAGS build_flags = D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAG_PREFER_FAST_TRACE;
    D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_INPUTS top_level_inputs;
    GX_SET_ZERO(top_level_inputs)
    top_level_inputs.DescsLayout = D3D12_ELEMENTS_LAYOUT_ARRAY;
    top_level_inputs.Flags = build_flags;
    top_level_inputs.NumDescs = 1;
    top_level_inputs.Type = D3D12_RAYTRACING_ACCELERATION_STRUCTURE_TYPE_TOP_LEVEL;

    D3D12_RAYTRACING_ACCELERATION_STRUCTURE_PREBUILD_INFO top_level_prebuild_info;
    GX_SET_ZERO(top_level_prebuild_info)
    dev->GetRaytracingAccelerationStructurePrebuildInfo(&top_level_inputs, &top_level_prebuild_info);
    GX_ASSERT(top_level_prebuild_info.ResultDataMaxSizeInBytes > 0)

    D3D12_RAYTRACING_ACCELERATION_STRUCTURE_PREBUILD_INFO bottom_level_prebuild_info;
    GX_SET_ZERO(bottom_level_prebuild_info)
    D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_INPUTS bottom_level_inputs = top_level_inputs;
    bottom_level_inputs.Type = D3D12_RAYTRACING_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL;
    bottom_level_inputs.pGeometryDescs = &geometry_desc;
    dev->GetRaytracingAccelerationStructurePrebuildInfo(&bottom_level_inputs, &bottom_level_prebuild_info);
    GX_ASSERT(bottom_level_prebuild_info.ResultDataMaxSizeInBytes > 0)

    Microsoft::WRL::ComPtr<ID3D12Resource> scratch_resource;
    allocate_uav_buffer(
        std::max(top_level_prebuild_info.ScratchDataSizeInBytes, bottom_level_prebuild_info.ScratchDataSizeInBytes),
        scratch_resource,
        D3D12_RESOURCE_STATE_UNORDERED_ACCESS,
        L"ScratchResource");

    // Allocate resources for acceleration structures.
    // Acceleration structures can only be placed in resources that are created in the default heap (or custom heap equivalent).
    // Default heap is OK since the application doesn't need CPU read/write access to them.
    // The resources that will contain acceleration structures must be created in the state D3D12_RESOURCE_STATE_RAYTRACING_ACCELERATION_STRUCTURE,
    // and must have resource flag D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS. The ALLOW_UNORDERED_ACCESS requirement simply acknowledges both:
    //  - the system will be doing this type of access in its implementation of acceleration structure builds behind the scenes.
    //  - from the app point of view, synchronization of writes/reads to acceleration structures is accomplished using UAV barriers.

    allocate_uav_buffer(
        bottom_level_prebuild_info.ResultDataMaxSizeInBytes,
        bottom_level_acceleration_structure,
        D3D12_RESOURCE_STATE_RAYTRACING_ACCELERATION_STRUCTURE,
        L"BottomLevelAccelerationStructure");
    allocate_uav_buffer(
        top_level_prebuild_info.ResultDataMaxSizeInBytes,
        top_level_acceleration_structure,
        D3D12_RESOURCE_STATE_RAYTRACING_ACCELERATION_STRUCTURE,
        L"TopLevelAccelerationStructure");

    // Create an instance desc for the bottom-level acceleration structure.
    Microsoft::WRL::ComPtr<ID3D12Resource> instance_descriptions;
    D3D12_RAYTRACING_INSTANCE_DESC instance_desc;
    GX_SET_ZERO(instance_desc)
    instance_desc.Transform[0][0] = 1.0f;
    instance_desc.Transform[1][1] = 1.0f;
    instance_desc.Transform[2][2] = 1.0f;
    instance_desc.InstanceMask = 1;
    instance_desc.AccelerationStructure = bottom_level_acceleration_structure->GetGPUVirtualAddress();
    allocate_upload_buffer(&instance_desc, sizeof(instance_desc), instance_descriptions, L"InstanceDescriptions");

    // Bottom Level Acceleration Structure desc
    D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_DESC bottom_level_build_desc;
    GX_SET_ZERO(bottom_level_build_desc)
    bottom_level_build_desc.Inputs = bottom_level_inputs;
    bottom_level_build_desc.ScratchAccelerationStructureData = scratch_resource->GetGPUVirtualAddress();
    bottom_level_build_desc.DestAccelerationStructureData = bottom_level_acceleration_structure->GetGPUVirtualAddress();

    // Top Level Acceleration Structure desc
    D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_DESC top_level_build_desc;
    GX_SET_ZERO(top_level_build_desc)
    top_level_inputs.InstanceDescs = instance_descriptions->GetGPUVirtualAddress();
    top_level_build_desc.Inputs = top_level_inputs;
    top_level_build_desc.DestAccelerationStructureData = top_level_acceleration_structure->GetGPUVirtualAddress();
    top_level_build_desc.ScratchAccelerationStructureData = scratch_resource->GetGPUVirtualAddress();

    // Build acceleration structure.
    command_list->BuildRaytracingAccelerationStructure(&bottom_level_build_desc, 0, nullptr);
    const auto barrier = CD3DX12_RESOURCE_BARRIER::UAV(bottom_level_acceleration_structure.Get());
    command_list->ResourceBarrier(1, &barrier);
    command_list->BuildRaytracingAccelerationStructure(&top_level_build_desc, 0, nullptr);
    GX_D3D_CHECK(command_list->Close())
    ID3D12CommandList* command_lists[] { command_list };
    // Kick off acceleration structure construction.
    command_queue->ExecuteCommandLists(1, command_lists);

    // Wait for GPU to finish as the locally created temporary GPU resources will get released once we go out of scope.
    swapchain->wait_for_gpu();
}

void gearoenix::direct3dx::Engine::build_shader_tables() noexcept
{
    auto* const dev = device->get_device().Get();

    // Get shader identifiers.
    Microsoft::WRL::ComPtr<ID3D12StateObjectProperties> state_object_properties;
    GX_D3D_CHECK(state_object.As(&state_object_properties))
    void* const ray_gen_shader_identifier = state_object_properties->GetShaderIdentifier(GX_RAYGEN_SHADER_NAME);
    void* const miss_shader_identifier = state_object_properties->GetShaderIdentifier(GX_MISS_SHADER_NAME);
    void* const hit_group_shader_identifier = state_object_properties->GetShaderIdentifier(GX_HIT_GROUP_NAME);
    const UINT shader_identifier_size = D3D12_SHADER_IDENTIFIER_SIZE_IN_BYTES;

    // Ray gen shader table
    {
        struct RootArguments final {
            RayGenConstantBuffer cb;
        } root_arguments;
        root_arguments.cb = ray_gen_cb;

        const UINT num_shader_records = 1;
        const UINT shader_record_size = shader_identifier_size + sizeof(root_arguments);
        ShaderTable ray_gen_shader_table_builder(dev, num_shader_records, shader_record_size, L"RayGenShaderTable");
        ray_gen_shader_table_builder.push_back(ShaderRecord(ray_gen_shader_identifier, shader_identifier_size, &root_arguments, sizeof(root_arguments)));
        ray_gen_shader_table = ray_gen_shader_table_builder.get_resource();
    }

    // Miss shader table
    {
        const UINT num_shader_records = 1;
        const UINT shader_record_size = shader_identifier_size;
        ShaderTable miss_shader_table_builder(dev, num_shader_records, shader_record_size, L"MissShaderTable");
        miss_shader_table_builder.push_back(ShaderRecord(miss_shader_identifier, shader_identifier_size));
        miss_shader_table = miss_shader_table_builder.get_resource();
    }

    // Hit group shader table
    {
        const UINT num_shader_records = 1;
        const UINT shader_record_size = shader_identifier_size;
        ShaderTable hit_group_shader_table_builder(dev, num_shader_records, shader_record_size, L"HitGroupShaderTable");
        hit_group_shader_table_builder.push_back(ShaderRecord(hit_group_shader_identifier, shader_identifier_size));
        hit_group_shader_table = hit_group_shader_table_builder.get_resource();
    }
}

void gearoenix::direct3dx::Engine::create_raytracing_output_resource() noexcept
{
    auto* const dev = device->get_device().Get();
    auto& base_plt_app = platform_application.get_base();
    // Create the output resource. The dimensions and format should match the swap-chain.
    const auto uav_desc = CD3DX12_RESOURCE_DESC::Tex2D(
        Swapchain::BACK_BUFFER_FORMAT,
        static_cast<UINT64>(base_plt_app.get_window_width()),
        static_cast<UINT64>(base_plt_app.get_window_height()),
        1, 1, 1, 0, D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS);
    const auto default_heap_properties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
    GX_D3D_CHECK(dev->CreateCommittedResource(
        &default_heap_properties, D3D12_HEAP_FLAG_NONE, &uav_desc, D3D12_RESOURCE_STATE_UNORDERED_ACCESS,
        nullptr, IID_PPV_ARGS(&raytracing_output)))
    raytracing_output->SetName(L"raytracing_output");
    D3D12_CPU_DESCRIPTOR_HANDLE uav_descriptor_handle;
    raytracing_output_resource_uav_descriptor_heap_index = allocate_descriptor(
        uav_descriptor_handle, raytracing_output_resource_uav_descriptor_heap_index);
    D3D12_UNORDERED_ACCESS_VIEW_DESC uav_description;
    GX_SET_ZERO(uav_description)
    uav_description.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE2D;
    dev->CreateUnorderedAccessView(raytracing_output.Get(), nullptr, &uav_description, uav_descriptor_handle);
    raytracing_output_resource_uav_gpu_descriptor = CD3DX12_GPU_DESCRIPTOR_HANDLE(
        descriptor_heap->GetGPUDescriptorHandleForHeapStart(),
        static_cast<INT>(raytracing_output_resource_uav_descriptor_heap_index),
        descriptor_size);
}

UINT gearoenix::direct3dx::Engine::allocate_descriptor(
    D3D12_CPU_DESCRIPTOR_HANDLE& cpu_descriptor,
    UINT descriptor_index_to_use) noexcept
{
    const auto descriptor_heap_cpu_base = descriptor_heap->GetCPUDescriptorHandleForHeapStart();
    if (descriptor_index_to_use >= descriptor_heap->GetDesc().NumDescriptors) {
        descriptor_index_to_use = descriptors_allocated++;
    }
    cpu_descriptor = CD3DX12_CPU_DESCRIPTOR_HANDLE(
        descriptor_heap_cpu_base, static_cast<INT>(descriptor_index_to_use), descriptor_size);
    return descriptor_index_to_use;
}

gearoenix::direct3dx::Engine::~Engine() noexcept
{
    world = nullptr;
    GX_TODO
}

bool gearoenix::direct3dx::Engine::is_supported() noexcept
{
    GX_TODO
    return true;
}

std::unique_ptr<gearoenix::direct3dx::Engine> gearoenix::direct3dx::Engine::construct(
    platform::Application& platform_application) noexcept
{
    return std::unique_ptr<Engine>(new Engine(platform_application));
}

void gearoenix::direct3dx::Engine::start_frame() noexcept
{
    render::engine::Engine::start_frame();
}

void gearoenix::direct3dx::Engine::end_frame() noexcept
{
    render::engine::Engine::end_frame();
}

void gearoenix::direct3dx::Engine::upload_imgui_fonts() noexcept
{
    GX_TODO
}

#endif
