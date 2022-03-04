#include "gx-mtl-submission.hpp"
#ifdef GX_RENDER_METAL_ENABLED
#import "../core/ecs/gx-cr-ecs-world.hpp"
#import "../physics/collider/gx-phs-cld-aabb.hpp"
#import "../physics/collider/gx-phs-cld-frustum.hpp"
#import "../physics/gx-phs-transformation.hpp"
#import "../platform/gx-plt-application.hpp"
#import "../render/camera/gx-rnd-cmr-camera.hpp"
#import "../render/model/gx-rnd-mdl-model.hpp"
#import "../render/scene/gx-rnd-scn-scene.hpp"
#import "gx-mtl-camera.hpp"
#import "gx-mtl-engine.hpp"
#import "gx-mtl-heap.hpp"
#import "gx-mtl-math-helper.hpp"
#import "gx-mtl-mesh.hpp"
#import "gx-mtl-model.hpp"
#import "gx-mtl-pipeline.hpp"
#import <thread>

gearoenix::metal::SubmissionManager::SubmissionManager(Engine& e) noexcept
    : e(e)
    , queue([e.get_device() newCommandQueue])
{}

gearoenix::metal::SubmissionManager::~SubmissionManager() noexcept
{
    [queue release];
}

void gearoenix::metal::SubmissionManager::update() noexcept
{
    auto* const world = e.get_world();
    const auto frame_number = e.get_frame_number();
    id <MTLCommandBuffer> cmd = [queue commandBuffer];
    cmd.label = @"Gearoenix-Command-Submission";

    auto view = e.get_platform_application().get_app_delegate().view_controller.metal_kit_view;
    MTLRenderPassDescriptor *render_pass_descriptor = view.currentRenderPassDescriptor; // TODO: it maybe needed to have different render-pass-desc for each camera
    
    GX_ASSERT(nullptr != render_pass_descriptor)
    
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
        world->synchronised_system<render::camera::Camera, Camera>([&](const core::ecs::Entity::id_t camera_id, render::camera::Camera& camera, Camera& metal_camera) {
            if (!camera.enabled)
                return;
            if (camera.get_scene_id() != scene_id)
                return;
            const auto camera_pool_index = camera_pool.emplace([&] { return CameraData(); });
            auto& camera_pool_ref = camera_pool[camera_pool_index];
            camera_pool_ref.cpu_buffer = metal_camera.uniform.cpu[frame_number];
            camera_pool_ref.gpu_buffer = metal_camera.uniform.gpu;
            scene_pool_ref.cameras.emplace(
                std::make_pair(camera.get_layer(), camera_id),
                camera_pool_index);
        });
        scenes.emplace(std::make_pair(scene.get_layer(), scene_id), scene_pool_index);
    });

    world->parallel_system<render::scene::Scene>([&](const core::ecs::Entity::id_t scene_id, render::scene::Scene& scene, const unsigned int) {
        if (!scene.enabled)
            return;
        auto& scene_data = scene_pool[scenes[std::make_pair(scene.get_layer(), scene_id)]];
        auto& bvh = bvh_pool[scene_data.bvh_pool_index];
        bvh.reset();
        world->synchronised_system<physics::collider::Aabb3, render::model::Model, Model, physics::Transformation>([&](const core::ecs::Entity::id_t, physics::collider::Aabb3& collider, render::model::Model& render_model, Model& model, physics::Transformation& model_transform) {
            if (!render_model.enabled)
                return;
            if (render_model.scene_id != scene_id)
                return;
            auto& mesh = *model.bound_mesh;
            id<MTLBuffer> uniform_buffer = model.uniform.cpu[frame_number];
            auto* const uniform_ptr = reinterpret_cast<ModelUniform*>(uniform_buffer.contents);
            uniform_ptr->model = simd_make_float4x4_t(model_transform.get_matrix());
            uniform_ptr->transposed_reversed_model = simd_make_float4x4_t(model_transform.get_inverted_matrix().transposed());
            bvh.add({
                collider.get_updated_box(),
                ModelBvhData {
                    .blocked_cameras_flags = render_model.block_cameras_flags,
                    .model = ModelData {
                        .args = model.gbuffers_filler_args.buffer,
                        .vertex = mesh.vertex_buffer,
                        .index = mesh.index_buffer,
                        .indices_count = mesh.indices_count,
                    }
                }
            });
        });
        bvh.create_nodes();
        world->parallel_system<render::camera::Camera, physics::collider::Frustum, physics::Transformation, Camera>([&](const core::ecs::Entity::id_t camera_id, render::camera::Camera& render_camera, physics::collider::Frustum& frustum, physics::Transformation& transform, Camera& metal_camera, const unsigned int) {
            if (!render_camera.get_is_enabled())
                return;
            if (scene_id != render_camera.get_scene_id())
                return;
            const auto camera_location = transform.get_location();
            auto& camera_data = camera_pool[scene_data.cameras[std::make_pair(render_camera.get_layer(), camera_id)]];
            auto* const uniform_ptr = reinterpret_cast<CameraUniform*>(camera_data.cpu_buffer.contents);
            uniform_ptr->view_projection = simd_make_float4x4_t(render_camera.get_view_projection());
            camera_data.opaque_models_data.clear();
            camera_data.tranclucent_models_data.clear();
            bvh.call_on_intersecting(frustum, [&](const std::remove_reference_t<decltype(bvh)>::Data& bvh_node_data) {
                if ((bvh_node_data.user_data.blocked_cameras_flags & render_camera.get_flag()) == 0)
                    return;
                const auto dir = camera_location - bvh_node_data.box.get_center();
                const auto dis = dir.square_length() * (dir.dot(transform.get_z_axis()) > 0.0 ? 1.0 : -1.0) - bvh_node_data.box.get_diameter().square_length() * 0.5;
                camera_data.opaque_models_data.push_back({ dis, bvh_node_data.user_data.model });
                // TODO opaque/translucent in ModelBvhData
            });
            std::sort(camera_data.opaque_models_data.begin(), camera_data.opaque_models_data.end(), [](const auto& rhs, const auto& lhs) { return rhs.first < lhs.first; });
            std::sort(camera_data.tranclucent_models_data.begin(), camera_data.tranclucent_models_data.end(), [](const auto& rhs, const auto& lhs) { return rhs.first > lhs.first; });
        });
    });

    const auto blit = [cmd blitCommandEncoder];
    blit.label = @"Copy of uniform buffers";
    [blit pushDebugGroup:@"Copy pass"];
    
    world->synchronised_system<Model>([&](const core::ecs::Entity::id_t scene_id, Model& m) {
        m.uniform.update(blit, frame_number);
    });
    
    world->synchronised_system<Camera>([&](const core::ecs::Entity::id_t scene_id, Camera& c) {
        c.uniform.update(blit, frame_number);
    });
    
    [blit popDebugGroup];
    [blit endEncoding];
    
    // std::vector<std::pair<id<MTLParallelRenderCommandEncoder>, id<MTLRenderCommandEncoder>>> encoders(std::thread::hardware_concurrency());
    std::vector<std::pair<id<MTLParallelRenderCommandEncoder>, id<MTLRenderCommandEncoder>>> encoders(1);
    
    for (auto& scene_layer_entity_id_pool_index : scenes) {
        auto& scene = scene_pool[scene_layer_entity_id_pool_index.second];
        for (auto& camera_layer_entity_id_pool_index : scene.cameras) {
            auto& camera = camera_pool[camera_layer_entity_id_pool_index.second];
            
            for(auto& [parallel_encoder, encoder]: encoders)
            {
                parallel_encoder = [cmd parallelRenderCommandEncoderWithDescriptor:render_pass_descriptor];
                encoder = [parallel_encoder renderCommandEncoder];
                [encoder useHeap:e.get_heap_manager()->gpu];
                [encoder setCullMode:MTLCullModeBack];
                [encoder setFrontFacingWinding:MTLWindingClockwise];
                [encoder setRenderPipelineState:e.get_pipeline_manager()->get_gbuffers_filler_ps()];
                [encoder setDepthStencilState:e.get_pipeline_manager()->get_depth_state()];
            }

            // core::sync::ParallelFor::seq_ranges_exec(camera.opaque_models_data.begin(), camera.opaque_models_data.end(), [&](std::pair<double, ModelData>& distance_model_data, const unsigned int thread_index) {
            for(auto& distance_model_data: camera.opaque_models_data) { const std::size_t thread_index = 0;
                auto& model_data = distance_model_data.second;
                const auto& enc = encoders[thread_index].second;
                [enc setVertexBuffer:model_data.vertex offset:0 atIndex:GEAROENIX_METAL_GBUFFERS_FILLER_VERTEX_BUFFER_BIND_INDEX];
                [enc setVertexBuffer:camera.gpu_buffer offset:0 atIndex:GEAROENIX_METAL_GBUFFERS_FILLER_CAMERA_UNIFORM_BIND_INDEX];
                [enc setVertexBuffer:model_data.args offset:0 atIndex:GEAROENIX_METAL_GBUFFERS_FILLER_ARGUMENT_BUFFER_BIND_INDEX];
                [enc setFragmentBuffer:camera.gpu_buffer offset:0 atIndex:GEAROENIX_METAL_GBUFFERS_FILLER_CAMERA_UNIFORM_BIND_INDEX];
                [enc setFragmentBuffer:model_data.args offset:0 atIndex:GEAROENIX_METAL_GBUFFERS_FILLER_ARGUMENT_BUFFER_BIND_INDEX];
                [enc drawIndexedPrimitives:MTLPrimitiveTypeTriangle indexCount:model_data.indices_count indexType:MTLIndexTypeUInt32 indexBuffer:model_data.index indexBufferOffset:0];
            //});
            }

            for(auto& [parallel_encoder, encoder]: encoders)
            {
                [encoder popDebugGroup];
                [encoder endEncoding];
                [parallel_encoder endEncoding];
            }
        }
    }
    
    [cmd presentDrawable:view.currentDrawable];
    [cmd commit];
    [cmd waitUntilCompleted];
}

#endif
