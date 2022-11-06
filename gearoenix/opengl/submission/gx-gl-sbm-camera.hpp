#ifndef GEAROENIX_GL_SUBMISSION_CAMERA_HPP
#define GEAROENIX_GL_SUBMISSION_CAMERA_HPP
#include "../../render/gx-rnd-build-configuration.hpp"
#ifdef GX_RENDER_OPENGL_ENABLED
#include "../../core/ecs/gx-cr-ecs-types.hpp"
#include "gx-gl-sbm-model.hpp"

namespace gearoenix::gl::submission {
struct Camera final {
    uint framebuffer = static_cast<uint>(-1);
    math::Vec4<sizei> viewport_clip;
    math::Mat4x4<float> vp;
    math::Vec3<float> pos;
    core::ecs::entity_id_t out_reference = 0;
    float skybox_scale = 1.0;
    float hdr_tune_mapping = 1.0f;
    float gamma_correction = 2.2f;
    std::vector<std::pair<double, Model>> opaque_models_data;
    std::vector<std::pair<double, Model>> translucent_models_data;
    std::vector<std::vector<std::pair<double, Model>>> threads_opaque_models_data;
    std::vector<std::vector<std::pair<double, Model>>> threads_translucent_models_data;
    std::vector<math::Mat4x4<float>> mvps;
    std::vector<std::vector<std::pair<std::size_t, math::Mat4x4<float>>>> opaque_threads_mvps;
    std::vector<std::vector<std::pair<std::size_t, math::Mat4x4<float>>>> translucent_threads_mvps;
    std::vector<DebugModel> debug_meshes;
    std::vector<std::vector<DebugModel>> debug_meshes_threads;

    Camera() noexcept;
};
}

#endif
#endif