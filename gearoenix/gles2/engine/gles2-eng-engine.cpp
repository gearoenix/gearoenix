#include "gles2-eng-engine.hpp"
#ifdef GX_USE_OPENGL_ES2
#include "../../core/asset/cr-asset-manager.hpp"
#include "../../core/event/cr-ev-event.hpp"
#include "../../core/event/cr-ev-sys-system.hpp"
#include "../../core/event/cr-ev-window-resize.hpp"
#include "../../core/sync/cr-sync-end-caller.hpp"
#include "../../gl/gl-constants.hpp"
#include "../../gl/gl-loader.hpp"
#include "../../physics/phs-engine.hpp"
#include "../../render/camera/rnd-cmr-camera.hpp"
#include "../../render/pipeline/rnd-pip-manager.hpp"
#include "../../render/scene/rnd-scn-scene.hpp"
#include "../../system/sys-app.hpp"
#include "../../system/sys-log.hpp"
#include "../buffer/gles2-buf-manager.hpp"
#include "../buffer/gles2-buf-uniform.hpp"
#include "../command/gles2-cmd-buffer.hpp"
#include "../command/gles2-cmd-manager.hpp"
#include "../pipeline/gles2-pip-manager.hpp"
#include "../sync/gles2-sy-semaphore.hpp"
#include "../texture/gles2-txt-2d.hpp"
#include "../texture/gles2-txt-cube.hpp"
#include "../texture/gles2-txt-target.hpp"

void gearoenix::gles2::engine::Engine::initialize() noexcept
{
#ifdef GX_GLES2_ENGINE_PROFILING
    prof_last_time_draw = std::chrono::high_resolution_clock::now();
#endif
}

gearoenix::gles2::engine::Engine::Engine(system::Application* const sys_app) noexcept
    : render::engine::Engine(sys_app, render::engine::Type::OPENGL_ES2)
{
    initialize();
	pipeline_manager = new pipeline::Manager(this);
}

std::shared_ptr<gearoenix::gles2::engine::Engine> gearoenix::gles2::engine::Engine::construct(system::Application* const sys_app) noexcept
{
    gl::Loader::get_error();
#ifdef GX_DEBUG_GLES2
    gl::Loader::check_for_error();
#endif
    std::shared_ptr<Engine> e(new Engine(sys_app));
    e->buffer_manager = new buffer::Manager(e.get());
    e->command_manager = new command::Manager();
#ifdef GX_DEBUG_GLES2
    gl::Loader::check_for_error();
#endif
    e->main_render_target = std::shared_ptr<render::texture::Target>(new texture::Target(e.get()));
#ifdef GX_DEBUG_GLES2
    gl::Loader::check_for_error();
#endif
    return e;
}

gearoenix::gles2::engine::Engine::~Engine() noexcept
{
    terminate();
}

void gearoenix::gles2::engine::Engine::update() noexcept
{
#ifdef GX_DEBUG_GLES2
    gl::Loader::check_for_error();
#endif
    gl::Loader::clear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    render::engine::Engine::update();
#ifdef GX_DEBUG_GLES2
    gl::Loader::check_for_error();
#endif
#ifdef GX_GLES2_ENGINE_PROFILING
    auto now = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> time_span = std::chrono::duration_cast<std::chrono::duration<double>>(now - prof_last_time_draw);
    prof_frames_time += time_span.count();
    prof_frames_count++;
    if (prof_frames_count > 1999) {
        prof_frames_time = 2000.0 / prof_frames_time;
        GXLOGI("Average frame per second in 2000 loop is: " << prof_frames_time)
        prof_frames_count = 0;
        prof_frames_time = 0.0f;
    }
    prof_last_time_draw = now;
#endif
}

void gearoenix::gles2::engine::Engine::terminate() noexcept
{
    render::engine::Engine::terminate();
}

gearoenix::render::sync::Semaphore* gearoenix::gles2::engine::Engine::create_semaphore() const noexcept
{
    return new sync::Semaphore();
}

gearoenix::render::texture::Texture2D* gearoenix::gles2::engine::Engine::create_texture_2d(
    const core::Id id,
    const void* data,
    const render::texture::TextureFormat::Id f,
    const render::texture::SampleInfo s,
    const unsigned int img_width,
    const unsigned int img_height,
    const core::sync::EndCaller<core::sync::EndCallerIgnore>& call) noexcept
{
    return new texture::Texture2D(id, this, data, f, s, img_width, img_height, call);
}

gearoenix::render::texture::Cube* gearoenix::gles2::engine::Engine::create_texture_cube(
    const core::Id id,
    const void* data,
    const render::texture::TextureFormat::Id f,
    const render::texture::SampleInfo s,
    const unsigned int aspect,
    const core::sync::EndCaller<core::sync::EndCallerIgnore>& call) noexcept
{
    return new texture::Cube(id, this, data, f, s, aspect, call);
}

gearoenix::render::texture::Target* gearoenix::gles2::engine::Engine::create_render_target(
    core::Id id,
	const std::vector<render::texture::Info>& infos,
    unsigned int width,
    unsigned int height,
    const gearoenix::core::sync::EndCaller<gearoenix::core::sync::EndCallerIgnore>& call) noexcept
{
    return new texture::Target(id, this, infos, width, height, call);
}

void gearoenix::gles2::engine::Engine::submit(
    const std::size_t,
    const render::sync::Semaphore* const* const,
    const std::size_t cmds_count,
    const render::command::Buffer* const* const cmds,
    const std::size_t,
    const render::sync::Semaphore* const* const) noexcept
{
    for (std::size_t i = 0; i < cmds_count; ++i)
        dynamic_cast<const command::Buffer*>(cmds[i])->play();
}

#endif
