#include "gx-dxr-command.hpp"
#ifdef GX_RENDER_DXR_ENABLED
#include "gx-dxr-check.hpp"
#include "gx-dxr-device.hpp"

gearoenix::dxr::Command::Command(Device& device, const D3D12_COMMAND_LIST_TYPE t) noexcept
{
    auto* const d = device.get_device().Get();
    GX_DXR_CHECK(d->CreateCommandAllocator(t, IID_PPV_ARGS(&allocator)))
    GX_DXR_CHECK(d->CreateCommandList(0, t, allocator.Get(), nullptr, IID_PPV_ARGS(&list)))
    GX_DXR_CHECK(list->Close())
}

gearoenix::dxr::Command::~Command() noexcept = default;

ID3D12GraphicsCommandList6* gearoenix::dxr::Command::get_list() noexcept
{
    return list.Get();
}

void gearoenix::dxr::Command::begin(ID3D12PipelineState* const pipeline_state) noexcept
{
    if (is_recording)
        GX_LOG_F("Command list is in recording state or it isn't closed yet.")
    is_recording = true;
    GX_DXR_CHECK(allocator->Reset())
    GX_DXR_CHECK(list->Reset(allocator.Get(), pipeline_state))
}

void gearoenix::dxr::Command::close() noexcept
{
    if (!is_recording)
        GX_LOG_F("Command list is not in recording state or it didn't start recording with begin() function.")
    GX_DXR_CHECK(list->Close())
    is_recording = false;
}

void gearoenix::dxr::Command::force_close() noexcept
{
    if (is_recording) {
        GX_DXR_CHECK(list->Close())
        is_recording = false;
    }
}

#endif
