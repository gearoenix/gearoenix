#ifndef GEAROENIX_GL_SUBMISSION_CAMERA_HPP
#define GEAROENIX_GL_SUBMISSION_CAMERA_HPP
#include "../../render/gx-rnd-build-configuration.hpp"
#ifdef GX_RENDER_OPENGL_ENABLED
#include "../../core/ecs/gx-cr-ecs-types.hpp"
#include "../../render/camera/gx-rnd-cmr-colour-tuning.hpp"
#include "gx-gl-sbm-model.hpp"

namespace gearoenix::physics {
struct Transformation;
}

namespace gearoenix::render::camera {
struct BloomData;
struct Camera;
}

namespace gearoenix::gl {
struct Camera;
}

namespace gearoenix::gl::submission {
struct BloomData final {
    math::Vec4<float> scatter_clamp_max_threshold_threshold_knee;
    uint prefilter_target = static_cast<uint>(-1);
    std::array<uint, GX_RENDER_MAX_BLOOM_DOWN_SAMPLE_COUNT> horizontal_targets { static_cast<uint>(-1) };
    std::array<uint, GX_RENDER_MAX_BLOOM_DOWN_SAMPLE_COUNT> vertical_targets { static_cast<uint>(-1) };
    std::array<uint, GX_RENDER_MAX_BLOOM_DOWN_SAMPLE_COUNT + 1> upsampler_targets { static_cast<uint>(-1) };

    [[nodiscard]] static std::optional<BloomData> construct(const std::optional<render::camera::BloomData>& rnd_bd);
};

struct Camera final {
    std::optional<BloomData> bloom_data = std::nullopt;
    uint framebuffer = static_cast<uint>(-1);
    uint colour_attachment = static_cast<uint>(-1);
    uint depth_attachment = static_cast<uint>(-1);
    uint second_framebuffer = static_cast<uint>(-1);
    uint second_colour_attachment = static_cast<uint>(-1);
    math::Vec4<sizei> viewport_clip;
    math::Mat4x4<float> vp;
    math::Vec3<float> pos;
    core::ecs::entity_id_t out_reference = 0;
    float skybox_scale = 1.0;
    float exposure_value = 1.0f;
    render::camera::ColourTuning colour_tuning;
    const std::string* name = nullptr;
    std::vector<std::pair<double, Model>> models_data;
    std::vector<std::vector<std::pair<double, Model>>> threads_opaque_models_data;
    std::vector<std::vector<std::pair<double, Model>>> threads_translucent_models_data;
    std::vector<math::Mat4x4<float>> mvps;
    std::vector<std::vector<std::pair<std::size_t, math::Mat4x4<float>>>> threads_mvps;
    std::vector<DebugModel> debug_meshes;
    std::vector<std::vector<DebugModel>> debug_meshes_threads;

    Camera();
    void clear(
        const gl::Camera& gl_cam,
        const render::camera::Camera& rnd_cam,
        const math::Vec3<float>& location);
};
}

#endif
#endif