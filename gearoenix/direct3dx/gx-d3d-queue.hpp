#ifndef GEAROENIX_DIRECT3DX_QUEUE_HPP
#define GEAROENIX_DIRECT3DX_QUEUE_HPP
#include "../render/gx-rnd-build-configuration.hpp"
#ifdef GX_RENDER_DIRECT3DX_ENABLED
#include "gx-d3d-loader.hpp"

namespace gearoenix::direct3dx {
struct Device;
struct Queue final {
    GX_GET_CREF_PRV(std::shared_ptr<Device>, device)
    GX_GET_CREF_PRV(Microsoft::WRL::ComPtr<ID3D12CommandQueue>, command_queue)

    explicit Queue(std::shared_ptr<Device> device) noexcept;
};
}

#endif
#endif