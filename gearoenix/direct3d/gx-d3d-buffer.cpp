#include "gx-d3d-buffer.hpp"
#ifdef GX_RENDER_DIRECT3D_ENABLED
#include "../core/macro/gx-cr-mcr-zeroer.hpp"
#include "../math/gx-math-numeric.hpp"
#include "gx-d3d-check.hpp"
#include "gx-d3d-device.hpp"
#include "gx-d3d-engine.hpp"

gearoenix::d3d::CpuBuffer::CpuBuffer(
    ID3D12Device* const device,
    const UINT buffer_size,
    [[maybe_unused]] LPCWSTR resource_name) noexcept
{
    auto p = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
    auto d = CD3DX12_RESOURCE_DESC::Buffer(buffer_size);
    GX_D3D_CHECK(device->CreateCommittedResource(&p, D3D12_HEAP_FLAG_NONE, &d, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&resource)))
    if (nullptr != resource_name) {
        resource->SetName(resource_name);
    }
    auto r = CD3DX12_RANGE(0, 0);
    GX_D3D_CHECK(resource->Map(0, &r, &pointer))
}

gearoenix::d3d::CpuBuffer::~CpuBuffer() noexcept
{
    if (nullptr != resource)
        resource->Unmap(0, nullptr);
}

gearoenix::d3d::CpuBuffer::CpuBuffer(CpuBuffer&& o) noexcept
    : resource(std::move(o.resource))
    , pointer(o.pointer)
{
    o.resource = nullptr;
    o.pointer = nullptr;
}

void gearoenix::d3d::CpuBuffer::copy(const void* data, std::size_t size) noexcept
{
    std::memcpy(pointer, data, size);
}

gearoenix::d3d::GpuBuffer::GpuBuffer(
    ID3D12Device* const device,
    const UINT buffer_size,
    [[maybe_unused]] LPCWSTR resource_name) noexcept
{
    auto p = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
    auto d = CD3DX12_RESOURCE_DESC::Buffer(buffer_size);
    GX_D3D_CHECK(device->CreateCommittedResource(&p, D3D12_HEAP_FLAG_NONE, &d, D3D12_RESOURCE_STATE_COPY_DEST, nullptr, IID_PPV_ARGS(&resource)))
    resource->SetName(resource_name);
}

gearoenix::d3d::UniformBuffer::UniformBuffer(const Engine& e, const UINT buffer_size, LPCWSTR resource_name) noexcept
    : buffer(e.get_device()->get_device().Get(), math::Numeric::align(buffer_size, 256U), resource_name)
    , descriptor(e.get_descriptor_manager()->allocate_others())
{
    D3D12_CONSTANT_BUFFER_VIEW_DESC desc;
    GX_SET_ZERO(desc)
    desc.BufferLocation = buffer.get_resource()->GetGPUVirtualAddress();
    desc.SizeInBytes = math::Numeric::align(buffer_size, 256U);
    e.get_device()->get_device()->CreateConstantBufferView(&desc, descriptor.cpu_handle);
}

gearoenix::d3d::UniformBuffer::UniformBuffer(UniformBuffer&& o) noexcept
    : buffer(std::move(o.buffer))
    , descriptor(std::move(o.descriptor))
{
}

#endif
