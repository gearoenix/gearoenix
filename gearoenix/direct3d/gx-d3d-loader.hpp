#pragma once

#include "../render/gx-rnd-build-configuration.hpp"
#ifdef GX_RENDER_DIRECT3D_ENABLED
#include "../core/macro/gx-cr-mcr-getter-setter.hpp"
#include <d3d12.h>
#include <d3dx12.h>
#include <dxgi1_6.h>
#include <memory>
#include <wrl.h>
#endif