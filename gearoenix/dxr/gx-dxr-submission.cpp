#include "gx-dxr-submission.hpp"

#ifdef GX_RENDER_DXR_ENABLED
#include "../core/ecs/gx-cr-ecs-world.hpp"
#include "../core/macro/gx-cr-mcr-counter.hpp"
#include "../physics/collider/gx-phs-cld-aabb.hpp"
#include "../physics/collider/gx-phs-cld-frustum.hpp"
#include "../physics/gx-phs-transformation.hpp"
#include "../render/camera/gx-rnd-cmr-camera.hpp"
#include "../render/material/gx-rnd-mat-pbr.hpp"
#include "../render/model/gx-rnd-mdl-model.hpp"
#include "../render/scene/gx-rnd-scn-scene.hpp"
#include "gx-dxr-build-configuration.hpp"
#include "gx-dxr-check.hpp"
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
#include <algorithm>

gearoenix::dxr::SubmissionManager::CameraData::Frame::Frame(Device& device) noexcept
{
    const std::size_t threads_count = std::thread::hardware_concurrency();
    threads_g_buffer_filler_commands.reserve(threads_count);
    threads_g_buffer_filler_command_lists_raw.reserve(threads_count);
    for (std::size_t thread_index = 0; thread_index < threads_count; ++thread_index) {
        threads_g_buffer_filler_commands.emplace_back(device, D3D12_COMMAND_LIST_TYPE_DIRECT);
        threads_g_buffer_filler_command_lists_raw.push_back(threads_g_buffer_filler_commands[thread_index].get_list());
    }
}

gearoenix::dxr::SubmissionManager::CameraData::CameraData(Device& device) noexcept
    : frames {
        Frame(device),
        Frame(device),
        Frame(device),
    }
{
}

bool gearoenix::dxr::SubmissionManager::fill_g_buffer(const std::size_t camera_pool_index) noexcept
{
    auto& camera = camera_pool[camera_pool_index];
    auto& frame = camera.frames[e.get_frame_number()];
    std::array<ID3D12DescriptorHeap*, 2> heaps {
        descriptor_manager->get_allocator().heap.Get(),
        descriptor_manager->get_sampler_allocator().heap.Get()
    };
    bool first_command = true;
    for (auto& command : frame.threads_g_buffer_filler_commands) {
        command.begin(pipeline_manager->get_g_buffer_filler_pipeline_state().Get());
        auto* const cmd = command.get_list();
        cmd->SetGraphicsRootSignature(pipeline_manager->get_g_buffer_filler_root_signature().Get());
        cmd->SetDescriptorHeaps(static_cast<UINT>(heaps.size()), heaps.data());
        cmd->SetGraphicsRootDescriptorTable(1, descriptor_manager->get_texture_2d_region_gpu_handle());
        cmd->SetGraphicsRootDescriptorTable(2, descriptor_manager->get_samplers_region_gpu_handle());
        cmd->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        if (first_command) // I don't like that 'if' and 'bool' but if there was any other neat idea I'll welcome it.
            swapchain->transit_to_target(cmd);
        swapchain->prepare(cmd); // TODO scissor and viewport settings must come from camera settings.
        if (first_command)
            swapchain->clear(cmd); // TODO: if our rendering was rasterised forward rendering, here it had only have to clear the depth buffer, but I cant imagine right now how is it going to be for the ray tracing
        first_command = false;
    }

    core::sync::ParallelFor::seq_ranges_exec(camera.opaque_models_data.begin(), camera.opaque_models_data.end(), [&](std::pair<double, ModelBvhData>& distance_model_data, const unsigned int thread_index) {
        auto& model_data = distance_model_data.second;
        auto* const cmd = frame.threads_g_buffer_filler_commands[thread_index].get_list();
        cmd->SetGraphicsRootConstantBufferView(0, model_data.current_frame_uniform_address);
        cmd->IASetVertexBuffers(0, 1, model_data.vertex_view);
        cmd->IASetIndexBuffer(model_data.index_view);
        cmd->DrawIndexedInstanced(model_data.indices_count, 1, 0, 0, 0);
    });

    swapchain->transit_to_present(frame.threads_g_buffer_filler_commands.back().get_list());
    for (auto& command : frame.threads_g_buffer_filler_commands)
        command.close();
    queue->exe(
        frame.threads_g_buffer_filler_command_lists_raw.data(),
        static_cast<UINT>(frame.threads_g_buffer_filler_command_lists_raw.size()));

    return false;
}

gearoenix::dxr::SubmissionManager::SubmissionManager(Engine& e) noexcept
    : e(e)
    , device(e.get_device())
    , descriptor_manager(e.get_descriptor_manager())
    , pipeline_manager(e.get_pipeline_manager())
    , swapchain(e.get_swapchain())
    , queue(e.get_queue())
{
}

