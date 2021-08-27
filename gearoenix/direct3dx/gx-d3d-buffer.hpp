#ifndef GEAROENIX_DIRECT3DX_BUFFER_HPP
#define GEAROENIX_DIRECT3DX_BUFFER_HPP
#include "../render/gx-rnd-build-configuration.hpp"
#ifdef GX_RENDER_DIRECT3DX_ENABLED
#include "gx-d3d-loader.hpp"

namespace gearoenix::direct3dx {
struct GpuUploadBuffer {
    GX_GET_CREF_PRT(Microsoft::WRL::ComPtr<ID3D12Resource>, resource)
protected:
    GpuUploadBuffer() noexcept = default;
    virtual ~GpuUploadBuffer() noexcept;
    void allocate(ID3D12Device* device, UINT buffer_size, LPCWSTR resource_name) noexcept;
    [[nodiscard]] uint8_t* map_cpu_write_only() noexcept;
};
}

#endif
#endif