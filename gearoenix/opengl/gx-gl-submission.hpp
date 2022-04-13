#ifndef GEAROENIX_GL_SUBMISSION_HPP
#define GEAROENIX_GL_SUBMISSION_HPP
#include "../render/gx-rnd-build-configuration.hpp"
#ifdef GX_RENDER_OPENGL_ENABLED
#include "../core/ecs/gx-cr-ecs-entity.hpp"
#include "../core/gx-cr-pool.hpp"
#include "../math/gx-math-matrix-4d.hpp"
#include "../physics/accelerator/gx-phs-acc-bvh.hpp"
#include "gx-gl-types.hpp"
#include <memory>
#include <vector>

namespace gearoenix::gl {
struct Engine;
struct FinalEffectsShader;
struct GBuffersFillerShader;
struct SsaoResolveShader;
struct Target;
struct Texture2D;
struct SubmissionManager final {
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
        math::Vec4<float> ssao_settings;
        boost::container::flat_map<std::pair<double /*layer*/, core::ecs::Entity::id_t /*camera-entity-id*/>, std::size_t /*camera-pool-index*/> cameras;
    };

private:
    Engine& e;
    const std::unique_ptr<GBuffersFillerShader> gbuffers_filler_shader;
    const std::unique_ptr<SsaoResolveShader> ssao_resolve_shader;
    const std::unique_ptr<FinalEffectsShader> final_effects_shader;
    uint gbuffer_width, gbuffer_height;
    float gbuffer_aspect_ratio = 1.2f;
    float gbuffer_uv_move_x = 0.001f;
    float gbuffer_uv_move_y = 0.001f;
    std::shared_ptr<Texture2D> position_depth_texture;
    std::shared_ptr<Texture2D> albedo_metallic_texture;
    std::shared_ptr<Texture2D> normal_roughness_texture;
    std::shared_ptr<Texture2D> emission_ambient_occlusion_texture;
    std::unique_ptr<Target> gbuffers_target;
    std::shared_ptr<Texture2D> ssao_resolve_texture;
    std::unique_ptr<Target> ssao_resolve_target;
    uint screen_vertex_object = 0;
    uint screen_vertex_buffer = 0;
    boost::container::flat_map<core::ecs::Entity::id_t, physics::accelerator::Bvh<ModelBvhData>> scenes_bvhs;
    core::Pool<CameraData> camera_pool;
    core::Pool<SceneData> scene_pool;

    boost::container::flat_map<std::pair<double /*layer*/, core::ecs::Entity::id_t /*scene-entity-id*/>, std::size_t /*scene-pool-index*/> scenes;

    void initialise_gbuffers() noexcept;
    void initialise_ssao() noexcept;
    [[nodiscard]] bool fill_gbuffers(const std::size_t camera_pool_index) noexcept;

public:
    SubmissionManager(Engine& e) noexcept;
    ~SubmissionManager() noexcept;
    void update() noexcept;
};
}

#endif
#endif
