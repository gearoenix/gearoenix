#include "gx-dxr-buffer.hpp"
#ifdef GX_RENDER_DXR_ENABLED
#include "gx-dxr-check.hpp"

gearoenix::dxr::CpuBuffer::CpuBuffer(
    ID3D12Device* const device,
    const UINT buffer_size,
    [[maybe_unused]] LPCWSTR resource_name) noexcept
{
    auto p = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
    auto d = CD3DX12_RESOURCE_DESC::Buffer(buffer_size);
    GX_DXR_CHECK(device->CreateCommittedResource(&p, D3D12_HEAP_FLAG_NONE, &d, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&resource)))
#ifdef GX_DEBUG_MODE
    if (nullptr != resource_name) {
        resource->SetName(resource_name);
    }
#endif
    auto r = CD3DX12_RANGE(0, 0);
    GX_DXR_CHECK(resource->Map(0, &r, &pointer))
}

gearoenix::dxr::CpuBuffer::~CpuBuffer() noexcept
{
    resource->Unmap(0, nullptr);
}

void gearoenix::dxr::CpuBuffer::copy(const void* data, std::size_t size) noexcept
{
    std::memcpy(pointer, data, size);
}

gearoenix::dxr::GpuBuffer::GpuBuffer(
    ID3D12Device* const device,
    const UINT buffer_size,
    [[maybe_unused]] LPCWSTR resource_name) noexcept
{
    auto p = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
    auto d = CD3DX12_RESOURCE_DESC::Buffer(buffer_size);
    GX_DXR_CHECK(device->CreateCommittedResource(&p, D3D12_HEAP_FLAG_NONE, &d, D3D12_RESOURCE_STATE_COPY_DEST, nullptr, IID_PPV_ARGS(&resource)))
#ifdef GX_DEBUG_MODE
    resource->SetName(resource_name);
#endif
}

#endif