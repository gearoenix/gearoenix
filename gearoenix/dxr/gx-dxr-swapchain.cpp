#include "gx-dxr-swapchain.hpp"
#ifdef GX_RENDER_DXR_ENABLED
#include "../core/macro/gx-cr-mcr-assert.hpp"
#include "../core/macro/gx-cr-mcr-zeroer.hpp"
#include "../platform/gx-plt-application.hpp"
#include "gx-dxr-adapter.hpp"
#include "gx-dxr-check.hpp"
#include "gx-dxr-device.hpp"
#include "gx-dxr-queue.hpp"
#include <d3dx12.h>

gearoenix::dxr::Swapchain::Swapchain(std::shared_ptr<Queue> q) noexcept
    : queue(std::move(q))
    , device(queue->get_device())
    , viewport {}
    , scissor {}
    , clear_colour { 0.3f, 0.15f, 0.115f, 1.0f }
{
    auto* const dev = device->get_device().Get();
    D3D12_DESCRIPTOR_HEAP_DESC rtv_descriptor_heap_desc;
    GX_SET_ZERO(rtv_descriptor_heap_desc)
    rtv_descriptor_heap_desc.NumDescriptors = GX_DXR_FRAMES_BACKBUFFER_NUMBER;
    rtv_descriptor_heap_desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
    GX_DXR_CHECK(dev->CreateDescriptorHeap(&rtv_descriptor_heap_desc, IID_PPV_ARGS(&rtv_descriptor_heap)))
    rtv_descriptor_size = dev->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
    D3D12_DESCRIPTOR_HEAP_DESC dsv_descriptor_heap_desc;
    GX_SET_ZERO(dsv_descriptor_heap_desc)
    dsv_descriptor_heap_desc.NumDescriptors = 1;
    dsv_descriptor_heap_desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
    GX_DXR_CHECK(dev->CreateDescriptorHeap(&dsv_descriptor_heap_desc, IID_PPV_ARGS(&dsv_descriptor_heap)))
    // Create a fence for tracking GPU execution progress.
    GX_DXR_CHECK(dev->CreateFence(current_fence_value, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence)))
    for (UINT i = 1; i <= GX_DXR_FRAMES_BACKBUFFER_NUMBER; ++i)
        fence_values.push(current_fence_value);
    fence_event.Attach(CreateEvent(nullptr, FALSE, FALSE, nullptr));
    GX_ASSERT(fence_event.IsValid())
    GX_SET_ZERO(viewport)
    GX_SET_ZERO(scissor)
}

gearoenix::dxr::Swapchain::~Swapchain() noexcept = default;

