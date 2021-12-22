#include "gx-dxr-uploader.hpp"

#ifdef GX_RENDER_DXR_ENABLED
#include "../core/macro/gx-cr-mcr-zeroer.hpp"
#include "gx-dxr-buffer.hpp"
#include "gx-dxr-check.hpp"
#include "gx-dxr-device.hpp"
#include "gx-dxr-queue.hpp"
#include "gx-dxr-texture.hpp"

void gearoenix::dxr::Uploader::wait(const UINT64 fv) noexcept
{
    auto fence_event = CreateEventW(nullptr, false, false, nullptr);
    GX_ASSERT_D(NULL != fence_event)
    GX_DXR_CHECK(fence->SetEventOnCompletion(fv, fence_event));
    GX_ASSERT_D(WAIT_OBJECT_0 == WaitForSingleObjectEx(fence_event, INFINITE, false))
    GX_ASSERT_D(0 != CloseHandle(fence_event))
}

gearoenix::dxr::Uploader::Uploader(std::shared_ptr<Device> _device) noexcept
    : device(std::move(_device))
    , copy_queue(new Queue(device, Queue::Type::Copy))
    , direct_queue(new Queue(device, Queue::Type::Direct)) {
        GX_DXR_CHECK(device->get_device()->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence)))
    }

    gearoenix::dxr::Uploader::~Uploader() noexcept = default;

void gearoenix::dxr::Uploader::upload(
    std::vector<std::uint8_t>&& data,
    std::shared_ptr<GpuBuffer>&& buffer,
    core::sync::EndCallerIgnored&& end) noexcept
{
    uploader.push([this, data = std::move(data), buffer = std::move(buffer), end = std::move(end)]() {
        const auto& dev = device->get_device();
        Microsoft::WRL::ComPtr<ID3D12CommandAllocator> cal;
        Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList6> cmd;
        GX_DXR_CHECK(dev->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_COPY, IID_PPV_ARGS(&cal)))
        GX_DXR_CHECK(dev->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_COPY, cal.Get(), nullptr, IID_PPV_ARGS(&cmd)))
        auto cb = std::make_shared<CpuBuffer>(dev.Get(), static_cast<UINT>(data.size()), nullptr);
        cb->copy(data.data(), data.size());
        cmd->CopyBufferRegion(buffer->get_resource().Get(), 0, cb->get_resource().Get(), 0, static_cast<UINT>(data.size()));
        const auto t = CD3DX12_RESOURCE_BARRIER::Transition(buffer->get_resource().Get(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_COMMON);
        cmd->ResourceBarrier(1, &t);
        GX_DXR_CHECK(cmd->Close())
        ID3D12CommandList* const cmds[] = { static_cast<ID3D12CommandList*>(cmd.Get()) };
        copy_queue->get_command_queue()->ExecuteCommandLists(1, cmds);
        const auto current_fence_value = fence_value;
        ++fence_value;
        GX_DXR_CHECK(copy_queue->get_command_queue()->Signal(fence.Get(), current_fence_value))
        uploader.push([this,
                          buffer = std::move(buffer),
                          cb = std::move(cb),
                          end = std::move(end),
                          cmd = std::move(cmd),
                          cal = std::move(cal),
                          current_fence_value]() {
            wait(current_fence_value);
            (void)buffer;
            (void)end;
            (void)cb;
            (void)cmd;
            (void)cal;
        });
    });
}

void gearoenix::dxr::Uploader::upload(
    std::vector<std::uint8_t>&& data,
    std::shared_ptr<Texture2D>&& texture,
    const UINT subresource_index,
    core::sync::EndCallerIgnored end) noexcept
{
    uploader.push([this, data = std::move(data), texture = std::move(texture), end = std::move(end), subresource_index]() {
        const auto desc = texture->get_resource()->GetDesc();
        const auto dev = device->get_device();
        Microsoft::WRL::ComPtr<ID3D12CommandAllocator> cal;
        Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList6> cmd;
        Microsoft::WRL::ComPtr<ID3D12CommandAllocator> gcal;
        Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList6> gcmd;
        GX_DXR_CHECK(dev->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_COPY, IID_PPV_ARGS(&cal)))
        GX_DXR_CHECK(dev->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_COPY, cal.Get(), nullptr, IID_PPV_ARGS(&cmd)))
        GX_DXR_CHECK(dev->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&gcal)))
        GX_DXR_CHECK(dev->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, gcal.Get(), nullptr, IID_PPV_ARGS(&gcmd)))
        const UINT64 total_bytes = GetRequiredIntermediateSize(texture->get_resource().Get(), 0, 1);
        auto cb = std::make_shared<CpuBuffer>(dev.Get(), static_cast<UINT>(total_bytes), nullptr);
        D3D12_SUBRESOURCE_DATA texture_data;
        GX_SET_ZERO(texture_data)
        texture_data.pData = data.data();
        texture_data.RowPitch = data.size() / desc.Height;
        texture_data.SlicePitch = texture_data.RowPitch * desc.Height;
        UpdateSubresources(cmd.Get(), texture->get_resource().Get(), cb->get_resource().Get(), 0, subresource_index, 1, &texture_data);
        GX_DXR_CHECK(cmd->Close())
        copy_queue->exe(cmd);
        GX_DXR_CHECK(copy_queue->get_command_queue()->Signal(fence.Get(), fence_value))
        GX_DXR_CHECK(direct_queue->get_command_queue()->Wait(fence.Get(), fence_value))
        ++fence_value;
        const auto t1 = CD3DX12_RESOURCE_BARRIER::Transition(texture->get_resource().Get(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_ALL_SHADER_RESOURCE);
        gcmd->ResourceBarrier(1, &t1);
        GX_DXR_CHECK(gcmd->Close())
        direct_queue->exe(gcmd);
        const auto current_fence_value = fence_value;
        ++fence_value;
        GX_DXR_CHECK(direct_queue->get_command_queue()->Signal(fence.Get(), current_fence_value))
        uploader.push([this,
                          texture = std::move(texture),
                          cb = std::move(cb),
                          end = std::move(end),
                          cmd = std::move(cmd),
                          cal = std::move(cal),
                          gcmd = std::move(gcmd),
                          gcal = std::move(gcal),
                          current_fence_value]() {
            wait(current_fence_value);
            (void)texture;
            (void)end;
            (void)cb;
            (void)cmd;
            (void)cal;
            (void)gcmd;
            (void)gcal;
        });
    });
}

#endif