gearoenix::dxr::SubmissionManager::~SubmissionManager() noexcept = default;

bool gearoenix::dxr::SubmissionManager::render_frame() noexcept
{
    auto* const world = e.get_world();

    bvh_pool.clear();
    camera_pool.clear();
    scene_pool.clear();
    scenes.clear();

    world->synchronised_system<render::scene::Scene>([&](const core::ecs::Entity::id_t scene_id, render::scene::Scene& scene) {
        if (!scene.enabled)
            return;
        const auto scene_pool_index = scene_pool.emplace([] { return SceneData(); });
        auto& scene_pool_ref = scene_pool[scene_pool_index];
        scene_pool_ref.bvh_pool_index = bvh_pool.emplace([] { return physics::accelerator::Bvh<ModelBvhData>(); });
        scene_pool_ref.cameras.clear();
        world->synchronised_system<render::camera::Camera>([&](const core::ecs::Entity::id_t camera_id, render::camera::Camera& camera) {
            if (!camera.enabled)
                return;
            if (camera.get_scene_id() != scene_id)
                return;
            scene_pool_ref.cameras.emplace(
                std::make_pair(camera.get_layer(), camera_id),
                camera_pool.emplace([&] { return CameraData(*e.get_device()); }));
        });
        scenes.emplace(std::make_pair(scene.get_layer(), scene_id), scene_pool_index);
    });

    world->parallel_system<render::scene::Scene>([&](const core::ecs::Entity::id_t scene_id, render::scene::Scene& scene, const unsigned int) {
        if (!scene.enabled)
            return;
        auto& scene_data = scene_pool[scenes[std::make_pair(scene.get_layer(), scene_id)]];
        auto& bvh = bvh_pool[scene_data.bvh_pool_index];
        bvh.reset();
        world->synchronised_system<physics::collider::Aabb3, render::model::Model, Model>([&](
                                                                                              const core::ecs::Entity::id_t,
                                                                                              physics::collider::Aabb3& collider,
                                                                                              render::model::Model& render_model,
                                                                                              Model& model) {
            if (!render_model.enabled)
                return;
            if (render_model.scene_id != scene_id)
                return;
            auto& mesh = *model.bound_mesh;
            bvh.add({ collider.get_updated_box(), ModelBvhData {
                                                      .current_frame_uniform_address = model.uniforms[e.get_frame_number()].get_buffer().get_resource()->GetGPUVirtualAddress(),
                                                      .vertex_view = &mesh.vv,
                                                      .index_view = &mesh.iv,
                                                      .indices_count = mesh.indices_count,
                                                  } });
        });
        bvh.create_nodes();
        world->parallel_system<render::camera::Camera, physics::collider::Frustum, physics::Transformation>([&](
                                                                                                                const core::ecs::Entity::id_t camera_id,
                                                                                                                render::camera::Camera& camera,
                                                                                                                physics::collider::Frustum& frustum,
                                                                                                                physics::Transformation& transform,
                                                                                                                const unsigned int) {
            if (!camera.get_is_enabled())
                return;
            if (scene_id != camera.get_scene_id())
                return;
            const auto camera_location = transform.get_location();
            auto& camera_data = camera_pool[scene_data.cameras[std::make_pair(camera.get_layer(), camera_id)]];
            camera_data.opaque_models_data.clear();
            camera_data.tranclucent_models_data.clear();
            bvh.call_on_intersecting(frustum, [&](const std::remove_reference_t<decltype(bvh)>::Data& bvh_node_data) {
                // TODO flag check for camera block in ModelBvhData
                const auto dir = camera_location - bvh_node_data.box.get_center();
                const auto dis = dir.square_length() * (dir.dot(transform.get_z_axis()) > 0.0 ? 1.0 : -1.0) - bvh_node_data.box.get_diameter().square_length() * 0.5;
                camera_data.opaque_models_data.push_back({ dis, bvh_node_data.user_data });
                // TODO opaque/translucent in ModelBvhData
            });
            std::sort(camera_data.opaque_models_data.begin(), camera_data.opaque_models_data.end(), [](const auto& rhs, const auto& lhs) { return rhs.first < lhs.first; });
            std::sort(camera_data.tranclucent_models_data.begin(), camera_data.tranclucent_models_data.end(), [](const auto& rhs, const auto& lhs) { return rhs.first > lhs.first; });
        });
    });

    for (auto& scene_layer_entity_id_pool_index : scenes) {
        auto& scene = scene_pool[scene_layer_entity_id_pool_index.second];
        for (auto& camera_layer_entity_id_pool_index : scene.cameras) {
            if (fill_g_buffer(camera_layer_entity_id_pool_index.second))
                return true;
        }
    }

    return swapchain->present();
}

#endif