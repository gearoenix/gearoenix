#ifndef GEAROENIX_DIRECT3DX_DEVICE_HPP
#define GEAROENIX_DIRECT3DX_DEVICE_HPP
#include "../render/gx-rnd-build-configuration.hpp"
#ifdef GX_RENDER_DIRECT3DX_ENABLED
#include "gx-d3d-loader.hpp"

namespace gearoenix::platform {
struct Application;
}

namespace gearoenix::direct3dx {
struct Adapter;
struct Device final {
    GX_GET_REFC_PRV(std::shared_ptr<Adapter>, adapter)
    GX_GET_CREF_PRV(Microsoft::WRL::ComPtr<ID3D12Device>, device)

    explicit Device(std::shared_ptr<Adapter> adapter) noexcept;

    void create_window_size_dependent_resources() noexcept;
    void wait_for_gpu() noexcept;
};
}

#endif
#endif