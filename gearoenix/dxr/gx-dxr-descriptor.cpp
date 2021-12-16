#include "gx-dxr-descriptor.hpp"

#ifdef GX_RENDER_DXR_ENABLED
#include "../core/gx-cr-allocator.hpp"
#include "../core/macro/gx-cr-mcr-zeroer.hpp"
#include "gx-dxr-check.hpp"
#include "gx-dxr-device.hpp"
#include "gx-dxr-texture.hpp"
#include "shaders/gx-dxr-shd-common.hpp"

gearoenix::dxr::DescriptorAllocator::DescriptorAllocator(Device& d, const bool for_sampler) noexcept
    : allocator(core::Allocator::construct(GPU_DESCRIPTORS_COUNT))
{
    D3D12_DESCRIPTOR_HEAP_DESC desc;
    GX_SET_ZERO(desc);
    desc.NumDescriptors = for_sampler ? SAMPLER_DESCRIPTORS_COUNT : GPU_DESCRIPTORS_COUNT;
    desc.Type = for_sampler ? D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER : D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
    desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
    const auto& dev = d.get_device();
    GX_DXR_CHECK(dev->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&heap)))

#ifdef GX_DEBUG_MODE
    GX_DXR_CHECK(heap->SetName(for_sampler ? L"main-sampler-descriptor-heap" : L"main-descriptor-heap"))
#endif

    size_increment = dev->GetDescriptorHandleIncrementSize(for_sampler ? D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER : D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
    cpu_starting_handle = heap->GetCPUDescriptorHandleForHeapStart();
    gpu_starting_handle = heap->GetGPUDescriptorHandleForHeapStart();
}

gearoenix::dxr::DescriptorAllocator::~DescriptorAllocator() noexcept = default;

gearoenix::dxr::Descriptor::Descriptor(
    D3D12_CPU_DESCRIPTOR_HANDLE&& cpu_handle,
    D3D12_GPU_DESCRIPTOR_HANDLE&& gpu_handle,
    std::shared_ptr<core::Allocator>&& allocator) noexcept
    : cpu_handle(std::move(cpu_handle))
    , gpu_handle(std::move(gpu_handle))
    , allocator(std::move(allocator))
{
}

gearoenix::dxr::Descriptor::~Descriptor() noexcept = default;

gearoenix::dxr::Descriptor::Descriptor(Descriptor&&) noexcept = default;

gearoenix::dxr::DescriptorManager::DescriptorManager(std::shared_ptr<Device> d) noexcept
    : device(std::move(d))
    , allocator(*device)
    , sampler_allocator(*device, true)
    , texture_2d_region_allocator(allocator.allocator->allocate(allocator.size_increment * GX_DXR_TEXTURE_2DS_COUNT))
    , texture_2d_region_gpu_handle { allocator.gpu_starting_handle.ptr + texture_2d_region_allocator->get_offset() }
    , samplers_region_gpu_handle { sampler_allocator.gpu_starting_handle.ptr }
{
    D3D12_SAMPLER_DESC desc;
    for (const auto& [rs, i] : TextureManager::samplers_indices) {
        const auto ptr = sampler_allocator.cpu_starting_handle.ptr + sampler_allocator.size_increment * i;
        TextureManager::convert(rs, desc);
        device->get_device()->CreateSampler(&desc, { ptr });
    }
}

gearoenix::dxr::DescriptorManager::~DescriptorManager() noexcept = default;

gearoenix::dxr::Descriptor gearoenix::dxr::DescriptorManager::allocate_texture_2d() noexcept
{
    auto alc = texture_2d_region_allocator->allocate(allocator.size_increment);
    return Descriptor(
        D3D12_CPU_DESCRIPTOR_HANDLE {
            .ptr = allocator.cpu_starting_handle.ptr + alc->get_offset(),
        },
        D3D12_GPU_DESCRIPTOR_HANDLE {
            .ptr = allocator.gpu_starting_handle.ptr + alc->get_offset(),
        },
        std::move(alc));
}

gearoenix::dxr::Descriptor gearoenix::dxr::DescriptorManager::allocate_others() noexcept
{
    auto alc = allocator.allocator->allocate(allocator.size_increment);
    return Descriptor(
        D3D12_CPU_DESCRIPTOR_HANDLE {
            .ptr = allocator.cpu_starting_handle.ptr + alc->get_offset(),
        },
        D3D12_GPU_DESCRIPTOR_HANDLE {
            .ptr = allocator.gpu_starting_handle.ptr + alc->get_offset(),
        },
        std::move(alc));
}

#endif
