#ifndef GEAROENIX_GL_SUBMISSION_HPP
#define GEAROENIX_GL_SUBMISSION_HPP
#include "../render/gx-rnd-build-configuration.hpp"
#ifdef GX_RENDER_OPENGL_ENABLED
#include "../core/ecs/gx-cr-ecs-entity.hpp"
#include "../core/gx-cr-pool.hpp"
#include "../math/gx-math-matrix-4d.hpp"
#include "../physics/accelerator/gx-phs-acc-bvh.hpp"
#include "gx-gl-types.hpp"
#include <limits>
#include <memory>
#include <optional>
#include <vector>

namespace gearoenix::render::scene {
struct Scene;
}

namespace gearoenix::gl {
struct Engine;
struct ShaderFinal;
struct ShaderForwardPbr;
struct ShaderGBuffersFiller;
struct ShaderDeferredPbr;
struct ShaderDeferredPbrTransparent;
struct ShaderIrradiance;
struct ShaderRadiance;
struct ShaderSkyboxCube;
struct ShaderSkyboxEquirectangular;
struct ShaderSsaoResolve;
struct Target;
struct Texture2D;
struct TextureCube;
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
        // Reflection probe data
        uint irradiance = static_cast<uint>(-1);
        uint radiance = static_cast<uint>(-1);
        double reflection_probe_size = std::numeric_limits<double>::max();
    };

    struct ModelBvhData final {
        std::uint64_t blocked_cameras_flags = static_cast<std::uint64_t>(-1);
        ModelData model;
    };

    struct DynamicModelData final {
        ModelBvhData base;
        math::Aabb3<double> box;
    };

    struct CameraData final {
        uint framebuffer = static_cast<uint>(-1);
        math::Vec4<sizei> viewport_clip;
        math::Mat4x4<float> vp;
        math::Vec3<float> pos;
        core::ecs::Entity::id_t out_reference = 0;
        float skybox_scale = 1.0;
        std::vector<std::pair<double, ModelData>> opaque_models_data;
        std::vector<std::pair<double, ModelData>> tranclucent_models_data;
        std::vector<std::vector<std::pair<double, ModelData>>> threads_opaque_models_data;
        std::vector<std::vector<std::pair<double, ModelData>>> threads_tranclucent_models_data;

        CameraData() noexcept;
    };

    struct SkyboxData final {
        uint vertex_object = 0;
        uint albedo_txt = 0;
    };

    struct ReflectionData final {
        uint irradiance = 0;
        uint radiance = 0;
        math::Aabb3<double> box;
        double size = -std::numeric_limits<double>::max();
        float radiance_mips_count = 0.0f;
    };

    struct SceneData final {
        math::Vec4<float> ssao_settings;
        boost::container::flat_map<std::tuple<double /*layer*/, core::ecs::Entity::id_t /*skybox-entity-id*/, bool /*equirectangular*/>, SkyboxData> skyboxes;
        boost::container::flat_map<std::pair<double /*layer*/, core::ecs::Entity::id_t /*camera-entity-id*/>, std::size_t /*camera-pool-index*/> cameras;
        boost::container::flat_map<core::ecs::Entity::id_t, std::size_t /*camera-pool-index*/> reflection_cameras;
        boost::container::flat_map<core::ecs::Entity::id_t, std::size_t /*camera-pool-index*/> shadow_cameras;
        boost::container::flat_map<core::ecs::Entity::id_t, ReflectionData> reflections;
        std::pair<core::ecs::Entity::id_t, ReflectionData> default_reflection;
        std::vector<DynamicModelData> dynamic_models;
    };

