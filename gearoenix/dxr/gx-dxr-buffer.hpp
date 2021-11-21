#pragma once
#include "../render/gx-rnd-build-configuration.hpp"
#ifdef GX_RENDER_DXR_ENABLED
#include "gx-dxr-loader.hpp"

namespace gearoenix::dxr {
struct CpuBuffer final {
    GX_GET_CREF_PRV(Microsoft::WRL::ComPtr<ID3D12Resource>, resource)
    GX_GET_PTR_PRV(void, pointer)

    CpuBuffer(ID3D12Device* device, UINT buffer_size, LPCWSTR resource_name) noexcept;
    ~CpuBuffer() noexcept;
    void copy(const void* data, std::size_t size) noexcept;
};

struct GpuBuffer final {
    GX_GET_CREF_PRV(Microsoft::WRL::ComPtr<ID3D12Resource>, resource)

    GpuBuffer(ID3D12Device* device, UINT buffer_size, LPCWSTR resource_name) noexcept;
};
}
#endif