#include "gx-gl-sbm-camera.hpp"
#ifdef GX_RENDER_OPENGL_ENABLED
#include "../../physics/gx-phs-transformation.hpp"
#include "../../render/camera/gx-rnd-cmr-camera.hpp"
#include "../../render/texture/gx-rnd-txt-manager.hpp"
#include "../../render/engine/gx-rnd-eng-engine.hpp"
#include "../gx-gl-camera.hpp"
#include "../gx-gl-target.hpp"
#include "../gx-gl-texture.hpp"
#include <thread>

std::optional<gearoenix::gl::submission::BloomData> gearoenix::gl::submission::BloomData::construct(
        const std::optional<gl::BloomData>& gl_bd,
    const std::optional<render::camera::BloomData>& rnd_bd)
{
    if (!rnd_bd.has_value())
        return std::nullopt;
    BloomData r;
    r.scatter_clamp_max_threshold_threshold_knee = rnd_bd->get_scatter_clamp_max_threshold_threshold_knee();
    r.prefilter_target = gl_bd->get_prefilter_target()->get_framebuffer();
    for (std::size_t i = 0; i < r.horizontal_targets.size(); ++i) {
        r.horizontal_targets[i] = gl_bd->get_horizontal_targets()[i]->get_framebuffer();
    }
    for (std::size_t i = 0; i < r.vertical_targets.size(); ++i) {
        r.vertical_targets[i] = gl_bd->get_vertical_targets()[i]->get_framebuffer();
    }
    for (std::size_t i = 0; i < r.upsampler_targets.size(); ++i) {
        r.upsampler_targets[i] = gl_bd->get_upsampler_targets()[i]->get_framebuffer();
    }
    return r;
}

gearoenix::gl::submission::Camera::Camera()
    : threads_opaque_models_data(std::thread::hardware_concurrency())
    , threads_translucent_models_data(std::thread::hardware_concurrency())
    , threads_mvps(std::thread::hardware_concurrency())
    , debug_meshes_threads(std::thread::hardware_concurrency())
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
    for (auto& v : threads_translucent_models_data){
        v.clear();
    }
    for (auto& v : threads_mvps){
        v.clear();
    }
    for (auto& v : debug_meshes_threads){
        v.clear();
    }

    const bool has_customised_target = gl_cam.get_target().is_customised();

    const math::Vec2<std::size_t> target_dimension = gl_cam.get_target().get_dimension();

    depth_attachment = static_cast<uint>(-1);
    second_framebuffer = static_cast<uint>(-1);
    second_colour_attachment = static_cast<uint>(-1);

    if(has_customised_target) {
        const auto& target = *gl_cam.get_gl_target().get_customised().target;
        const auto& attachments = target.get_gl_attachments();
        framebuffer = target.get_framebuffer();
        colour_attachment = attachments[0].texture_object;
        if (attachments.size() == 2) {
            // it means we have the main camera not shadow or reflection probe convoluter
            depth_attachment = attachments[1].texture_object;
        }
    }
    else {
        const auto& def = gl_cam.get_gl_target().get_default();
        {
            const auto& target = *def.first;
            const auto& attachments = target.get_gl_attachments();
            framebuffer = target.get_framebuffer();
            colour_attachment = attachments[0].texture_object;
            depth_attachment = attachments[1].texture_object;
        }
        if (nullptr != def.second) {
            const auto& target = *def.second;
            const auto& attachments = target.get_gl_attachments();
            second_framebuffer = target.get_framebuffer();
            second_colour_attachment = attachments[0].texture_object;
        }
    }
    name = &gl_cam.get_name();
    viewport_clip = math::Vec4<sizei>(gl_cam.get_starting_clip_ending_clip() * math::Vec4<float>(target_dimension, target_dimension));
    vp = gl_cam.get_view_projection();
    pos = location;
    skybox_scale = gl_cam.get_far() / 1.732051f;
    exposure_value = gl_cam.get_exposure().get_enabled() ? gl_cam.get_exposure().get_value() : 1.0f;
    colour_tuning = gl_cam.get_colour_tuning();
    bloom_data = BloomData::construct(gl_cam.get_gl_bloom_data(), gl_cam.get_bloom_data());
    out_reference = gl_cam.get_parent_entity_id();
}

#endif