private:
    Engine& e;
    const std::unique_ptr<ShaderFinal> final_shader;
    const std::unique_ptr<ShaderForwardPbr> forward_pbr_shader;
    const std::unique_ptr<ShaderGBuffersFiller> gbuffers_filler_shader;
    const std::unique_ptr<ShaderDeferredPbr> deferred_pbr_shader;
    const std::unique_ptr<ShaderDeferredPbrTransparent> deferred_pbr_transparent_shader;
    const std::unique_ptr<ShaderIrradiance> irradiance_shader;
    const std::unique_ptr<ShaderRadiance> radiance_shader;
    const std::unique_ptr<ShaderSkyboxCube> skybox_cube_shader;
    const std::unique_ptr<ShaderSkyboxEquirectangular> skybox_equirectangular_shader;
    const std::unique_ptr<ShaderSsaoResolve> ssao_resolve_shader;
    uint gbuffer_width, gbuffer_height;
    float gbuffer_aspect_ratio = 1.2f;
    float gbuffer_uv_move_x = 0.001f;
    float gbuffer_uv_move_y = 0.001f;
    std::shared_ptr<Texture2D> gbuffers_albedo_metallic_texture;
    std::shared_ptr<Texture2D> gbuffers_position_depth_texture;
    std::shared_ptr<Texture2D> gbuffers_normal_ao_texture;
    std::shared_ptr<Texture2D> gbuffers_emission_roughness_texture;
    std::shared_ptr<Texture2D> gbuffers_irradiance_texture;
    std::shared_ptr<Texture2D> gbuffers_radiance_texture;
    std::shared_ptr<Texture2D> gbuffers_depth_texture;
    std::shared_ptr<Target> gbuffers_target;
    std::shared_ptr<Texture2D> ssao_resolve_texture;
    std::shared_ptr<Target> ssao_resolve_target;
    std::shared_ptr<Texture2D> final_texture;
    std::shared_ptr<Target> final_target;
    std::shared_ptr<Texture2D> brdflut;
    std::shared_ptr<TextureCube> black_cube;
    uint screen_vertex_object = 0;
    uint screen_vertex_buffer = 0;
    boost::container::flat_map<core::ecs::Entity::id_t, physics::accelerator::Bvh<ModelBvhData>> scenes_bvhs;
    core::Pool<CameraData> camera_pool;
    core::Pool<SceneData> scene_pool;
    boost::container::flat_map<std::pair<double /*layer*/, core::ecs::Entity::id_t /*scene-entity-id*/>, std::size_t /*scene-pool-index*/> scenes;
    math::Vec4<sizei> current_viewport_clip;
    math::Vec4<sizei> gbuffer_viewport_clip;
    uint current_bound_framebuffer = static_cast<uint>(-1);

    void initialise_gbuffers() noexcept;
    void initialise_ssao() noexcept;
    void initialise_final() noexcept;
    void initialise_screen_vertices() noexcept;

    void fill_scenes() noexcept;
    void update_scenes() noexcept;
    void update_scene(core::ecs::Entity::id_t scene_id, SceneData& scene_data, render::scene::Scene& render_scene) noexcept;
    void update_scene_bvh(core::ecs::Entity::id_t scene_id, SceneData& scene_data, render::scene::Scene& render_scene, physics::accelerator::Bvh<ModelBvhData>& bvh) noexcept;
    void update_scene_dynamic_models(core::ecs::Entity::id_t scene_id, SceneData& scene_data) noexcept;
    void update_scene_reflection_probes(SceneData& scene_data, render::scene::Scene& render_scene, physics::accelerator::Bvh<ModelBvhData>& bvh) noexcept;
    void update_scene_cameras(core::ecs::Entity::id_t scene_id, SceneData& scene_data, physics::accelerator::Bvh<ModelBvhData>& bvh) noexcept;
    void fill_gbuffers(const std::size_t camera_pool_index) noexcept;
    void render_shadows() noexcept;
    void render_reflection_probes() noexcept;
    void render_reflection_probes(const SceneData& scene) noexcept;
    void render_forward_camera(const SceneData& scene, const CameraData& camera) noexcept;
    void render_with_deferred() noexcept;
    void render_with_forward() noexcept;
    void render_imgui() noexcept;
    void set_viewport_clip(const math::Vec4<sizei>& viewport_clip) noexcept;
    void set_framebuffer(uint framebuffer_object) noexcept;

public:
    SubmissionManager(Engine& e) noexcept;
    ~SubmissionManager() noexcept;
    void update() noexcept;
};
}

#endif
#endif
