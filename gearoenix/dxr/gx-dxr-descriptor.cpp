#include "gx-dxr-descriptor.hpp"

#ifdef GX_RENDER_DXR_ENABLED
#include "../core/gx-cr-allocator.hpp"
#include "../core/macro/gx-cr-mcr-zeroer.hpp"
#include "gx-dxr-check.hpp"
#include "gx-dxr-device.hpp"

gearoenix::dxr::DescriptorAllocator::DescriptorAllocator(Device& d) noexcept
{
    D3D12_DESCRIPTOR_HEAP_DESC desc;
    GX_SET_ZERO(desc);
    desc.NumDescriptors = GPU_DESCRIPTORS_COUNT;
    desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
    desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
    const auto& dev = d.get_device();
    GX_DXR_CHECK(dev->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&heap)))

#ifdef GX_DEBUG_MODE
    GX_DXR_CHECK(heap->SetName(L"main-descriptor-heap"))
#endif

    size_increment = dev->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
    cpu_starting_handle = heap->GetCPUDescriptorHandleForHeapStart();
    gpu_starting_handle = heap->GetGPUDescriptorHandleForHeapStart();
}

gearoenix::dxr::DescriptorAllocator::~DescriptorAllocator() noexcept = default;

gearoenix::dxr::Descriptor::Descriptor(
    D3D12_CPU_DESCRIPTOR_HANDLE&& cpu_handle,
    std::shared_ptr<core::Allocator>&& allocator) noexcept
    : cpu_handle(std::move(cpu_handle))
    , allocator(std::move(allocator))
{
}

gearoenix::dxr::Descriptor::~Descriptor() noexcept = default;

gearoenix::dxr::Descriptor::Descriptor(Descriptor&&) noexcept = default;

gearoenix::dxr::DescriptorManager::DescriptorManager(std::shared_ptr<Device> d) noexcept
    : device(std::move(d))
    , allocator(*device)
{
}

gearoenix::dxr::DescriptorManager::~DescriptorManager() noexcept = default;

gearoenix::dxr::Descriptor gearoenix::dxr::DescriptorManager::allocate() noexcept
{
    auto alc = allocator.allocator->allocate(1);
    return Descriptor(
        D3D12_CPU_DESCRIPTOR_HANDLE {
            .ptr = allocator.cpu_starting_handle.ptr + alc->get_offset(),
        },
        std::move(alc));
}

#endif
