#pragma once

#include "../render/gx-rnd-build-configuration.hpp"
#ifdef GX_RENDER_DIRECT3D_ENABLED
#include "gx-d3d-loader.hpp"

namespace gearoenix::d3d {
struct Adapter final {
    constexpr static D3D_FEATURE_LEVEL MINIMUM_FEATURE_LEVEL = D3D_FEATURE_LEVEL_12_1;
    GX_GET_CREF_PRV(Microsoft::WRL::ComPtr<IDXGIFactory7>, factory)
    GX_GET_CREF_PRV(Microsoft::WRL::ComPtr<IDXGIAdapter4>, adapter)

    Adapter();
};
}

#endif