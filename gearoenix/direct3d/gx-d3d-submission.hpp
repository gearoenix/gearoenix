#pragma once

#include "../render/gx-rnd-build-configuration.hpp"
#ifdef GX_RENDER_DIRECT3D_ENABLED
#include "../core/ecs/gx-cr-ecs-entity.hpp"
#include "../core/gx-cr-pool.hpp"
#include "../physics/accelerator/gx-phs-acc-bvh.hpp"
#include "gx-d3d-build-configuration.hpp"
#include "gx-d3d-command.hpp"
#include <boost/container/flat_map.hpp>
#include <vector>

namespace gearoenix::d3d {
struct DescriptorManager;
struct Device;
struct Engine;
struct PipelineManager;
struct Queue;
struct Swapchain;
struct ModelUniform;

struct SubmissionManager final {
    struct ModelBvhData final {
        std::uint64_t blocked_cameras_flags = static_cast<std::uint64_t>(-1);
        ModelUniform* uniform_ptr = nullptr;
        D3D12_GPU_VIRTUAL_ADDRESS current_frame_uniform_address;
        const D3D12_VERTEX_BUFFER_VIEW* vertex_view = nullptr;
        const D3D12_INDEX_BUFFER_VIEW* index_view = nullptr;
        UINT indices_count = 0;
    };

    struct CameraData final {
        struct Frame final {
            std::vector<Command> threads_g_buffer_filler_commands;
            std::vector<ID3D12CommandList*> threads_g_buffer_filler_command_lists_raw;

            Frame(Device&);
        };
        D3D12_GPU_VIRTUAL_ADDRESS current_frame_uniform_address;
        std::vector<std::pair<double, ModelBvhData>> opaque_models_data;
        std::vector<std::pair<double, ModelBvhData>> tranclucent_models_data;
        std::array<Frame, GX_D3D_FRAMES_BACKBUFFER_NUMBER> frames;

        CameraData(Device&);
    };

    struct SceneData final {
        std::size_t bvh_pool_index = 0;
        boost::container::flat_map<std::pair<double /*layer*/, core::ecs::entity_id_t /*camera-entity-id*/>, std::size_t /*camera-pool-index*/> cameras;
    };

    GX_GET_RRF_PRV(Engine, e)
    GX_GET_REFC_PRV(std::shared_ptr<Device>, device)
    GX_GET_REFC_PRV(std::shared_ptr<DescriptorManager>, descriptor_manager)
    GX_GET_REFC_PRV(std::shared_ptr<PipelineManager>, pipeline_manager)
    GX_GET_REFC_PRV(std::shared_ptr<Swapchain>, swapchain)
    GX_GET_REFC_PRV(std::shared_ptr<Queue>, queue)

private:
    core::Pool<physics::accelerator::Bvh<ModelBvhData>> bvh_pool;
    core::Pool<CameraData> camera_pool;
    core::Pool<SceneData> scene_pool;

    boost::container::flat_map<std::pair<double /*layer*/, core::ecs::entity_id_t /*scene-entity-id*/>, std::size_t /*scene-pool-index*/> scenes;

    [[nodiscard]] bool fill_g_buffer(const std::size_t camera_pool_index);

public:
    SubmissionManager(Engine& e);
    ~SubmissionManager();

    [[nodiscard]] bool render_frame();
};
}

#endif