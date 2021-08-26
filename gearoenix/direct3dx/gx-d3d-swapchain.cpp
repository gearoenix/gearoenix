#include "gx-d3d-swapchain.hpp"
#ifdef GX_RENDER_DIRECT3DX_ENABLED
#include "../core/macro/gx-cr-mcr-zeroer.hpp"
#include "../platform/gx-plt-application.hpp"
#include "gx-d3d-adapter.hpp"
#include "gx-d3d-check.hpp"
#include "gx-d3d-device.hpp"
#include "gx-d3d-queue.hpp"
#include <d3dx12.h>

gearoenix::direct3dx::Swapchain::Swapchain(std::shared_ptr<Queue> q) noexcept
    : queue(std::move(q))
    , device(queue->get_device())
    , screen_viewport {}
    , scissor_rect {}
{
    auto* const dev = device->get_device().Get();
    D3D12_DESCRIPTOR_HEAP_DESC rtv_descriptor_heap_desc;
    GX_SET_ZERO(rtv_descriptor_heap_desc)
    rtv_descriptor_heap_desc.NumDescriptors = BACK_BUFFERS_COUNT;
    rtv_descriptor_heap_desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
    GX_D3D_CHECK(dev->CreateDescriptorHeap(&rtv_descriptor_heap_desc, IID_PPV_ARGS(&rtv_descriptor_heap)))
    rtv_descriptor_size = dev->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
    D3D12_DESCRIPTOR_HEAP_DESC dsv_descriptor_heap_desc;
    GX_SET_ZERO(dsv_descriptor_heap_desc)
    dsv_descriptor_heap_desc.NumDescriptors = 1;
    dsv_descriptor_heap_desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
    GX_D3D_CHECK(dev->CreateDescriptorHeap(&dsv_descriptor_heap_desc, IID_PPV_ARGS(&dsv_descriptor_heap)))
    for (auto& frame : frames) {
        GX_D3D_CHECK(dev->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&frame.command_allocator)))
    }
    GX_D3D_CHECK(dev->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, frames[0].command_allocator.Get(), nullptr, IID_PPV_ARGS(&command_list)))
    GX_D3D_CHECK(command_list->Close())
    // Create a fence for tracking GPU execution progress.
    GX_D3D_CHECK(dev->CreateFence(frames[back_buffer_index].fence_value, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence)))
    ++frames[back_buffer_index].fence_value;
    fence_event.Attach(CreateEvent(nullptr, FALSE, FALSE, nullptr));
    if (!fence_event.IsValid())
        GX_LOG_F("CreateEvent failed.")
    GX_SET_ZERO(screen_viewport)
    GX_SET_ZERO(scissor_rect)
}

gearoenix::direct3dx::Swapchain::~Swapchain() noexcept = default;

