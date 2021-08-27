#include "gx-d3d-buffer.hpp"
#ifdef GX_RENDER_DIRECT3DX_ENABLED
#include "gx-d3d-check.hpp"

gearoenix::direct3dx::GpuUploadBuffer::~GpuUploadBuffer() noexcept
{
    if (nullptr != resource.Get()) {
        resource->Unmap(0, nullptr);
    }
}

void gearoenix::direct3dx::GpuUploadBuffer::allocate(
    ID3D12Device* const device,
    const UINT buffer_size,
    LPCWSTR resource_name) noexcept
{
    const auto upload_heap_properties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
    auto buffer_desc = CD3DX12_RESOURCE_DESC::Buffer(buffer_size);
    GX_D3D_CHECK(device->CreateCommittedResource(
        &upload_heap_properties,
        D3D12_HEAP_FLAG_NONE,
        &buffer_desc,
        D3D12_RESOURCE_STATE_GENERIC_READ,
        nullptr,
        IID_PPV_ARGS(&resource)))
    resource->SetName(resource_name);
}

uint8_t* gearoenix::direct3dx::GpuUploadBuffer::map_cpu_write_only() noexcept
{
    uint8_t* mapped_data;
    // We don't unmap this until the app closes. Keeping buffer mapped for the lifetime of the resource is okay.
    CD3DX12_RANGE read_range(0, 0); // We do not intend to read from this resource on the CPU.
    GX_D3D_CHECK(resource->Map(0, &read_range, reinterpret_cast<void**>(&mapped_data)))
    return mapped_data;
}

#endif