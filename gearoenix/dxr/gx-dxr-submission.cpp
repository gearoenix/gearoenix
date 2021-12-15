#include "gx-dxr-submission.hpp"

#ifdef GX_RENDER_DXR_ENABLED
#include "../core/ecs/gx-cr-ecs-world.hpp"
#include "../core/macro/gx-cr-mcr-counter.hpp"
#include "gx-dxr-check.hpp"
#include "gx-dxr-descriptor.hpp"
#include "gx-dxr-device.hpp"
#include "gx-dxr-engine.hpp"
#include "gx-dxr-mesh.hpp"
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
    ID3D12DescriptorHeap* heaps[] = { descriptor_manager->get_allocator().heap.Get() };
    f.cmd->SetDescriptorHeaps(GX_COUNT_OF(heaps), heaps);
    f.cmd->SetGraphicsRootDescriptorTable(1, descriptor_manager->get_texture_2d_region_gpu_handle());
    f.cmd->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    // TODO
    swapchain->prepare(f.cmd.Get());
    e.get_world()->parallel_system<Mesh, MaterialBuffer>([&](const core::ecs::Entity::id_t, Mesh& m, MaterialBuffer& mb, const unsigned int) noexcept {
        auto* colour = reinterpret_cast<math::Vec4<float>*>(mb.uniform.get_buffer().get_pointer());
        colour->x = 1.0f;
        colour->y = 1.0f;
        colour->z = 0.0f;
        colour->w = 1.0f;
        f.cmd->SetGraphicsRootConstantBufferView(0, mb.uniform.get_buffer().get_resource()->GetGPUVirtualAddress());
        f.cmd->IASetVertexBuffers(0, 1, &m.get_vv());
        f.cmd->IASetIndexBuffer(&m.get_iv());
        f.cmd->DrawIndexedInstanced(m.get_indices_count(), 1, 0, 0, 0);
    });
    return swapchain->present(f.cmd.Get());
}

#endif