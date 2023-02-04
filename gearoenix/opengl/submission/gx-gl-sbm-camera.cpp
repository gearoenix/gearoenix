#include "gx-gl-sbm-camera.hpp"
#ifdef GX_RENDER_OPENGL_ENABLED
#include "../../physics/gx-phs-transformation.hpp"
#include "../../render/camera/gx-rnd-cmr-camera.hpp"
#include "../gx-gl-camera.hpp"
#include "../gx-gl-target.hpp"
#include "../gx-gl-texture.hpp"
#include <thread>

std::optional<gearoenix::gl::submission::BloomData> gearoenix::gl::submission::BloomData::construct(
    const std::optional<gl::BloomData>& gl_bd,
    const std::optional<render::camera::BloomData>& rnd_bd) noexcept
{
    if (!rnd_bd.has_value())
        return std::nullopt;
    BloomData r;
    r.scatter_clamp_max_threshold_threshold_knee = rnd_bd->get_scatter_clamp_max_threshold_threshold_knee();
    r.prefilter_target = gl_bd->get_prefilter_target()->get_framebuffer();
    for (std::size_t i = 0; i < r.horizontal_targets.size(); ++i)
        r.horizontal_targets[i] = gl_bd->get_horizontal_targets()[i]->get_framebuffer();
    for (std::size_t i = 0; i < r.vertical_targets.size(); ++i)
        r.vertical_targets[i] = gl_bd->get_vertical_targets()[i]->get_framebuffer();
    for (std::size_t i = 0; i < r.upsampler_targets.size(); ++i)
        r.upsampler_targets[i] = gl_bd->get_upsampler_targets()[i]->get_framebuffer();
    return r;
}

gearoenix::gl::submission::Camera::Camera() noexcept
    : threads_opaque_models_data(std::thread::hardware_concurrency())
    , threads_translucent_models_data(std::thread::hardware_concurrency())
    , threads_mvps(std::thread::hardware_concurrency())
    , debug_meshes_threads(std::thread::hardware_concurrency())
{
}

void gearoenix::gl::submission::Camera::clear(
    const gl::Camera& gl_cam,
    const render::camera::Camera& rnd_cam,
    const math::Vec3<float>& location) noexcept
{
    models_data.clear();
    mvps.clear();
    debug_meshes.clear();
    name = nullptr;

    for (auto& v : threads_opaque_models_data)
        v.clear();
    for (auto& v : threads_translucent_models_data)
        v.clear();
    for (auto& v : threads_mvps)
        v.clear();
    for (auto& v : debug_meshes_threads)
        v.clear();

    const auto target_dimension = rnd_cam.get_target()->get_dimension();
    framebuffer = gl_cam.get_target()->get_framebuffer();
    colour_attachment = gl_cam.get_target()->get_gl_attachments()[0].texture_object;
    if (gl_cam.get_target()->get_gl_attachments().size() == 2) {
        // it means we have the main camera not shadow or reflection probe
        depth_attachment = gl_cam.get_target()->get_gl_attachments()[1].texture_object;
    } else {
        depth_attachment = static_cast<uint>(-1);
    }
    if (nullptr != gl_cam.get_second_target()) {
        second_framebuffer = gl_cam.get_second_target()->get_framebuffer();
        second_colour_attachment = gl_cam.get_second_target()->get_gl_attachments()[0].texture_object;
    } else {
        second_framebuffer = static_cast<uint>(-1);
        second_colour_attachment = static_cast<uint>(-1);
    }
    name = &gl_cam.get_name();
    viewport_clip = math::Vec4<sizei>(rnd_cam.get_starting_clip_ending_clip() * math::Vec4<float>(target_dimension, target_dimension));
    vp = rnd_cam.get_view_projection();
    pos = location;
    skybox_scale = rnd_cam.get_far() / 1.732051f;
    exposure_value = rnd_cam.get_exposure().get_enabled() ? rnd_cam.get_exposure().get_value() : 1.0f;
    colour_tuning = rnd_cam.get_colour_tuning();
    bloom_data = BloomData::construct(gl_cam.get_bloom_data(), rnd_cam.get_bloom_data());
    out_reference = rnd_cam.get_reference_id();
}

#endif