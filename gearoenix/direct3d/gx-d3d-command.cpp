#include "gx-d3d-command.hpp"
#ifdef GX_RENDER_DIRECT3D_ENABLED
#include "gx-d3d-check.hpp"
#include "gx-d3d-device.hpp"

gearoenix::d3d::Command::Command(Device& device, const D3D12_COMMAND_LIST_TYPE t) noexcept
{
    auto* const d = device.get_device().Get();
    GX_D3D_CHECK(d->CreateCommandAllocator(t, IID_PPV_ARGS(&allocator)));
    GX_D3D_CHECK(d->CreateCommandList(0, t, allocator.Get(), nullptr, IID_PPV_ARGS(&list)));
    GX_D3D_CHECK(list->Close());
}

gearoenix::d3d::Command::~Command() noexcept = default;

ID3D12GraphicsCommandList6* gearoenix::d3d::Command::get_list() noexcept
{
    return list.Get();
}

void gearoenix::d3d::Command::begin(ID3D12PipelineState* const pipeline_state) noexcept
{
    if (is_recording)
        GX_LOG_F("Command list is in recording state or it isn't closed yet.");
    is_recording = true;
    GX_D3D_CHECK(allocator->Reset());
    GX_D3D_CHECK(list->Reset(allocator.Get(), pipeline_state));
}

void gearoenix::d3d::Command::close() noexcept
{
    if (!is_recording)
        GX_LOG_F("Command list is not in recording state or it didn't start recording with begin() function.");
    GX_D3D_CHECK(list->Close());
    is_recording = false;
}

void gearoenix::d3d::Command::force_close() noexcept
{
    if (is_recording) {
        GX_D3D_CHECK(list->Close());
        is_recording = false;
    }
}

#endif