bool gearoenix::direct3dx::Swapchain::set_window_size(const platform::Application& platform_application) noexcept
{
    auto* const dev = device->get_device().Get();
    const auto& base_plt_app = platform_application.get_base();
    const auto window_width = static_cast<UINT>(base_plt_app.get_window_width());
    const auto window_height = static_cast<UINT>(base_plt_app.get_window_height());
    wait_for_gpu();
    // Release resources that are tied to the swap chain and update fence values.
    for (auto& frame : frames) {
        frame.render_target.Reset();
        frame.fence_value = frames[back_buffer_index].fence_value;
    }
    // If the swap chain already exists, resize it, otherwise create one.
    if (swapchain) {
        // If the swap chain already exists, resize it.
        HRESULT hr = swapchain->ResizeBuffers(
            BACK_BUFFERS_COUNT,
            window_width,
            window_height,
            BACK_BUFFER_FORMAT,
            DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING);

        if (hr == DXGI_ERROR_DEVICE_REMOVED || hr == DXGI_ERROR_DEVICE_RESET) {
            GX_LOG_D("Device lost on resize.")
            return true;
        }
        GX_D3D_CHECK(hr)
    } else {
        // Create a descriptor for the swap chain.
        DXGI_SWAP_CHAIN_DESC1 swap_chain_desc;
        GX_SET_ZERO(swap_chain_desc)
        swap_chain_desc.Width = window_width;
        swap_chain_desc.Height = window_height;
        swap_chain_desc.Format = BACK_BUFFER_FORMAT;
        swap_chain_desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        swap_chain_desc.BufferCount = BACK_BUFFERS_COUNT;
        swap_chain_desc.SampleDesc.Count = 1;
        swap_chain_desc.SampleDesc.Quality = 0;
        swap_chain_desc.Scaling = DXGI_SCALING_STRETCH;
        swap_chain_desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
        swap_chain_desc.AlphaMode = DXGI_ALPHA_MODE_IGNORE;
        swap_chain_desc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING;

        DXGI_SWAP_CHAIN_FULLSCREEN_DESC fs_swapchain_desc;
        GX_SET_ZERO(fs_swapchain_desc)
        fs_swapchain_desc.Windowed = TRUE;
        GX_TODO // We have to make sure the following commented is not necessary
            //- DXGI does not allow creating a swapchain targeting a window which has fullscreen styles(no border + topmost).
            //- Temporarily remove the topmost property for creating the swapchain.
            // bool prevIsFullscreen = Win32Application::IsFullscreen();
            // if (prevIsFullscreen) {
            //    Win32Application::SetWindowZorderToTopMost(false);
            // }
            const auto& factory
            = device->get_adapter()->get_factory();
        Microsoft::WRL::ComPtr<IDXGISwapChain1> base_swapchain;
        GX_D3D_CHECK(factory->CreateSwapChainForHwnd(
            queue->get_command_queue().Get(),
            platform_application.get_window(),
            &swap_chain_desc,
            &fs_swapchain_desc,
            nullptr,
            &base_swapchain))
        GX_TODO // We have to make sure the following commented is not necessary
            // if (prevIsFullscreen) {
            //    Win32Application::SetWindowZorderToTopMost(true);
            // }
            GX_D3D_CHECK(base_swapchain.As(&swapchain))
            // With tearing support enabled we will handle ALT+Enter key presses in the
            // window message loop rather than let DXGI handle it by calling SetFullscreenState.
            factory->MakeWindowAssociation(platform_application.get_window(), DXGI_MWA_NO_ALT_ENTER);
    }
    // Obtain the back buffers for this window which will be the final render targets
    // and create render target views for each of them.
    for (UINT n = 0; n < BACK_BUFFERS_COUNT; ++n) {
        auto& frame = frames[n];
        auto& render_target = frame.render_target;
        GX_D3D_CHECK(swapchain->GetBuffer(n, IID_PPV_ARGS(&render_target)))
        const auto name = std::wstring(L"Render target ") + std::to_wstring(n);
        render_target->SetName(name.c_str());

        D3D12_RENDER_TARGET_VIEW_DESC rtv_desc;
        GX_SET_ZERO(rtv_desc)
        rtv_desc.Format = BACK_BUFFER_FORMAT;
        rtv_desc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;

        CD3DX12_CPU_DESCRIPTOR_HANDLE rtv_descriptor(
            rtv_descriptor_heap->GetCPUDescriptorHandleForHeapStart(),
            static_cast<INT>(n), rtv_descriptor_size);
        dev->CreateRenderTargetView(render_target.Get(), &rtv_desc, rtv_descriptor);
    }
    // Reset the index to the current back buffer.
    back_buffer_index = swapchain->GetCurrentBackBufferIndex();
    CD3DX12_HEAP_PROPERTIES depth_heap_properties(D3D12_HEAP_TYPE_DEFAULT);
    D3D12_RESOURCE_DESC depth_stencil_desc = CD3DX12_RESOURCE_DESC::Tex2D(
        DEPTH_BUFFER_FORMAT, window_width, window_height, 1, 1);
    depth_stencil_desc.Flags |= D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;
    D3D12_CLEAR_VALUE depth_optimized_clear_value;
    GX_SET_ZERO(depth_optimized_clear_value)
    depth_optimized_clear_value.Format = DEPTH_BUFFER_FORMAT;
    depth_optimized_clear_value.DepthStencil.Depth = 1.0f;
    depth_optimized_clear_value.DepthStencil.Stencil = 0;
    GX_D3D_CHECK(dev->CreateCommittedResource(
        &depth_heap_properties,
        D3D12_HEAP_FLAG_NONE,
        &depth_stencil_desc,
        D3D12_RESOURCE_STATE_DEPTH_WRITE,
        &depth_optimized_clear_value,
        IID_PPV_ARGS(&depth_stencil)))
    depth_stencil->SetName(L"Depth stencil");
    D3D12_DEPTH_STENCIL_VIEW_DESC dsv_desc;
    GX_SET_ZERO(dsv_desc)
    dsv_desc.Format = DEPTH_BUFFER_FORMAT;
    dsv_desc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
    dev->CreateDepthStencilView(depth_stencil.Get(), &dsv_desc, dsv_descriptor_heap->GetCPUDescriptorHandleForHeapStart());
    // Set the 3D rendering viewport and scissor rectangle to target the entire window.
    screen_viewport.TopLeftX = screen_viewport.TopLeftY = 0.f;
    screen_viewport.Width = static_cast<float>(window_width);
    screen_viewport.Height = static_cast<float>(window_height);
    screen_viewport.MinDepth = D3D12_MIN_DEPTH;
    screen_viewport.MaxDepth = D3D12_MAX_DEPTH;
    scissor_rect.left = scissor_rect.top = 0;
    scissor_rect.right = static_cast<LONG>(window_width);
    scissor_rect.bottom = static_cast<LONG>(window_height);
    return false;
}

void gearoenix::direct3dx::Swapchain::wait_for_gpu() noexcept
{
    if (nullptr == fence || !fence_event.IsValid()) {
        GX_LOG_D("wait_for_gpu on invalid state.")
        return;
    }
    // Schedule a Signal command in the GPU queue.
    const UINT64 fence_value = frames[back_buffer_index].fence_value;
    if (FAILED(queue->get_command_queue()->Signal(fence.Get(), fence_value))) {
        GX_LOG_D("Signal failed.")
        return;
    }
    // Wait until the Signal has been processed.
    if (FAILED(fence->SetEventOnCompletion(fence_value, fence_event.Get()))) {
        GX_LOG_D("Wait failed.")
        return;
    }
    WaitForSingleObjectEx(fence_event.Get(), INFINITE, FALSE);
    // Increment the fence value for the current frame.
    ++frames[back_buffer_index].fence_value;
}

const Microsoft::WRL::ComPtr<ID3D12CommandAllocator> gearoenix::direct3dx::Swapchain::get_current_command_allocator() const noexcept
{
    return frames[back_buffer_index].command_allocator;
}

#endif