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
    //glGenFramebuffers(1, &shadow_map_framebuffer);
    //glGenRenderbuffers(1, &shadow_map_depth);
    //glBindRenderbuffer(GL_RENDERBUFFER, shadow_map_depth);
    //glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, shadow_map_aspect, shadow_map_aspect);
    //glBindFramebuffer(GL_FRAMEBUFFER, shadow_map_framebuffer);
    //glGenTextures(1, &shadow_map_color);
    //glBindTexture(GL_TEXTURE_2D, shadow_map_color);
    //glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, shadow_map_depth);
    //glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, shadow_map_aspect, shadow_map_aspect, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    //glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, shadow_map_color, 0);
    //if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    //    GXUNEXPECTED;
    //glEnable(GL_CULL_FACE);
    //glCullFace(GL_BACK);
    //glEnable(GL_BLEND);
    //glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    //glEnable(GL_DEPTH_TEST);
    //glEnable(GL_SCISSOR_TEST);
    //glEnable(GL_STENCIL_TEST);
    //glViewport(0, 0, shadow_map_aspect, shadow_map_aspect);
    //glScissor(0, 0, shadow_map_aspect, shadow_map_aspect);
    //glBindRenderbuffer(GL_RENDERBUFFER, render_depth);
    //glBindFramebuffer(GL_FRAMEBUFFER, render_framebuffer);
    //if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    //    GXUNEXPECTED;
    //shadow_map_texture = new texture::Texture2D(sysapp->get_asset_manager()->create_id(), shadow_map_color, this);
    //pipeline_manager = new render::pipeline::Manager(this);
#ifdef GX_GLES2_ENGINE_PROFILING
    prof_last_time_draw = std::chrono::high_resolution_clock::now();
#endif
}

gearoenix::gles2::engine::Engine::Engine(system::Application* const sys_app) noexcept
    : render::engine::Engine(sys_app, render::engine::Type::OPENGL_ES2)
{
    initialize();
}

std::shared_ptr<gearoenix::gles2::engine::Engine> gearoenix::gles2::engine::Engine::construct(system::Application* const sys_app) noexcept
{
    std::shared_ptr<Engine> e(new Engine(sys_app));
    e->pipeline_manager = new pipeline::Manager(e);
    e->buffer_manager = new buffer::Manager(e);
    e->command_manager = new command::Manager();
    e->main_render_target = new texture::Target(e);
    return e;
}

gearoenix::gles2::engine::Engine::~Engine() noexcept
{
    terminate();
}

void gearoenix::gles2::engine::Engine::update() noexcept
{
    gl::Loader::clear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    render::engine::Engine::update();
    //do_load_functions();
    //physics_engine->wait();
    //for (std::pair<const core::Id, std::shared_ptr<render::scene::Scene>>& scene : loaded_scenes) {
    //    glBindRenderbuffer(GL_RENDERBUFFER, shadow_map_depth);
    //    glBindFramebuffer(GL_FRAMEBUFFER, shadow_map_framebuffer);
    //    glViewport(0, 0, shadow_map_aspect, shadow_map_aspect);
    //    glScissor(0, 0, shadow_map_aspect, shadow_map_aspect);
    //    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    //    scene.second->cast_shadow();
    //    glBindRenderbuffer(GL_RENDERBUFFER, render_depth);
    //    glBindFramebuffer(GL_FRAMEBUFFER, render_framebuffer);
    //    glViewport(0, 0, (GLsizei)win_width, (GLsizei)win_height);
    //    glScissor(0, 0, (GLsizei)win_width, (GLsizei)win_height);
    //    glClear(GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    //    glDisable(GL_DEPTH_TEST);
    //    glDepthMask(GL_FALSE);
    //    scene.second->draw_sky();
    //    glEnable(GL_DEPTH_TEST);
    //    glDepthMask(GL_TRUE);
    //    scene.second->draw(shadow_map_texture);
    //}
    //physics_engine->update();
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
        GXLOGI("Average frame per second in 2000 loop is: " << prof_frames_time);
        prof_frames_count = 0;
        prof_frames_time = 0.0f;
    }
    prof_last_time_draw = now;
