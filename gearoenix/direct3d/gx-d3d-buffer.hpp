#pragma once
#include "../render/gx-rnd-build-configuration.hpp"
#ifdef GX_RENDER_DIRECT3D_ENABLED
#include "gx-d3d-descriptor.hpp"
#include "gx-d3d-loader.hpp"

namespace gearoenix::d3d {
struct Descriptor;
struct Engine;

struct CpuBuffer final {
    GX_GET_CREF_PRV(Microsoft::WRL::ComPtr<ID3D12Resource>, resource)
    GX_GET_PTR_PRV(void, pointer)

    CpuBuffer(ID3D12Device* device, UINT buffer_size, LPCWSTR resource_name) noexcept;
    ~CpuBuffer() noexcept;
    CpuBuffer(CpuBuffer&&) noexcept;
    void copy(const void* data, std::size_t size) noexcept;
};

struct GpuBuffer final {
    GX_GET_CREF_PRV(Microsoft::WRL::ComPtr<ID3D12Resource>, resource)

    GpuBuffer(ID3D12Device* device, UINT buffer_size, LPCWSTR resource_name) noexcept;
};

struct UniformBuffer final {
    GX_GET_REF_PRV(CpuBuffer, buffer)
    GX_GET_CREF_PRV(Descriptor, descriptor)

    UniformBuffer(const Engine& e, UINT buffer_size, LPCWSTR resource_name) noexcept;
    UniformBuffer(UniformBuffer&&) noexcept;
};
}
#endif