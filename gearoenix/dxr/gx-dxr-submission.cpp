#include "gx-dxr-submission.hpp"

#ifdef GX_RENDER_DXR_ENABLED
#include "gx-dxr-check.hpp"
#include "gx-dxr-descriptor.hpp"
#include "gx-dxr-device.hpp"
#include "gx-dxr-engine.hpp"
#include "gx-dxr-pipeline.hpp"
#include "gx-dxr-swapchain.hpp"

gearoenix::dxr::SubmissionManager::SubmissionManager(Engine& e) noexcept
    : e(e)
    , device(e.get_device())
    , descriptor_manager(e.get_descriptor_manager())
    , pipeline_manager(e.get_pipeline_manager())
    , swapchain(e.get_swapchain())
    , frames_count(Swapchain::BACK_BUFFERS_COUNT)
    , frames_data(frames_count)
{
    auto* const d = device->get_device().Get();
    for (auto& f : frames_data) {
        GX_DXR_CHECK(d->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&f.cal)))
        GX_DXR_CHECK(d->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, f.cal.Get(), nullptr, IID_PPV_ARGS(&f.cmd)))
        GX_DXR_CHECK(f.cmd->Close())
    }
}

gearoenix::dxr::SubmissionManager::~SubmissionManager() noexcept = default;

bool gearoenix::dxr::SubmissionManager::render_frame() noexcept
{
    ++frame_number;
    auto& f = frames_data[frame_number % frames_count];
    GX_DXR_CHECK(f.cal->Reset())
    GX_DXR_CHECK(f.cmd->Reset(f.cal.Get(), pipeline_manager->get_g_buffer_filler_pipeline_state().Get()))
    f.cmd->SetGraphicsRootSignature(pipeline_manager->get_g_buffer_filler_root_signature().Get());
    // TODO
    swapchain->prepare(f.cmd.Get());
    // TODO
    return swapchain->present(f.cmd.Get());
}

#endif