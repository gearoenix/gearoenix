#pragma once
#include "../render/gx-rnd-build-configuration.hpp"
#ifdef GX_RENDER_DXR_ENABLED
#include "gx-dxr-loader.hpp"

namespace gearoenix::dxr {
struct Device;
struct Queue final {
    enum struct Type {
        Copy,
        Direct,
        Compute,
    };

    GX_GET_CREF_PRV(std::shared_ptr<Device>, device)
    GX_GET_CREF_PRV(Microsoft::WRL::ComPtr<ID3D12CommandQueue>, command_queue)

    explicit Queue(std::shared_ptr<Device> device, Type ty) noexcept;
    ~Queue() noexcept;

    void exe(const Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList6>& cmd) noexcept;
    void exe(ID3D12CommandList** cmds, UINT count) noexcept;
};
}

#endif