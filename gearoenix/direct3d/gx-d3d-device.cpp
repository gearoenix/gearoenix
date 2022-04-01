#include "gx-d3d-device.hpp"
#ifdef GX_RENDER_DIRECT3D_ENABLED
#include "../core/gx-cr-build-configuration.hpp"
#include "../core/macro/gx-cr-mcr-counter.hpp"
#include "../core/macro/gx-cr-mcr-zeroer.hpp"
#include "../platform/gx-plt-application.hpp"
#include "gx-d3d-adapter.hpp"
#include "gx-d3d-check.hpp"

gearoenix::d3d::Device::Device(std::shared_ptr<Adapter> _adapter) noexcept
    : adapter(std::move(_adapter))
{
    if (FAILED(D3D12CreateDevice(
            adapter->get_adapter().Get(), Adapter::MINIMUM_FEATURE_LEVEL, IID_PPV_ARGS(&device))))
        GX_LOG_F("Can not create device.");
#ifdef GX_DEBUG_MODE
    Microsoft::WRL::ComPtr<ID3D12InfoQueue> d3d_info_queue;
    if (SUCCEEDED(device.As(&d3d_info_queue))) {
        d3d_info_queue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, true);
        d3d_info_queue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, true);
        d3d_info_queue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_WARNING, true);
        D3D12_MESSAGE_ID hide[] {
            D3D12_MESSAGE_ID_MAP_INVALID_NULLRANGE,
            D3D12_MESSAGE_ID_UNMAP_INVALID_NULLRANGE,
            // TODO: Remove it. Workarounds for debug layer issues on hybrid-graphics systems
            D3D12_MESSAGE_ID_EXECUTECOMMANDLISTS_WRONGSWAPCHAINBUFFERREFERENCE,
            D3D12_MESSAGE_ID_RESOURCE_BARRIER_MISMATCHING_COMMAND_LIST_TYPE,
        };
        D3D12_INFO_QUEUE_FILTER filter;
        GX_SET_ZERO(filter);
        filter.DenyList.NumIDs = GX_COUNT_OF(hide);
        filter.DenyList.pIDList = hide;
        d3d_info_queue->AddStorageFilterEntries(&filter);
    }
#endif
}
#endif
