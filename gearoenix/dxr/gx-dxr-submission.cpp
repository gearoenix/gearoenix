#include "gx-dxr-submission.hpp"

#ifdef GX_RENDER_DXR_ENABLED
#include "../core/ecs/gx-cr-ecs-world.hpp"
#include "../core/macro/gx-cr-mcr-counter.hpp"
#include "../render/material/gx-rnd-mat-pbr.hpp"
#include "gx-dxr-build-configuration.hpp"
#include "gx-dxr-check.hpp"
#include "gx-dxr-command.hpp"
#include "gx-dxr-descriptor.hpp"
#include "gx-dxr-device.hpp"
#include "gx-dxr-engine.hpp"
#include "gx-dxr-mesh.hpp"
#include "gx-dxr-model.hpp"
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
    , frames_count(GX_DXR_FRAMES_BACKBUFFER_NUMBER)
    , frames_data(frames_count)
{
    const std::size_t threads_count = std::thread::hardware_concurrency();
    for (auto& f : frames_data) {
        f.threads_g_buffer_filler_commands.resize(threads_count);
        f.threads_g_buffer_filler_command_lists_raw.resize(threads_count);
        for (std::size_t thread_index = 0; thread_index < threads_count; ++thread_index) {
            f.threads_g_buffer_filler_commands[thread_index] = std::make_unique<Command>(*device, D3D12_COMMAND_LIST_TYPE_DIRECT);
            f.threads_g_buffer_filler_command_lists_raw[thread_index] = f.threads_g_buffer_filler_commands[thread_index]->get_list();
        }
    }
}

gearoenix::dxr::SubmissionManager::~SubmissionManager() noexcept = default;

bool gearoenix::dxr::SubmissionManager::render_frame() noexcept
{
    ++frame_number;
    const auto frame_index = frame_number % frames_count;
    auto& f = frames_data[frame_index];
    ID3D12DescriptorHeap* heaps[] = {
        descriptor_manager->get_allocator().heap.Get(),
        descriptor_manager->get_sampler_allocator().heap.Get()
    };
    bool first_command = true;
    for (auto& command : f.threads_g_buffer_filler_commands) {
        command->begin(pipeline_manager->get_g_buffer_filler_pipeline_state().Get());
        auto* const cmd = command->get_list();
        cmd->SetGraphicsRootSignature(pipeline_manager->get_g_buffer_filler_root_signature().Get());
        cmd->SetDescriptorHeaps(GX_COUNT_OF(heaps), heaps);
        cmd->SetGraphicsRootDescriptorTable(1, descriptor_manager->get_texture_2d_region_gpu_handle());
        cmd->SetGraphicsRootDescriptorTable(2, descriptor_manager->get_samplers_region_gpu_handle());
        cmd->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        if (first_command)
            swapchain->transit_to_target(cmd);
        swapchain->prepare(cmd);
        if (first_command)
            swapchain->clear(cmd);
        first_command = false;
    }
    // TODO better scene management
    e.get_world()->parallel_system<Model>([&](const core::ecs::Entity::id_t, Model& model, const unsigned int thread_index) noexcept {
        auto* const cmd = f.threads_g_buffer_filler_commands[thread_index]->get_list();
        auto& m = *model.bound_mesh;
        cmd->SetGraphicsRootConstantBufferView(
            0, model.uniforms[frame_index].get_buffer().get_resource()->GetGPUVirtualAddress());
        cmd->IASetVertexBuffers(0, 1, &m.vv);
        cmd->IASetIndexBuffer(&m.iv);
        cmd->DrawIndexedInstanced(m.indices_count, 1, 0, 0, 0);
    });
    swapchain->transit_to_present(f.threads_g_buffer_filler_commands.back()->get_list());
    for (auto& command : f.threads_g_buffer_filler_commands)
        command->close();
    queue->exe(
        f.threads_g_buffer_filler_command_lists_raw.data(),
        static_cast<UINT>(f.threads_g_buffer_filler_command_lists_raw.size()));
    return swapchain->present();
}

void gearoenix::dxr::SubmissionManager::clear_command_lists() noexcept
{
    if (frames_data.empty())
        return;
    for (auto& f : frames_data)
        for (auto& c : f.threads_g_buffer_filler_commands)
            c->force_close();
}

#endif