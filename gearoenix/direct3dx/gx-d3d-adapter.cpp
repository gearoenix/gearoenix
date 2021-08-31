#include "gx-d3d-adapter.hpp"
#ifdef GX_RENDER_DIRECT3DX_ENABLED
#include "../core/gx-cr-build-configuration.hpp"
#include "../core/gx-cr-string.hpp"
#include "../core/macro/gx-cr-mcr-zeroer.hpp"
#include "gx-d3d-check.hpp"
#include <dxgidebug.h>

gearoenix::direct3dx::Adapter::Adapter() noexcept
{
#ifdef GX_DEBUG_MODE
    Microsoft::WRL::ComPtr<ID3D12Debug> debug_controller;
    if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debug_controller)))) {
        debug_controller->EnableDebugLayer();
    } else {
        GX_LOG_F("Direct3D Debug Device is not available")
    }
    Microsoft::WRL::ComPtr<IDXGIInfoQueue> dxgi_info_queue;
    if (SUCCEEDED(DXGIGetDebugInterface1(0, IID_PPV_ARGS(&dxgi_info_queue)))) {
        GX_D3D_CHECK(CreateDXGIFactory2(DXGI_CREATE_FACTORY_DEBUG, IID_PPV_ARGS(&factory)))
        dxgi_info_queue->SetBreakOnSeverity(DXGI_DEBUG_ALL, DXGI_INFO_QUEUE_MESSAGE_SEVERITY_ERROR, true);
        dxgi_info_queue->SetBreakOnSeverity(DXGI_DEBUG_ALL, DXGI_INFO_QUEUE_MESSAGE_SEVERITY_CORRUPTION, true);
        dxgi_info_queue->SetBreakOnSeverity(DXGI_DEBUG_ALL, DXGI_INFO_QUEUE_MESSAGE_SEVERITY_WARNING, true);
    }
#endif
    if (nullptr == factory) {
        GX_D3D_CHECK(CreateDXGIFactory1(IID_PPV_ARGS(&factory)))
    }
    bool adapter_not_found = true;
    for (UINT adapter_id = 0; DXGI_ERROR_NOT_FOUND != factory->EnumAdapterByGpuPreference(adapter_id, DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE, IID_PPV_ARGS(&adapter)); ++adapter_id) {
        DXGI_ADAPTER_DESC1 desc;
        GX_LOG_D("Adapter description: " << core::String::to_string(desc.Description))
        GX_D3D_CHECK(adapter->GetDesc1(&desc))
        if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE) {
            GX_LOG_D("Adapter is software.")
            continue;
        }
        Microsoft::WRL::ComPtr<ID3D12Device8> test_device;
        if (FAILED(D3D12CreateDevice(adapter.Get(), MINIMUM_FEATURE_LEVEL, IID_PPV_ARGS(&test_device)))) {
            GX_LOG_D("Adapter doesn't support minimum required feature level.")
            continue;
        }
        D3D12_FEATURE_DATA_D3D12_OPTIONS5 feature_support_data;
        GX_SET_ZERO(feature_support_data)
        if (FAILED(test_device->CheckFeatureSupport(
                D3D12_FEATURE_D3D12_OPTIONS5, &feature_support_data, sizeof(feature_support_data)))) {
            GX_LOG_D("Device doesn't support feature option 5.")
            continue;
        }
        if (feature_support_data.RaytracingTier == D3D12_RAYTRACING_TIER_NOT_SUPPORTED) {
            GX_LOG_D("Device doesn't support raytracing.")
            continue;
        }
        GX_LOG_D("Adapter with description: " << core::String::to_string(desc.Description) << " has been accepted")
        adapter_not_found = false;
        break;
    }
    if (adapter_not_found)
        GX_LOG_F("Expected adapter not found.")
}

#endif
