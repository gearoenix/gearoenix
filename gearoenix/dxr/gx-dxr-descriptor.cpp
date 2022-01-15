#include "gx-dxr-descriptor.hpp"

#ifdef GX_RENDER_DXR_ENABLED
#include "../core/gx-cr-allocator.hpp"
#include "../core/macro/gx-cr-mcr-zeroer.hpp"
#include "gx-dxr-check.hpp"
#include "gx-dxr-device.hpp"
#include "gx-dxr-texture.hpp"
#include "shaders/gx-dxr-shd-common.hpp"

gearoenix::dxr::DescriptorAllocator::DescriptorAllocator(
    Device& d,
    const UINT descriptors_count,
    const D3D12_DESCRIPTOR_HEAP_TYPE descriptor_type,
    const wchar_t* const name) noexcept
    : allocator(core::Allocator::construct(descriptors_count))
{
    D3D12_DESCRIPTOR_HEAP_DESC desc;
    GX_SET_ZERO(desc);
    desc.NumDescriptors = descriptors_count;
    desc.Type = descriptor_type;
    desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
    const auto& dev = d.get_device();
    GX_DXR_CHECK(dev->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&heap)))

#ifdef GX_DEBUG_MODE
    GX_DXR_CHECK(heap->SetName(name))
#endif

    size_increment = dev->GetDescriptorHandleIncrementSize(descriptor_type);
    cpu_starting_handle = heap->GetCPUDescriptorHandleForHeapStart();
    gpu_starting_handle = heap->GetGPUDescriptorHandleForHeapStart();
}

gearoenix::dxr::DescriptorAllocator::~DescriptorAllocator() noexcept = default;

gearoenix::dxr::CpuDescriptorAllocator::CpuDescriptorAllocator(
    Device& d,
    const UINT descriptors_count,
    const D3D12_DESCRIPTOR_HEAP_TYPE descriptor_type,
    const wchar_t* const name) noexcept
    : allocator(core::Allocator::construct(descriptors_count))
{
    D3D12_DESCRIPTOR_HEAP_DESC desc;
    GX_SET_ZERO(desc);
    desc.NumDescriptors = descriptors_count;
    desc.Type = descriptor_type;
    const auto& dev = d.get_device();
    GX_DXR_CHECK(dev->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&heap)))

#ifdef GX_DEBUG_MODE
    GX_DXR_CHECK(heap->SetName(name))
#endif

    size_increment = dev->GetDescriptorHandleIncrementSize(descriptor_type);
    cpu_starting_handle = heap->GetCPUDescriptorHandleForHeapStart();
}

gearoenix::dxr::CpuDescriptorAllocator::~CpuDescriptorAllocator() noexcept = default;

gearoenix::dxr::Descriptor::Descriptor(
    const UINT resource_index,
    D3D12_CPU_DESCRIPTOR_HANDLE&& cpu_handle,
    D3D12_GPU_DESCRIPTOR_HANDLE&& gpu_handle,
    std::shared_ptr<core::Allocator>&& allocator) noexcept
    : resource_index(resource_index)
    , cpu_handle(std::move(cpu_handle))
    , gpu_handle(std::move(gpu_handle))
    , allocator(std::move(allocator))
{
}

gearoenix::dxr::Descriptor::~Descriptor() noexcept = default;

gearoenix::dxr::Descriptor::Descriptor(Descriptor&&) noexcept = default;

gearoenix::dxr::CpuDescriptor::CpuDescriptor(
    const UINT resource_index,
    D3D12_CPU_DESCRIPTOR_HANDLE&& cpu_handle,
    std::shared_ptr<core::Allocator>&& allocator) noexcept
    : resource_index(resource_index)
    , cpu_handle(std::move(cpu_handle))
    , allocator(std::move(allocator))
{
}

gearoenix::dxr::CpuDescriptor::~CpuDescriptor() noexcept = default;

gearoenix::dxr::CpuDescriptor::CpuDescriptor(CpuDescriptor&&) noexcept = default;

gearoenix::dxr::DescriptorManager::DescriptorManager(std::shared_ptr<Device> d) noexcept
    : device(std::move(d))
    , allocator(*device, GPU_DESCRIPTORS_COUNT, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, L"main-descriptor-heap")
    , sampler_allocator(*device, SAMPLER_DESCRIPTORS_COUNT, D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER, L"main-sampler-descriptor-heap")
    , rtv_allocator(*device, RTV_DESCRIPTORS_COUNT, D3D12_DESCRIPTOR_HEAP_TYPE_RTV, L"main-rtv-descriptor-heap")
    , dsv_allocator(*device, DSV_DESCRIPTORS_COUNT, D3D12_DESCRIPTOR_HEAP_TYPE_DSV, L"main-dsv-descriptor-heap")
    , texture_2d_region_allocator(allocator.allocator->allocate(GX_DXR_TEXTURE_2DS_COUNT))
    , texture_2d_region_gpu_handle { allocator.gpu_starting_handle.ptr + texture_2d_region_allocator->get_offset() * allocator.size_increment }
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
    auto alc = texture_2d_region_allocator->allocate(1);
    const auto ptr_inc = alc->get_offset() * allocator.size_increment;
    return Descriptor(
        static_cast<UINT>(alc->get_offset()),
        D3D12_CPU_DESCRIPTOR_HANDLE {
            .ptr = allocator.cpu_starting_handle.ptr + ptr_inc,
        },
        D3D12_GPU_DESCRIPTOR_HANDLE {
            .ptr = allocator.gpu_starting_handle.ptr + ptr_inc,
        },
        std::move(alc));
}

gearoenix::dxr::Descriptor gearoenix::dxr::DescriptorManager::allocate_others() noexcept
{
    auto alc = allocator.allocator->allocate(1);
    const auto ptr_inc = alc->get_offset() * allocator.size_increment;
    return Descriptor(
        static_cast<UINT>(alc->get_offset()),
        D3D12_CPU_DESCRIPTOR_HANDLE {
            .ptr = allocator.cpu_starting_handle.ptr + ptr_inc,
        },
        D3D12_GPU_DESCRIPTOR_HANDLE {
            .ptr = allocator.gpu_starting_handle.ptr + ptr_inc,
        },
        std::move(alc));
}

gearoenix::dxr::CpuDescriptor gearoenix::dxr::DescriptorManager::allocate_rtv() noexcept
{
    auto alc = rtv_allocator.allocator->allocate(1);
    const auto ptr_inc = alc->get_offset() * rtv_allocator.size_increment;
    return CpuDescriptor(
        static_cast<UINT>(alc->get_offset()),
        D3D12_CPU_DESCRIPTOR_HANDLE {
            .ptr = rtv_allocator.cpu_starting_handle.ptr + ptr_inc,
        },
        std::move(alc));
}

gearoenix::dxr::CpuDescriptor gearoenix::dxr::DescriptorManager::allocate_dsv() noexcept
{
    auto alc = dsv_allocator.allocator->allocate(1);
    const auto ptr_inc = alc->get_offset() * dsv_allocator.size_increment;
    return CpuDescriptor(
        static_cast<UINT>(alc->get_offset()),
        D3D12_CPU_DESCRIPTOR_HANDLE {
            .ptr = dsv_allocator.cpu_starting_handle.ptr + ptr_inc,
        },
        std::move(alc));
}

#endif
