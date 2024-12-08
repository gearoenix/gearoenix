#include "gx-gl-sbm-camera.hpp"
#ifdef GX_RENDER_OPENGL_ENABLED
#include "../../core/sync/gx-cr-sync-thread.hpp"
#include "../../physics/gx-phs-transformation.hpp"
#include "../../render/camera/gx-rnd-cmr-camera.hpp"
#include "../../render/engine/gx-rnd-eng-engine.hpp"
#include "../gx-gl-camera.hpp"
#include "../gx-gl-target.hpp"

std::optional<gearoenix::gl::submission::BloomData> gearoenix::gl::submission::BloomData::construct(
    const std::optional<render::camera::BloomData>& b)
{
    if (!b.has_value()) {
        return std::nullopt;
    }
    BloomData r;
    r.scatter_clamp_max_threshold_threshold_knee = b->get_scatter_clamp_max_threshold_threshold_knee();
    return r;
}

gearoenix::gl::submission::Camera::Camera()
    : threads_opaque_models_data(core::sync::threads_count())
    , threads_translucent_models_data(core::sync::threads_count())
    , threads_mvps(core::sync::threads_count())
    , debug_meshes_threads(core::sync::threads_count())
{
}

void gearoenix::gl::submission::Camera::clear(
    const gl::Camera& gl_cam,
    const math::Vec3<float>& location)
{
    models_data.clear();
    mvps.clear();
    debug_meshes.clear();
    name = nullptr;

    for (auto& v : threads_opaque_models_data) {
        v.clear();
    }
    for (auto& v : threads_translucent_models_data) {
        v.clear();
    }
    for (auto& v : threads_mvps) {
        v.clear();
    }
    for (auto& v : debug_meshes_threads) {
        v.clear();
    }

    const bool has_customised_target = gl_cam.get_target().is_customised();

    const math::Vec2<std::uint32_t> target_dimension = gl_cam.get_target().get_dimension();

    depth_attachment = static_cast<uint>(-1);
    colour_attachments[0] = static_cast<uint>(-1);
    colour_attachments[1] = static_cast<uint>(-1);

    if (has_customised_target) {
        const auto& target = *gl_cam.get_gl_target().get_customised().target;
        const auto& attachments = target.get_gl_attachments();
        main_framebuffer = target.get_framebuffer();
        colour_attachments[0] = attachments[0].texture_object;
        if (attachments.size() == 2) {
            // it means we have the main camera not shadow or reflection probe convoluter
            depth_attachment = attachments[1].texture_object;
        }
    } else {
        const auto& def = gl_cam.get_gl_target().get_default();
        {
            const auto& target = *def.main;
            const auto& attachments = target.get_gl_attachments();
            main_framebuffer = target.get_framebuffer();
            colour_attachments[0] = attachments[0].texture_object;
            depth_attachment = attachments[1].texture_object;
        }
        {
            colour_attachments[1] = def.targets[1][0]->get_gl_attachments()[0].texture_object;
        }
        for (std::uint32_t ti = 0; ti < framebuffers.size(); ++ti) {
            for (std::uint32_t mi = 0; mi < framebuffers[ti].size(); ++mi) {
                const auto& target = *def.targets[ti][mi];
                framebuffers[ti][mi] = target.get_framebuffer();
            }
        }
    }
    name = &gl_cam.get_name();
    viewport_clip = math::Vec4<sizei>(gl_cam.get_starting_clip_ending_clip() * math::Vec4<float>(target_dimension, target_dimension));
    vp = gl_cam.get_view_projection();
    pos = location;
    skybox_scale = gl_cam.get_far() / 1.732051f;
    exposure_value = gl_cam.get_exposure().get_enabled() ? gl_cam.get_exposure().get_value() : 1.0f;
    colour_tuning = gl_cam.get_colour_tuning();
    bloom_data = BloomData::construct(gl_cam.get_bloom_data());
    out_reference = gl_cam.get_parent_entity_id();
}

#endif