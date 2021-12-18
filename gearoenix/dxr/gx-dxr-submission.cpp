#include "gx-dxr-submission.hpp"

#ifdef GX_RENDER_DXR_ENABLED
#include "../core/ecs/gx-cr-ecs-world.hpp"
#include "../core/macro/gx-cr-mcr-counter.hpp"
#include "../render/material/gx-rnd-mat-pbr.hpp"
#include "gx-dxr-check.hpp"
#include "gx-dxr-descriptor.hpp"
#include "gx-dxr-device.hpp"
#include "gx-dxr-engine.hpp"
#include "gx-dxr-mesh.hpp"
#include "gx-dxr-pipeline.hpp"
#include "gx-dxr-queue.hpp"
#include "gx-dxr-swapchain.hpp"
#include "gx-dxr-texture.hpp"
#include "shaders/gx-dxr-shd-common.hpp"

gearoenix::dxr::SubmissionManager::SubmissionManager(Engine& e) noexcept
    : e(e)
    , device(e.get_device())
    , descriptor_manager(e.get_descriptor_manager())
    , pipeline_manager(e.get_pipeline_manager())
    , swapchain(e.get_swapchain())
    , queue(e.get_queue())
    , frames_count(Swapchain::BACK_BUFFERS_COUNT)
    , frames_data(frames_count)
{
    auto* const d = device->get_device().Get();
    GX_DXR_CHECK(d->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&command_allocator)))
    const std::size_t threads_count = std::thread::hardware_concurrency();
    threads_command_allocators.resize(threads_count);
    for (auto& a : threads_command_allocators)
        GX_DXR_CHECK(d->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&a)))
    for (auto& f : frames_data) {
        f.threads_g_buffer_filler_command_lists.resize(threads_count);
        f.threads_g_buffer_filler_command_lists_raw.resize(threads_count);
        for (std::size_t thread_index = 0; thread_index < threads_count; ++thread_index) {
            GX_DXR_CHECK(d->CreateCommandList(
                0, D3D12_COMMAND_LIST_TYPE_DIRECT, threads_command_allocators[thread_index].Get(),
                nullptr, IID_PPV_ARGS(&f.threads_g_buffer_filler_command_lists[thread_index])))
            GX_DXR_CHECK(f.threads_g_buffer_filler_command_lists[thread_index]->Close())
            f.threads_g_buffer_filler_command_lists_raw[thread_index] = f.threads_g_buffer_filler_command_lists[thread_index].Get();
        }
    }
}

gearoenix::dxr::SubmissionManager::~SubmissionManager() noexcept = default;

bool gearoenix::dxr::SubmissionManager::render_frame() noexcept
{
    ++frame_number;
    const auto threads_count = threads_command_allocators.size();
    auto& f = frames_data[frame_number % frames_count];
    for (auto& a : threads_command_allocators)
        GX_DXR_CHECK(a->Reset())
    GX_DXR_CHECK(command_allocator->Reset())
    ID3D12DescriptorHeap* heaps[] = {
        descriptor_manager->get_allocator().heap.Get(),
        descriptor_manager->get_sampler_allocator().heap.Get()
    };
    for (std::size_t thread_index = 0; thread_index < threads_count; ++thread_index) {
        auto* const cmd = f.threads_g_buffer_filler_command_lists[thread_index].Get();
        GX_DXR_CHECK(cmd->Reset(
            threads_command_allocators[thread_index].Get(),
            pipeline_manager->get_g_buffer_filler_pipeline_state().Get()))
        cmd->SetGraphicsRootSignature(pipeline_manager->get_g_buffer_filler_root_signature().Get());
        cmd->SetDescriptorHeaps(GX_COUNT_OF(heaps), heaps);
        cmd->SetGraphicsRootDescriptorTable(1, descriptor_manager->get_texture_2d_region_gpu_handle());
        cmd->SetGraphicsRootDescriptorTable(2, descriptor_manager->get_samplers_region_gpu_handle());
        cmd->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        if (0 == thread_index)
            swapchain->transit_to_target(cmd);
        swapchain->prepare(cmd);
        if (0 == thread_index)
            swapchain->clear(cmd);
    }
    // TODO better scene management
    e.get_world()->parallel_system<Mesh, MeshBuffer>([&](const core::ecs::Entity::id_t, Mesh& m, MeshBuffer& mb, const unsigned int thread_index) noexcept {
        auto* const cmd = f.threads_g_buffer_filler_command_lists[thread_index].Get();
        cmd->SetGraphicsRootConstantBufferView(0, mb.uniform.get_buffer().get_resource()->GetGPUVirtualAddress());
        cmd->IASetVertexBuffers(0, 1, &m.get_vv());
        cmd->IASetIndexBuffer(&m.get_iv());
        cmd->DrawIndexedInstanced(m.get_indices_count(), 1, 0, 0, 0);
    });
    swapchain->transit_to_present(f.threads_g_buffer_filler_command_lists[threads_count - 1].Get());
    for (auto& tcmd : f.threads_g_buffer_filler_command_lists)
        GX_DXR_CHECK(tcmd->Close())
    queue->exe(f.threads_g_buffer_filler_command_lists_raw.data(), static_cast<UINT>(threads_count));
    return swapchain->present();
}

#endif