#ifndef GEAROENIX_GL_SUBMISSION_HPP
#define GEAROENIX_GL_SUBMISSION_HPP
#include "../render/gx-rnd-build-configuration.hpp"
#ifdef GX_RENDER_OPENGL_ENABLED
#include "../core/ecs/gx-cr-ecs-entity.hpp"
#include "../core/gx-cr-pool.hpp"
#include "../math/gx-math-matrix-4d.hpp"
#include "../physics/accelerator/gx-phs-acc-bvh.hpp"
#include "gx-gl-types.hpp"
#include <vector>

namespace gearoenix::gl {
struct Engine;
struct GBuffersFillerShader;
struct SubmissionManager final {
    Engine& e;
    const std::unique_ptr<GBuffersFillerShader> gbuffers_filler;

    struct ModelData final {
        math::Mat4x4<float> m;
        math::Mat4x4<float> inv_m;
        math::Vec4<float> albedo_factor;
        math::Vec4<float> normal_metallic_factor;
        math::Vec4<float> emission_roughness_factor;
        math::Vec4<float> alpha_cutoff_occlusion_strength_radiance_lod_coefficient_reserved;
        uint vertex_object = 0;
        uint vertex_buffer = 0;
        uint index_buffer = 0;
        sizei indices_count = 0;
        uint albedo_txt = 0;
        uint normal_txt = 0;
        uint emission_txt = 0;
        uint metallic_roughness_txt = 0;
        uint occlusion_txt = 0;
    };

    struct ModelBvhData final {
        std::uint64_t blocked_cameras_flags = static_cast<std::uint64_t>(-1);
        ModelData model;
    };

    struct CameraData final {
        math::Mat4x4<float> vp;
        math::Vec3<float> pos;
        std::vector<std::pair<double, ModelData>> opaque_models_data;
        std::vector<std::pair<double, ModelData>> tranclucent_models_data;
        std::vector<std::vector<std::pair<double, ModelData>>> threads_opaque_models_data;
        std::vector<std::vector<std::pair<double, ModelData>>> threads_tranclucent_models_data;

        CameraData() noexcept;
    };

    struct SceneData final {
        boost::container::flat_map<std::pair<double /*layer*/, core::ecs::Entity::id_t /*camera-entity-id*/>, std::size_t /*camera-pool-index*/> cameras;
    };

private:
    boost::container::flat_map<core::ecs::Entity::id_t, physics::accelerator::Bvh<ModelBvhData>> scenes_bvhs;
    core::Pool<CameraData> camera_pool;
    core::Pool<SceneData> scene_pool;

    boost::container::flat_map<std::pair<double /*layer*/, core::ecs::Entity::id_t /*scene-entity-id*/>, std::size_t /*scene-pool-index*/> scenes;

    [[nodiscard]] bool fill_g_buffers(const std::size_t camera_pool_index) noexcept;

public:
    SubmissionManager(Engine& e) noexcept;
    ~SubmissionManager() noexcept;
    void update() noexcept;
};
}

#endif
#endif
