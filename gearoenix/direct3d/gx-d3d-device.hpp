#pragma once

#include "../render/gx-rnd-build-configuration.hpp"
#ifdef GX_RENDER_DIRECT3D_ENABLED
#include "gx-d3d-loader.hpp"

namespace gearoenix::platform {
struct Application;
}

namespace gearoenix::d3d {
struct Adapter;
struct Device final {
    GX_GET_REFC_PRV(std::shared_ptr<Adapter>, adapter)
    GX_GET_CREF_PRV(Microsoft::WRL::ComPtr<ID3D12Device8>, device)

    explicit Device(std::shared_ptr<Adapter> adapter) noexcept;
};
}

#endif