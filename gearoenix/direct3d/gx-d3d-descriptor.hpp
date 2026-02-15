#pragma once

#include "../render/gx-rnd-build-configuration.hpp"
#ifdef GX_RENDER_DIRECT3D_ENABLED
#include "gx-d3d-loader.hpp"

namespace gearoenix::core {
struct Allocator;
};

namespace gearoenix::d3d {

constexpr UINT GPU_DESCRIPTORS_COUNT = 1000000;
constexpr UINT SAMPLER_DESCRIPTORS_COUNT = 16;
constexpr UINT RTV_DESCRIPTORS_COUNT = 16;
constexpr UINT DSV_DESCRIPTORS_COUNT = 16;

struct Device;

struct DescriptorAllocator final {
    Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> heap;
    std::shared_ptr<core::Allocator> allocator;
    D3D12_CPU_DESCRIPTOR_HANDLE cpu_starting_handle {};
    D3D12_GPU_DESCRIPTOR_HANDLE gpu_starting_handle {};
    UINT size_increment = 0;

    explicit DescriptorAllocator(Device& d, UINT descriptors_count, D3D12_DESCRIPTOR_HEAP_TYPE descriptor_type, const wchar_t* name);
    ~DescriptorAllocator();
};

struct CpuDescriptorAllocator final {
    Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> heap;
    std::shared_ptr<core::Allocator> allocator;
    D3D12_CPU_DESCRIPTOR_HANDLE cpu_starting_handle {};
    UINT size_increment = 0;

    explicit CpuDescriptorAllocator(Device& d, UINT descriptors_count, D3D12_DESCRIPTOR_HEAP_TYPE descriptor_type, const wchar_t* name);
    ~CpuDescriptorAllocator();
};

struct Descriptor final {
    friend struct DescriptorManager;

    const UINT resource_index;
    const D3D12_CPU_DESCRIPTOR_HANDLE cpu_handle;
    const D3D12_GPU_DESCRIPTOR_HANDLE gpu_handle;

private:
    std::shared_ptr<core::Allocator> allocator;

    Descriptor(UINT resource_index, D3D12_CPU_DESCRIPTOR_HANDLE&& cpu_handle, D3D12_GPU_DESCRIPTOR_HANDLE&& gpu_handle, std::shared_ptr<core::Allocator>&& allocator);

public:
    ~Descriptor();
    Descriptor(Descriptor&&);
};

struct CpuDescriptor final {
    friend struct DescriptorManager;

    const UINT resource_index;
    const D3D12_CPU_DESCRIPTOR_HANDLE cpu_handle;

private:
    std::shared_ptr<core::Allocator> allocator;

    CpuDescriptor(UINT resource_index, D3D12_CPU_DESCRIPTOR_HANDLE&& cpu_handle, std::shared_ptr<core::Allocator>&& allocator);

public:
    ~CpuDescriptor();
    CpuDescriptor(CpuDescriptor&&);
};

struct DescriptorManager final {
    GX_GET_CREF_PRV(std::shared_ptr<Device>, device)
    GX_GET_CREF_PRV(DescriptorAllocator, allocator)
    GX_GET_CREF_PRV(DescriptorAllocator, sampler_allocator)
    GX_GET_CREF_PRV(CpuDescriptorAllocator, rtv_allocator)
    GX_GET_CREF_PRV(CpuDescriptorAllocator, dsv_allocator)
    GX_GET_REFC_PRV(std::shared_ptr<core::Allocator>, texture_2d_region_allocator)
    GX_GET_REFC_PRV(D3D12_GPU_DESCRIPTOR_HANDLE, texture_2d_region_gpu_handle)
    GX_GET_REFC_PRV(D3D12_GPU_DESCRIPTOR_HANDLE, samplers_region_gpu_handle)

    explicit DescriptorManager(std::shared_ptr<Device> device);
    ~DescriptorManager();
    [[nodiscard]] Descriptor allocate_texture_2d();
    [[nodiscard]] Descriptor allocate_others();
    [[nodiscard]] CpuDescriptor allocate_rtv();
    [[nodiscard]] CpuDescriptor allocate_dsv();
};
}

#endif