bool gearoenix::dxr::Swapchain::set_window_size(const platform::Application& platform_application) noexcept
{
    auto* const dev = device->get_device().Get();
    const auto& base_plt_app = platform_application.get_base();
    const auto window_width = static_cast<UINT>(base_plt_app.get_window_width());
    const auto window_height = static_cast<UINT>(base_plt_app.get_window_height());
    wait_for_gpu();
    // Release resources that are tied to the swap chain
    for (auto& frame : frames) {
        frame.render_target.Reset();
    }
    // If the swap chain already exists, resize it, otherwise create one.
    if (swapchain) {
        // If the swap chain already exists, resize it.
        HRESULT hr = swapchain->ResizeBuffers(
            GX_DXR_FRAMES_BACKBUFFER_NUMBER,
            window_width,
            window_height,
            BACK_BUFFER_FORMAT,
            0);

        if (hr == DXGI_ERROR_DEVICE_REMOVED || hr == DXGI_ERROR_DEVICE_RESET) {
            GX_LOG_D("Device lost on resize.")
            return true;
        }
        GX_DXR_CHECK(hr)
    } else {
        // Create a descriptor for the swap chain.
        DXGI_SWAP_CHAIN_DESC1 swap_chain_desc;
        GX_SET_ZERO(swap_chain_desc)
        swap_chain_desc.Width = window_width;
        swap_chain_desc.Height = window_height;
        swap_chain_desc.Format = BACK_BUFFER_FORMAT;
        swap_chain_desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        swap_chain_desc.BufferCount = GX_DXR_FRAMES_BACKBUFFER_NUMBER;
        swap_chain_desc.SampleDesc.Count = 1;
        swap_chain_desc.SampleDesc.Quality = 0;
        swap_chain_desc.Scaling = DXGI_SCALING_STRETCH;
        swap_chain_desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
        swap_chain_desc.AlphaMode = DXGI_ALPHA_MODE_IGNORE;

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
        GX_DXR_CHECK(factory->CreateSwapChainForHwnd(
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
        GX_DXR_CHECK(base_swapchain.As(&swapchain))
    }
    // Obtain the back buffers for this window which will be the final render targets
    // and create render target views for each of them.
    for (UINT n = 0; n < GX_DXR_FRAMES_BACKBUFFER_NUMBER; ++n) {
        auto& frame = frames[n];
        auto& render_target = frame.render_target;
        auto& rtv_descriptor = frame.rtv_descriptor;
        GX_DXR_CHECK(swapchain->GetBuffer(n, IID_PPV_ARGS(&render_target)))
        const auto name = std::wstring(L"Render target ") + std::to_wstring(n);
        render_target->SetName(name.c_str());

        D3D12_RENDER_TARGET_VIEW_DESC rtv_desc;
        GX_SET_ZERO(rtv_desc)
        rtv_desc.Format = BACK_BUFFER_FORMAT;
        rtv_desc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;

        rtv_descriptor = CD3DX12_CPU_DESCRIPTOR_HANDLE(
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
    GX_DXR_CHECK(dev->CreateCommittedResource(
        &depth_heap_properties,
        D3D12_HEAP_FLAG_NONE,
        &depth_stencil_desc,
        D3D12_RESOURCE_STATE_DEPTH_WRITE,
        &depth_optimized_clear_value,
        IID_PPV_ARGS(&depth_stencil)))
    depth_stencil->SetName(L"DepthStencil");
    D3D12_DEPTH_STENCIL_VIEW_DESC dsv_desc;
    GX_SET_ZERO(dsv_desc)
    dsv_desc.Format = DEPTH_BUFFER_FORMAT;
    dsv_desc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
    dev->CreateDepthStencilView(depth_stencil.Get(), &dsv_desc, dsv_descriptor_heap->GetCPUDescriptorHandleForHeapStart());
    // Set the 3D rendering viewport and scissor rectangle to target the entire window.
    viewport.TopLeftX = viewport.TopLeftY = 0.f;
    viewport.Width = static_cast<FLOAT>(window_width);
    viewport.Height = static_cast<FLOAT>(window_height);
    viewport.MinDepth = D3D12_MIN_DEPTH;
    viewport.MaxDepth = D3D12_MAX_DEPTH;
    scissor.left = scissor.top = 0;
    scissor.right = static_cast<LONG>(window_width);
    scissor.bottom = static_cast<LONG>(window_height);
    return false;
}

void gearoenix::dxr::Swapchain::wait_for_gpu() noexcept
{
    GX_ASSERT(nullptr != fence && fence_event.IsValid())
    GX_DXR_CHECK(queue->get_command_queue()->Signal(fence.Get(), ++current_fence_value))
    GX_DXR_CHECK(fence->SetEventOnCompletion(current_fence_value, fence_event.Get()))
    GX_ASSERT(WAIT_OBJECT_0 == WaitForSingleObjectEx(fence_event.Get(), INFINITE, FALSE))
    for (; !fence_values.empty(); fence_values.pop())
        ;
    for (UINT fi = 0; fi < GX_DXR_FRAMES_BACKBUFFER_NUMBER; ++fi)
        fence_values.push(current_fence_value);
}

const Microsoft::WRL::ComPtr<ID3D12Resource>& gearoenix::dxr::Swapchain::get_current_render_target() const noexcept
{
    return frames[back_buffer_index].render_target;
}

void gearoenix::dxr::Swapchain::transit_to_target(ID3D12GraphicsCommandList6* const cmd) noexcept
{
    D3D12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(
        frames[back_buffer_index].render_target.Get(),
        D3D12_RESOURCE_STATE_PRESENT,
        D3D12_RESOURCE_STATE_RENDER_TARGET);
    cmd->ResourceBarrier(1, &barrier);
}

void gearoenix::dxr::Swapchain::prepare(ID3D12GraphicsCommandList6* const cmd) noexcept
{
    auto& frame = frames[back_buffer_index];
    cmd->RSSetViewports(1, &viewport);
    cmd->RSSetScissorRects(1, &scissor);
    cmd->OMSetRenderTargets(1, &frame.rtv_descriptor, false, nullptr);
}

void gearoenix::dxr::Swapchain::clear(ID3D12GraphicsCommandList6* const cmd) noexcept
{
    cmd->ClearRenderTargetView(frames[back_buffer_index].rtv_descriptor, clear_colour, 0, nullptr);
}

void gearoenix::dxr::Swapchain::transit_to_present(ID3D12GraphicsCommandList6* const cmd) noexcept
{
    const auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(
        frames[back_buffer_index].render_target.Get(),
        D3D12_RESOURCE_STATE_RENDER_TARGET,
        D3D12_RESOURCE_STATE_PRESENT);
    cmd->ResourceBarrier(1, &barrier);
}

bool gearoenix::dxr::Swapchain::present() noexcept
{
    const auto hr = swapchain->Present(1, 0);
    // If the device was reset we must completely reinitialize the renderer.
    if (hr == DXGI_ERROR_DEVICE_REMOVED || hr == DXGI_ERROR_DEVICE_RESET) {
        GX_LOG_D("Device lost.")
        return true;
    } else {
        GX_DXR_CHECK(hr)
        move_to_next_frame();
    }
    return false;
}

void gearoenix::dxr::Swapchain::move_to_next_frame() noexcept
{
    GX_DXR_CHECK(queue->get_command_queue()->Signal(fence.Get(), ++current_fence_value));
    // Update the back buffer index.
    back_buffer_index = swapchain->GetCurrentBackBufferIndex();
    // If the next frame is not ready to be rendered yet, wait until it is ready.
    if (fence->GetCompletedValue() < fence_values.front()) {
        GX_DXR_CHECK(fence->SetEventOnCompletion(fence_values.front(), fence_event.Get()))
        GX_ASSERT(WAIT_OBJECT_0 != WaitForSingleObjectEx(fence_event.Get(), INFINITE, FALSE))
    }
    fence_values.pop();
    fence_values.push(current_fence_value);
}

#endif