#endif
}

void gearoenix::gles2::engine::Engine::terminate() noexcept
{
    render::engine::Engine::terminate();
    //if (shadow_map_texture != nullptr) {
    //    glDeleteFramebuffers(1, &shadow_map_framebuffer);
    //    shadow_map_framebuffer = 0;
    //    glDeleteRenderbuffers(1, &shadow_map_depth);
    //    shadow_map_depth = 0;
    //    delete shadow_map_texture;
    //    shadow_map_texture = nullptr;
    //}
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
    return new texture::Texture2D(id, std::static_pointer_cast<Engine>(sysapp->get_render_engine()), data, f, s, img_width, img_height, call);
}

gearoenix::render::texture::Cube* gearoenix::gles2::engine::Engine::create_texture_cube(
    const core::Id id,
    const void* data,
    const render::texture::TextureFormat::Id f,
    const render::texture::SampleInfo s,
    const unsigned int aspect,
    const core::sync::EndCaller<core::sync::EndCallerIgnore>& call) noexcept
{
    return new texture::Cube(id, std::static_pointer_cast<Engine>(sysapp->get_render_engine()), data, f, s, aspect, call);
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
        static_cast<const command::Buffer*>(cmds[i])->play();
}

//gearoenix::render::texture::Texture2D* gearoenix::gles2::engine::Engine::create_texture_2d(core::Id id, system::stream::Stream* file, core::sync::EndCaller<core::sync::EndCallerIgnore> c)
//{
//    return new texture::Texture2D(id, file, this, c);
//}
//
//gearoenix::render::texture::Cube* gearoenix::gles2::engine::Engine::create_texture_cube(core::Id id, system::stream::Stream* file, core::sync::EndCaller<core::sync::EndCallerIgnore> c)
//{
//    return new texture::Cube(id, file, this, c);
//}
//
//gearoenix::render::buffer::Mesh* gearoenix::gles2::engine::Engine::create_mesh(unsigned int vec, system::stream::Stream* file, core::sync::EndCaller<core::sync::EndCallerIgnore> c)
//{
//    return new buffer::Mesh(vec, file, this, c);
//}
//
//gearoenix::render::buffer::Uniform* gearoenix::gles2::engine::Engine::create_uniform(unsigned int s, core::sync::EndCaller<core::sync::EndCallerIgnore>)
//{
//    return new buffer::Uniform(s, this);
//}
//
//gearoenix::render::shader::Shader* gearoenix::gles2::engine::Engine::create_shader(core::Id sid, system::stream::Stream*, core::sync::EndCaller<core::sync::EndCallerIgnore> c)
//{
//    render::shader::Id shader_id = static_cast<render::shader::Id>(sid);
//    switch (shader_id) {
//    case render::shader::DEPTH_POS:
//    case render::shader::DEPTH_POS_NRM:
//    case render::shader::DEPTH_POS_NRM_UV:
//    case render::shader::DEPTH_POS_UV:
//        return new shader::Depth(shader_id, this, c);
//    case render::shader::DIRECTIONAL_COLORED_MATTE_NONREFLECTIVE_SHADOWLESS_OPAQUE:
//        return new shader::DirectionalColoredMatteNonreflectiveShadowlessOpaque(sid, this, c);
//    case render::shader::DIRECTIONAL_COLORED_SPECULATED_BAKED_CASTER_OPAQUE:
//    case render::shader::DIRECTIONAL_COLORED_SPECULATED_BAKED_SHADOWLESS_OPAQUE:
//        return new shader::DirectionalColoredSpeculatedBakedShadowlessOpaque(sid, this, c);
//    case render::shader::DIRECTIONAL_COLORED_SPECULATED_BAKED_FULL_OPAQUE:
//        return new shader::DirectionalColoredSpeculatedBakedFullOpaque(sid, this, c);
//    case render::shader::DIRECTIONAL_COLORED_SPECULATED_NONREFLECTIVE_SHADOWLESS_OPAQUE:
//        return new shader::DirectionalColoredSpeculatedNonreflectiveShadowlessOpaque(sid, this, c);
//    case render::shader::DIRECTIONAL_D2_SPECULATED_BAKED_FULL_OPAQUE:
//        return new shader::DirectionalD2SpeculatedBakedFullOpaque(sid, this, c);
//    case render::shader::DIRECTIONAL_D2_SPECULATED_NONREFLECTIVE_FULL_OPAQUE:
//        return new shader::DirectionalD2SpeculatedNonreflectiveFullOpaque(sid, this, c);
//    case render::shader::DIRECTIONAL_D2_SPECULATED_NONREFLECTIVE_SHADOWLESS_OPAQUE:
//        return new shader::DirectionalD2SpeculatedNonreflectiveShadowlessOpaque(sid, this, c);
//    case render::shader::FONT_COLORED:
//        return new shader::FontColored(sid, this, c);
//    case render::shader::SHADELESS_COLORED_MATTE_NONREFLECTIVE_CASTER_OPAQUE:
//    case render::shader::SHADELESS_COLORED_MATTE_NONREFLECTIVE_SHADOWLESS_OPAQUE:
//    case render::shader::SHADELESS_COLORED_MATTE_NONREFLECTIVE_SHADOWLESS_TRANSPARENT:
//        return new shader::ShadelessColoredMatteNonreflectiveShadowlessOpaque(sid, this, c);
//    case render::shader::SHADELESS_CUBE_MATTE_NONREFLECTIVE_SHADOWLESS_OPAQUE:
//        return new shader::ShadelessCubeMatteNonreflectiveShadowlessOpaque(sid, this, c);
//    case render::shader::SHADELESS_D2_MATTE_NONREFLECTIVE_CASTER_OPAQUE:
//    case render::shader::SHADELESS_D2_MATTE_NONREFLECTIVE_SHADOWLESS_OPAQUE:
//    case render::shader::SHADELESS_D2_MATTE_NONREFLECTIVE_SHADOWLESS_TRANSPARENT:
//        return new shader::ShadelessD2MatteNonreflectiveShadowlessOpaque(sid, this, c);
//    case render::shader::SKYBOX_BASIC:
//        return new shader::SkyboxBasic(sid, this, c);
//    default:
//        GXUNEXPECTED;
//    }
//}
//
//gearoenix::render::shader::Resources* gearoenix::gles2::engine::Engine::create_shader_resources(core::Id sid, render::pipeline::Pipeline* p, render::buffer::Uniform* ub, core::sync::EndCaller<core::sync::EndCallerIgnore>)
//{
//    pipeline::Pipeline* pip = static_cast<pipeline::Pipeline*>(p);
//    buffer::Uniform* u = static_cast<buffer::Uniform*>(ub);
//    switch (sid) {
//    case render::shader::DEPTH_POS:
//    case render::shader::DEPTH_POS_NRM:
//    case render::shader::DEPTH_POS_NRM_UV:
//    case render::shader::DEPTH_POS_UV:
//        return new shader::Depth::Resources(this, pip, u);
//    case render::shader::DIRECTIONAL_COLORED_MATTE_NONREFLECTIVE_SHADOWLESS_OPAQUE:
//        return new shader::DirectionalColoredMatteNonreflectiveShadowlessOpaque::Resources(this, pip, u);
//    case render::shader::DIRECTIONAL_COLORED_SPECULATED_BAKED_CASTER_OPAQUE:
//    case render::shader::DIRECTIONAL_COLORED_SPECULATED_BAKED_SHADOWLESS_OPAQUE:
//        return new shader::DirectionalColoredSpeculatedBakedShadowlessOpaque::Resources(this, pip, u);
//    case render::shader::DIRECTIONAL_COLORED_SPECULATED_BAKED_FULL_OPAQUE:
//        return new shader::DirectionalColoredSpeculatedBakedFullOpaque::Resources(this, pip, u);
//    case render::shader::DIRECTIONAL_COLORED_SPECULATED_NONREFLECTIVE_SHADOWLESS_OPAQUE:
//        return new shader::DirectionalColoredSpeculatedNonreflectiveShadowlessOpaque::Resources(this, pip, u);
//    case render::shader::DIRECTIONAL_D2_SPECULATED_BAKED_FULL_OPAQUE:
//        return new shader::DirectionalD2SpeculatedBakedFullOpaque::Resources(this, pip, u);
//    case render::shader::DIRECTIONAL_D2_SPECULATED_NONREFLECTIVE_FULL_OPAQUE:
//        return new shader::DirectionalD2SpeculatedNonreflectiveFullOpaque::Resources(this, pip, u);
//    case render::shader::DIRECTIONAL_D2_SPECULATED_NONREFLECTIVE_SHADOWLESS_OPAQUE:
//        return new shader::DirectionalD2SpeculatedNonreflectiveShadowlessOpaque::Resources(this, pip, u);
//    case render::shader::FONT_COLORED:
//        return new shader::FontColored::Resources(this, pip, u);
//    case render::shader::SHADELESS_COLORED_MATTE_NONREFLECTIVE_CASTER_OPAQUE:
//    case render::shader::SHADELESS_COLORED_MATTE_NONREFLECTIVE_SHADOWLESS_OPAQUE:
//    case render::shader::SHADELESS_COLORED_MATTE_NONREFLECTIVE_SHADOWLESS_TRANSPARENT:
//        return new shader::ShadelessColoredMatteNonreflectiveShadowlessOpaque::Resources(this, pip, u);
//    case render::shader::SHADELESS_CUBE_MATTE_NONREFLECTIVE_SHADOWLESS_OPAQUE:
//        return new shader::ShadelessCubeMatteNonreflectiveShadowlessOpaque::Resources(this, pip, u);
//    case render::shader::SHADELESS_D2_MATTE_NONREFLECTIVE_CASTER_OPAQUE:
//    case render::shader::SHADELESS_D2_MATTE_NONREFLECTIVE_SHADOWLESS_OPAQUE:
//    case render::shader::SHADELESS_D2_MATTE_NONREFLECTIVE_SHADOWLESS_TRANSPARENT:
//        return new shader::ShadelessD2MatteNonreflectiveShadowlessOpaque::Resources(this, pip, u);
//    case render::shader::SKYBOX_BASIC:
//        return new shader::SkyboxBasic::Resources(this, pip, u);
//    default:
//        GXUNEXPECTED;
//    }
//}
//
//gearoenix::render::pipeline::Pipeline* gearoenix::gles2::engine::Engine::create_pipeline(core::Id sid, core::sync::EndCaller<core::sync::EndCallerIgnore> c)
//{
//    return new pipeline::Pipeline(sid, this, c);
//}
//
//void gearoenix::gles2::engine::Engine::on_event(core::event::Event& e)
//{
//    render::Engine::on_event(e);
//    switch (e.get_type()) {
//    case core::event::Event::From::SYSTEM: {
//        const core::event::system::System& se = e.to_system();
//        switch (se.get_action()) {
//        case core::event::system::System::Action::UNLOAD: {
//            terminate();
//            break;
//        }
//        case core::event::system::System::Action::RELOAD: {
//            initialize();
//            break;
//        }
//        default:
//            break;
//        }
//        break;
//    }
//    case core::event::Event::From::WINDOW_RESIZE: {
//        const core::event::WindowResize& event = e.to_window_resize();
//        win_width = event.get_current_width();
//        win_height = event.get_current_height();
//        break;
//    }
//    default:
//        break;
//    }
//}

#endif
