#pragma once

#include "../render/gx-rnd-build-configuration.hpp"
#ifdef GX_RENDER_DIRECT3D_ENABLED
#include "gx-d3d-build-configuration.hpp"
#include "gx-d3d-loader.hpp"
#include <queue>

namespace gearoenix::platform {
struct Application;
}

namespace gearoenix::d3d {
struct Device;
struct Queue;
struct CpuDescriptor;
struct DescriptorManager;
struct Engine;
struct Swapchain final {
    constexpr static DXGI_FORMAT BACK_BUFFER_FORMAT = DXGI_FORMAT_B8G8R8A8_UNORM;
    constexpr static DXGI_FORMAT DEPTH_BUFFER_FORMAT = DXGI_FORMAT_D32_FLOAT;

    struct Frame final {
        Microsoft::WRL::ComPtr<ID3D12Resource> render_target;
        std::unique_ptr<CpuDescriptor> render_target_decriptor;
    };

    GX_GET_REFC_PRV(std::shared_ptr<Queue>, queue)
    GX_GET_REFC_PRV(std::shared_ptr<Device>, device)
    GX_GET_REFC_PRV(std::shared_ptr<DescriptorManager>, descriptor_manager)
    GX_GET_CREF_PRV(Microsoft::WRL::ComPtr<ID3D12Fence>, fence)
    GX_GET_CREF_PRV(Microsoft::WRL::ComPtr<IDXGISwapChain4>, swapchain)
    GX_GET_CREF_PRV(Microsoft::WRL::ComPtr<ID3D12Resource>, depth_stencil)
    GX_GET_UPTR_PRV(CpuDescriptor, depth_stencil_descriptor)
    GX_GET_CREF_PRV(Microsoft::WRL::Wrappers::Event, fence_event)
    GX_GET_CREF_PRV(D3D12_VIEWPORT, viewport)
    GX_GET_CREF_PRV(D3D12_RECT, scissor)
    GX_GET_ARRC_PRV(Frame, frames, GX_D3D_FRAMES_BACKBUFFER_NUMBER)
    GX_GET_ARRC_PRV(float, clear_colour, 4)
    GX_GET_VAL_PRV(UINT, back_buffer_index, 0)

    explicit Swapchain(Engine& e);
    ~Swapchain();
    /// Returns true if device is lost.
    [[nodiscard]] bool set_window_size(const platform::Application&);
    void wait_for_gpu();
    [[nodiscard]] const Microsoft::WRL::ComPtr<ID3D12Resource>& get_current_render_target() const;
    void transit_to_target(ID3D12GraphicsCommandList6* cmd);
    void prepare(ID3D12GraphicsCommandList6* cmd);
    void clear(ID3D12GraphicsCommandList6* cmd);
    void transit_to_present(ID3D12GraphicsCommandList6* cmd);
    [[nodiscard]] bool present();

private:
    void move_to_next_frame();

    std::queue<UINT64> fence_values;
    UINT64 current_fence_value = 0;
};
}

#endif