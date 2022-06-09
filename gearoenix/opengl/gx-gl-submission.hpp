#ifndef GEAROENIX_GL_SUBMISSION_HPP
#define GEAROENIX_GL_SUBMISSION_HPP
#include "../render/gx-rnd-build-configuration.hpp"
#ifdef GX_RENDER_OPENGL_ENABLED
#include "../core/ecs/gx-cr-ecs-entity.hpp"
#include "../core/gx-cr-pool.hpp"
#include "../math/gx-math-frustum.hpp"
#include "../math/gx-math-matrix-4d.hpp"
#include "../physics/accelerator/gx-phs-acc-bvh.hpp"
#include "gx-gl-types.hpp"
#include <array>
#include <limits>
#include <memory>
#include <optional>
#include <vector>

namespace gearoenix::render::scene {
struct Scene;
}

namespace gearoenix::gl::shader {
struct BGCAA;
struct DeferredPbr;
struct DeferredPbrTransparent;
struct Final;
struct ForwardPbr;
struct GBuffersFiller;
struct Irradiance;
struct Radiance;
struct ShadowCaster;
struct SkyboxCube;
struct SkyboxEquirectangular;
struct SsaoResolve;
}

namespace gearoenix::gl {
struct Engine;
struct Target;
struct Texture2D;
struct TextureCube;
struct SubmissionManager final {
    struct DirectionalShadowData final {
        math::Mat4x4<float> normalised_vp;
        math::Vec3<float> direction;
        math::Vec3<float> colour;
        uint shadow_map_texture = static_cast<uint>(-1);
    };

    struct ModelData final {
        math::Mat4x4<float> m;
        math::Mat4x4<float> inv_m;
        math::Vec4<float> albedo_factor;
        math::Vec4<float> normal_metallic_factor;
        math::Vec4<float> emission_roughness_factor;
        math::Vec4<float> alpha_cutoff_occlusion_strength_radiance_lod_coefficient_reserved;
        std::size_t shadow_caster_directional_lights_count = 0;
        std::array<math::Mat4x4<float>, GX_RENDER_MAX_DIRECTIONAL_LIGHTS_SHADOW_CASTER> shadow_caster_directional_lights_normalised_vp;
        std::array<math::Vec3<float>, GX_RENDER_MAX_DIRECTIONAL_LIGHTS_SHADOW_CASTER> shadow_caster_directional_lights_direction;
        std::array<math::Vec3<float>, GX_RENDER_MAX_DIRECTIONAL_LIGHTS_SHADOW_CASTER> shadow_caster_directional_lights_colour;
        std::array<uint, GX_RENDER_MAX_DIRECTIONAL_LIGHTS_SHADOW_CASTER> shadow_caster_directional_lights_shadow_map_texture;
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

    struct DirectionalShadowCasterData final {
        math::Frustum<double> frustum;
        DirectionalShadowData shadow_data;
    };

    struct SceneData final {
        math::Vec4<float> ssao_settings;
        boost::container::flat_map<std::tuple<double /*layer*/, core::ecs::Entity::id_t /*skybox-entity-id*/, bool /*equirectangular*/>, SkyboxData> skyboxes;
        boost::container::flat_map<std::pair<double /*layer*/, core::ecs::Entity::id_t /*camera-entity-id*/>, std::size_t /*camera-pool-index*/> cameras;
        boost::container::flat_map<core::ecs::Entity::id_t /*camera-id*/, std::size_t /*camera-pool-index*/> reflection_cameras;
        boost::container::flat_map<core::ecs::Entity::id_t /*camera-id*/, std::size_t /*camera-pool-index*/> shadow_cameras;
        boost::container::flat_map<core::ecs::Entity::id_t /*reflection-id*/, ReflectionData> reflections;
        boost::container::flat_map<core::ecs::Entity::id_t /*light-id*/, DirectionalShadowCasterData> shadow_caster_directional_lights;
        std::pair<core::ecs::Entity::id_t /*reflection-id*/, ReflectionData> default_reflection;
        std::vector<DynamicModelData> dynamic_models;
    };

