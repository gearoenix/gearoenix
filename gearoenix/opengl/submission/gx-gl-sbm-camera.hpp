#pragma once
#include "../../render/gx-rnd-build-configuration.hpp"
#ifdef GX_RENDER_OPENGL_ENABLED
#include "../../core/ecs/gx-cr-ecs-types.hpp"
#include "../../render/camera/gx-rnd-cmr-colour-tuning.hpp"
#include "gx-gl-sbm-model.hpp"

namespace gearoenix::core::ecs {
struct Entity;
}

namespace gearoenix::physics {
struct Transformation;
}

namespace gearoenix::render::camera {
struct BloomData;
}

namespace gearoenix::gl {
struct Camera;
}

namespace gearoenix::gl::submission {

struct Camera final {
    std::array<uint, 2> colour_attachments {};
    uint depth_attachment = static_cast<uint>(-1);
    uint main_framebuffer = static_cast<uint>(-1);
    std::array<std::array<uint, GX_RENDER_DEFAULT_CAMERA_TARGET_MIPS_COUNT>, 2> framebuffers {};
    math::Vec4<sizei> viewport_clip;

    std::vector<std::pair<double, Model>> models_data;
    std::vector<std::vector<std::pair<double, Model>>> threads_opaque_models_data;
    std::vector<std::vector<std::pair<double, Model>>> threads_translucent_models_data;
    std::vector<math::Mat4x4<float>> mvps;
    std::vector<std::vector<std::pair<std::uint32_t, math::Mat4x4<float>>>> threads_mvps;
    std::vector<DebugModel> debug_meshes;
    std::vector<std::vector<DebugModel>> debug_meshes_threads;

    Camera();
    void clear(
        const gl::Camera& gl_cam,
        const math::Vec3<float>& location);
};
}

#endif