#pragma once
#include "../../render/gx-rnd-build-configuration.hpp"
#ifdef GX_RENDER_OPENGL_ENABLED
#include "../../core/gx-cr-pool.hpp"
#include "../../physics/accelerator/gx-phs-acc-bvh.hpp"
#include "gx-gl-sbm-camera.hpp"
#include "gx-gl-sbm-scene.hpp"
#include <memory>
#include <optional>
#include <utility>
#include <vector>

namespace gearoenix::render::scene {
struct Scene;
}

namespace gearoenix::gl {
struct Engine;
struct Target;
struct Texture2D;
struct TextureCube;
}

namespace gearoenix::gl::shader {
struct BloomHorizontal;
struct BloomPrefilter;
struct BloomUpsampler;
struct BloomVertical;
struct ColourTuningAntiAliasingCombination;
struct DeferredPbr;
struct DeferredPbrTransparent;
struct Final;
struct Multiply;
struct Irradiance;
struct Radiance;
struct SkyboxCube;
struct SkyboxEquirectangular;
struct SsaoResolve;
struct UnlitCombination;
struct Unlit;
}

namespace gearoenix::gl::submission {
struct Manager final {
private:
    Engine& e;

    const std::unique_ptr<shader::Final> final_shader;
    const std::unique_ptr<shader::DeferredPbr> deferred_pbr_shader;
    const std::unique_ptr<shader::DeferredPbrTransparent> deferred_pbr_transparent_shader;
    const std::unique_ptr<shader::Irradiance> irradiance_shader;
    const std::unique_ptr<shader::Radiance> radiance_shader;
    const std::unique_ptr<shader::SkyboxCube> skybox_cube_shader;
    const std::unique_ptr<shader::SkyboxEquirectangular> skybox_equirectangular_shader;
    const std::unique_ptr<shader::SsaoResolve> ssao_resolve_shader;
    const std::unique_ptr<shader::Multiply> multiply_shader;
    const std::unique_ptr<shader::BloomPrefilter> bloom_prefilter_shader;
    const std::unique_ptr<shader::BloomHorizontal> bloom_horizontal_shader;
    const std::unique_ptr<shader::BloomVertical> bloom_vertical_shader;
    const std::unique_ptr<shader::BloomUpsampler> bloom_upsampler_shader;

    const std::shared_ptr<shader::ColourTuningAntiAliasingCombination> colour_tuning_anti_aliasing_shader_combination;
    const std::shared_ptr<shader::UnlitCombination> unlit_shader_combination;
    shader::Unlit& unlit_coloured_shader;

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
    std::shared_ptr<Texture2D> brdflut;
    std::shared_ptr<TextureCube> black_cube;

    math::Vec2<uint> back_buffer_size { static_cast<uint>(-1) };
    float back_buffer_aspect_ratio = 999.0f;
    math::Vec2<float> back_buffer_uv_move { math::Numeric::epsilon<float> };
    math::Vec4<sizei> back_buffer_viewport_clip { static_cast<sizei>(-1) };

    uint screen_vertex_object = 0;
    uint screen_vertex_buffer = 0;
    boost::container::flat_map<core::ecs::entity_id_t, physics::accelerator::Bvh<BvhNodeModel>> scenes_bvhs;
    core::Pool<Camera> camera_pool;
    core::Pool<Scene> scene_pool;
    boost::container::flat_map<std::pair<double /*layer*/, core::ecs::entity_id_t /*scene-entity-id*/>, std::uint32_t /*scene-pool-index*/> scenes;
    math::Vec4<sizei> current_viewport_clip;
    uint current_bound_framebuffer = static_cast<uint>(-1);
    uint current_shader = static_cast<uint>(-1);
    std::uint32_t resolution_cfg_listener_id = 0;

    void initialise_back_buffer_sizes();
    void back_buffer_size_changed();
    void initialise_gbuffers();
    void initialise_ssao();
    void initialise_screen_vertices();

    void fill_scenes();
    void update_scenes();
    void update_scene(core::ecs::entity_id_t scene_id, Scene& scene_data, render::scene::Scene& render_scene);
    void update_scene_bvh(core::ecs::entity_id_t scene_id, Scene& scene_data, render::scene::Scene& render_scene, physics::accelerator::Bvh<BvhNodeModel>& bvh);
    void update_scene_dynamic_models(core::ecs::entity_id_t scene_id, Scene& scene_data);
    void update_scene_reflection_probes(Scene& scene_data, render::scene::Scene& render_scene, physics::accelerator::Bvh<BvhNodeModel>& bvh);
    void update_scene_lights(Scene& scene_data, physics::accelerator::Bvh<BvhNodeModel>& bvh);
    void update_scene_cameras(core::ecs::entity_id_t scene_id, Scene& scene_data, physics::accelerator::Bvh<BvhNodeModel>& bvh);
    void render_shadows();
    void render_shadows(const Scene& scene, const Camera& camera);
    void render_shadows(const Scene& scene);
    void render_reflection_probes();
    void render_reflection_probes(const Scene& scene);
    void render_skyboxes(const Scene& scene, const Camera& camera);
    void render_forward_camera(const Scene& scene, const Camera& camera);
    void render_bloom(const Scene& scene, const Camera& camera);
    void render_colour_correction_anti_aliasing(const Scene& scene, const Camera& camera);
    void render_debug_meshes(const Scene& scene);
    void render_with_deferred();
    void render_with_forward();
    void render_imgui();
    void set_viewport_clip(const math::Vec4<sizei>& viewport_clip);
    void set_framebuffer(uint framebuffer_object);

public:
    explicit Manager(Engine& e);
    ~Manager();
    void update();
    void window_resized();
};
}

#endif