    typedef std::array<std::unique_ptr<shader::ForwardPbr>, GX_RENDER_MAX_DIRECTIONAL_LIGHTS_SHADOW_CASTER + 1> DirectionalLightShaderForwardPbr;
    typedef DirectionalLightShaderForwardPbr ShaderForwardPbrCombination;

private:
    Engine& e;
    const std::unique_ptr<shader::BGCAA> bgcaa_shader;
    const std::unique_ptr<shader::Final> final_shader;
    const std::unique_ptr<shader::GBuffersFiller> gbuffers_filler_shader;
    const std::unique_ptr<shader::DeferredPbr> deferred_pbr_shader;
    const std::unique_ptr<shader::DeferredPbrTransparent> deferred_pbr_transparent_shader;
    const std::unique_ptr<shader::Irradiance> irradiance_shader;
    const std::unique_ptr<shader::ShadowCaster> shadow_caster_shader;
    const std::unique_ptr<shader::Radiance> radiance_shader;
    const std::unique_ptr<shader::SkyboxCube> skybox_cube_shader;
    const std::unique_ptr<shader::SkyboxEquirectangular> skybox_equirectangular_shader;
    const std::unique_ptr<shader::SsaoResolve> ssao_resolve_shader;
    ShaderForwardPbrCombination forward_pbr_shader_combination;

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

    // BGCAAS: Bloom Gamma-correction Colour-tuning Anti-Alaising Source
    std::shared_ptr<Texture2D> high_bgcaas_texture;
    std::shared_ptr<Texture2D> low_bgcaas_texture;
    std::shared_ptr<Texture2D> depth_bgcaas_texture;
    std::shared_ptr<Target> bgcaas_target;

    math::Vec2<uint> backbuffer_size { static_cast<uint>(-1) };
    float backbuffer_aspect_ratio = 999.0f;
    math::Vec2<float> backbuffer_uv_move { 0.001f };
    math::Vec4<sizei> backbuffer_viewport_clip { static_cast<sizei>(-1) };

    uint screen_vertex_object = 0;
    uint screen_vertex_buffer = 0;
    boost::container::flat_map<core::ecs::Entity::id_t, physics::accelerator::Bvh<ModelBvhData>> scenes_bvhs;
    core::Pool<CameraData> camera_pool;
    core::Pool<SceneData> scene_pool;
    boost::container::flat_map<std::pair<double /*layer*/, core::ecs::Entity::id_t /*scene-entity-id*/>, std::size_t /*scene-pool-index*/> scenes;
    math::Vec4<sizei> current_viewport_clip;
    uint current_bound_framebuffer = static_cast<uint>(-1);

    void initialise_backbuffer_sizes() noexcept;
    void initialise_gbuffers() noexcept;
    void initialise_ssao() noexcept;
    void initialise_final() noexcept;
    void initialise_screen_vertices() noexcept;
    void initialise_bgcaas() noexcept;

    void fill_scenes() noexcept;
    void update_scenes() noexcept;
    void update_scene(core::ecs::Entity::id_t scene_id, SceneData& scene_data, render::scene::Scene& render_scene) noexcept;
    void update_scene_bvh(core::ecs::Entity::id_t scene_id, SceneData& scene_data, render::scene::Scene& render_scene, physics::accelerator::Bvh<ModelBvhData>& bvh) noexcept;
    void update_scene_dynamic_models(core::ecs::Entity::id_t scene_id, SceneData& scene_data) noexcept;
    void update_scene_reflection_probes(SceneData& scene_data, render::scene::Scene& render_scene, physics::accelerator::Bvh<ModelBvhData>& bvh) noexcept;
    void update_scene_lights(SceneData& scene_data, physics::accelerator::Bvh<ModelBvhData>& bvh) noexcept;
    void update_scene_cameras(core::ecs::Entity::id_t scene_id, SceneData& scene_data, physics::accelerator::Bvh<ModelBvhData>& bvh) noexcept;
    void render_shadows() noexcept;
    void render_shadows(const CameraData& camera) noexcept;
    void render_shadows(const SceneData& scene) noexcept;
    void render_reflection_probes() noexcept;
    void render_reflection_probes(const SceneData& scene) noexcept;
    void render_skyboxes(const SceneData& scene, const CameraData& camera) noexcept;
    void render_forward_camera(const SceneData& scene, const CameraData& camera) noexcept;
    void render_bgcaa(const SceneData& scene) noexcept;
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
