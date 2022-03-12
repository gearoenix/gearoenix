#ifndef GEAROENIX_METAL_SUBMISSION_HPP
#define GEAROENIX_METAL_SUBMISSION_HPP
#include "../render/gx-rnd-build-configuration.hpp"
#ifdef GX_RENDER_METAL_ENABLED
#import "../core/ecs/gx-cr-ecs-entity.hpp"
#import "../core/gx-cr-pool.hpp"
#import "../physics/accelerator/gx-phs-acc-bvh.hpp"
#import <Metal/MTLBuffer.h>
#import <Metal/MTLCommandQueue.h>
#import <vector>

namespace gearoenix::metal {
struct Engine;
struct SubmissionManager final {
    Engine& e;
    const id<MTLCommandQueue> queue;
    
    struct ModelData final {
        // Cache render data here
        id<MTLBuffer> args = nil;
        id<MTLBuffer> vertex = nil;
        id<MTLBuffer> index = nil;
        NSUInteger indices_count = 0;
    };
    
    struct ModelBvhData final {
        std::uint64_t blocked_cameras_flags = static_cast<std::uint64_t>(-1);
        ModelData model;
    };

    struct CameraData final {
        std::uint8_t* uniform_ptr = nullptr;
        NSUInteger uniform_gpu_offset = 0;
        std::vector<std::pair<double, ModelData>> opaque_models_data;
        std::vector<std::pair<double, ModelData>> tranclucent_models_data;
    };

    struct SceneData final {
        std::size_t bvh_pool_index = 0;
        boost::container::flat_map<std::pair<double /*layer*/, core::ecs::Entity::id_t /*camera-entity-id*/>, std::size_t /*camera-pool-index*/> cameras;
    };

private:
    core::Pool<physics::accelerator::Bvh<ModelBvhData>> bvh_pool;
    core::Pool<CameraData> camera_pool;
    core::Pool<SceneData> scene_pool;

    boost::container::flat_map<std::pair<double /*layer*/, core::ecs::Entity::id_t /*scene-entity-id*/>, std::size_t /*scene-pool-index*/> scenes;
    
    dispatch_semaphore_t present_semaphore;

    [[nodiscard]] bool fill_g_buffers(const std::size_t camera_pool_index) noexcept;
    
public:
    SubmissionManager(Engine& e) noexcept;
    ~SubmissionManager() noexcept;
    void update() noexcept;
};
}

#endif
#endif

