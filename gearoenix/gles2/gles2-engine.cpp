#include "gles2-engine.hpp"
#ifdef USE_OPENGL_ES2
#include "../core/asset/cr-asset-manager.hpp"
#include "../core/event/cr-ev-event.hpp"
#include "../core/event/cr-ev-sys-system.hpp"
#include "../core/event/cr-ev-window-resize.hpp"
#include "../core/sync/cr-sync-end-caller.hpp"
#include "../physics/phs-engine.hpp"
#include "../render/camera/rnd-cmr-camera.hpp"
#include "../render/pipeline/rnd-pip-manager.hpp"
#include "../render/scene/rnd-scn-scene.hpp"
#include "../system/sys-app.hpp"
#include "../system/sys-log.hpp"
#include "buffer/gles2-buf-mesh.hpp"
#include "buffer/gles2-buf-uniform.hpp"
#include "pipeline/gles2-pip-pipeline.hpp"
#include "shader/gles2-shd-depth.hpp"
#include "shader/gles2-shd-directional-colored-matte-nonreflective-shadowless-opaque.hpp"
#include "shader/gles2-shd-directional-colored-speculated-baked-full-opaque.hpp"
#include "shader/gles2-shd-directional-colored-speculated-baked-shadowless-opaque.hpp"
#include "shader/gles2-shd-directional-colored-speculated-nonreflective-shadowless-opaque.hpp"
#include "shader/gles2-shd-directional-d2-speculated-baked-full-opaque.hpp"
#include "shader/gles2-shd-directional-d2-speculated-nonreflective-full-opaque.hpp"
#include "shader/gles2-shd-directional-d2-speculated-nonreflective-shadowless-opaque.hpp"
#include "shader/gles2-shd-font-colored.hpp"
#include "shader/gles2-shd-shadeless-colored-matte-nonreflective-shadowless-opaque.hpp"
#include "shader/gles2-shd-shadeless-cube-matte-nonreflective-shadowless-opaque.hpp"
#include "shader/gles2-shd-shadeless-d2-matte-nonreflective-shadowless-opaque.hpp"
#include "shader/gles2-shd-skybox-basic.hpp"
#include "texture/gles2-txt-2d.hpp"
#include "texture/gles2-txt-cube.hpp"

void gearoenix::gles2::Engine::initialize()
{
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    win_width = (GLfloat)sysapp->get_width();
    win_height = (GLfloat)sysapp->get_height();
    glGetIntegerv(GL_FRAMEBUFFER_BINDING, (GLint*)&render_framebuffer);
    glGetIntegerv(GL_RENDERBUFFER_BINDING, (GLint*)&render_depth);
    glGenFramebuffers(1, &shadow_map_framebuffer);
    glGenRenderbuffers(1, &shadow_map_depth);
    glBindRenderbuffer(GL_RENDERBUFFER, shadow_map_depth);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, shadow_map_aspect, shadow_map_aspect);
    glBindFramebuffer(GL_FRAMEBUFFER, shadow_map_framebuffer);
    glGenTextures(1, &shadow_map_color);
    glBindTexture(GL_TEXTURE_2D, shadow_map_color);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, shadow_map_depth);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, shadow_map_aspect, shadow_map_aspect, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, shadow_map_color, 0);
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        UNEXPECTED;
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_SCISSOR_TEST);
    glEnable(GL_STENCIL_TEST);
    glViewport(0, 0, shadow_map_aspect, shadow_map_aspect);
    glScissor(0, 0, shadow_map_aspect, shadow_map_aspect);
    glBindRenderbuffer(GL_RENDERBUFFER, render_depth);
    glBindFramebuffer(GL_FRAMEBUFFER, render_framebuffer);
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        UNEXPECTED;
    shadow_map_texture = new texture::Texture2D(sysapp->get_asset_manager()->create_id(), shadow_map_color);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_SCISSOR_TEST);
    glEnable(GL_STENCIL_TEST);
    glViewport(0, 0, (GLsizei)win_width, (GLsizei)win_height);
    glScissor(0, 0, (GLsizei)win_width, (GLsizei)win_height);
    pipmgr = new render::pipeline::Manager(this);
#ifdef GLES2_PROFILING
    prof_last_time_draw = std::chrono::high_resolution_clock::now();
#endif
}

gearoenix::gles2::Engine::Engine(system::Application* sysapp)
    : render::Engine(sysapp)
{
    initialize();
}

gearoenix::gles2::Engine::~Engine()
{
    terminate();
}

void gearoenix::gles2::Engine::window_changed()
{
    TODO;
}

void gearoenix::gles2::Engine::update()
{
    CHECK_FOR_GRAPHIC_API_ERROR;
    glClear(GL_COLOR_BUFFER_BIT);
    do_load_functions();
    physics_engine->wait();
    for (std::pair<const core::Id, std::shared_ptr<render::scene::Scene>>& scene : loaded_scenes) {
        glBindRenderbuffer(GL_RENDERBUFFER, shadow_map_depth);
        glBindFramebuffer(GL_FRAMEBUFFER, shadow_map_framebuffer);
        glViewport(0, 0, shadow_map_aspect, shadow_map_aspect);
        glScissor(0, 0, shadow_map_aspect, shadow_map_aspect);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
        scene.second->cast_shadow();
        glBindRenderbuffer(GL_RENDERBUFFER, render_depth);
        glBindFramebuffer(GL_FRAMEBUFFER, render_framebuffer);
        glViewport(0, 0, (GLsizei)win_width, (GLsizei)win_height);
        glScissor(0, 0, (GLsizei)win_width, (GLsizei)win_height);
        glClear(GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
        glDisable(GL_DEPTH_TEST);
        glDepthMask(GL_FALSE);
        scene.second->draw_sky();
        glEnable(GL_DEPTH_TEST);
        glDepthMask(GL_TRUE);
        scene.second->draw(shadow_map_texture);
    }
    physics_engine->update();
#ifdef GLES2_PROFILING
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

void gearoenix::gles2::Engine::terminate()
{
    if (shadow_map_texture != nullptr) {
        glDeleteFramebuffers(1, &shadow_map_framebuffer);
        shadow_map_framebuffer = 0;
        glDeleteRenderbuffers(1, &shadow_map_depth);
        shadow_map_depth = 0;
        delete shadow_map_texture;
        shadow_map_texture = nullptr;
    }
}

gearoenix::render::texture::Texture2D* gearoenix::gles2::Engine::create_texture_2d(core::Id id, system::stream::Stream* file, core::sync::EndCaller<core::sync::EndCallerIgnore> c)
{
    return new texture::Texture2D(id, file, this, c);
}

gearoenix::render::texture::Cube* gearoenix::gles2::Engine::create_texture_cube(core::Id id, system::stream::Stream* file, core::sync::EndCaller<core::sync::EndCallerIgnore> c)
{
    return new texture::Cube(id, file, this, c);
}

gearoenix::render::buffer::Mesh* gearoenix::gles2::Engine::create_mesh(unsigned int vec, system::stream::Stream* file, core::sync::EndCaller<core::sync::EndCallerIgnore> c)
{
    return new buffer::Mesh(vec, file, this, c);
}

gearoenix::render::buffer::Uniform* gearoenix::gles2::Engine::create_uniform(unsigned int s, core::sync::EndCaller<core::sync::EndCallerIgnore>)
{
    return new buffer::Uniform(s, this);
}

gearoenix::render::shader::Shader* gearoenix::gles2::Engine::create_shader(core::Id sid, system::stream::Stream*, core::sync::EndCaller<core::sync::EndCallerIgnore> c)
{
    render::shader::Id shader_id = static_cast<render::shader::Id>(sid);
    switch (shader_id) {
    case render::shader::DEPTH_POS:
    case render::shader::DEPTH_POS_NRM:
    case render::shader::DEPTH_POS_NRM_UV:
    case render::shader::DEPTH_POS_UV:
        return new shader::Depth(shader_id, this, c);
    case render::shader::DIRECTIONAL_COLORED_MATTE_NONREFLECTIVE_SHADOWLESS_OPAQUE:
        return new shader::DirectionalColoredMatteNonreflectiveShadowlessOpaque(sid, this, c);
    case render::shader::DIRECTIONAL_COLORED_SPECULATED_BAKED_CASTER_OPAQUE:
    case render::shader::DIRECTIONAL_COLORED_SPECULATED_BAKED_SHADOWLESS_OPAQUE:
        return new shader::DirectionalColoredSpeculatedBakedShadowlessOpaque(sid, this, c);
    case render::shader::DIRECTIONAL_COLORED_SPECULATED_BAKED_FULL_OPAQUE:
        return new shader::DirectionalColoredSpeculatedBakedFullOpaque(sid, this, c);
    case render::shader::DIRECTIONAL_COLORED_SPECULATED_NONREFLECTIVE_SHADOWLESS_OPAQUE:
        return new shader::DirectionalColoredSpeculatedNonreflectiveShadowlessOpaque(sid, this, c);
    case render::shader::DIRECTIONAL_D2_SPECULATED_BAKED_FULL_OPAQUE:
        return new shader::DirectionalD2SpeculatedBakedFullOpaque(sid, this, c);
    case render::shader::DIRECTIONAL_D2_SPECULATED_NONREFLECTIVE_FULL_OPAQUE:
        return new shader::DirectionalD2SpeculatedNonreflectiveFullOpaque(sid, this, c);
    case render::shader::DIRECTIONAL_D2_SPECULATED_NONREFLECTIVE_SHADOWLESS_OPAQUE:
        return new shader::DirectionalD2SpeculatedNonreflectiveShadowlessOpaque(sid, this, c);
    case render::shader::FONT_COLORED:
        return new shader::FontColored(sid, this, c);
    case render::shader::SHADELESS_COLORED_MATTE_NONREFLECTIVE_CASTER_OPAQUE:
    case render::shader::SHADELESS_COLORED_MATTE_NONREFLECTIVE_SHADOWLESS_OPAQUE:
    case render::shader::SHADELESS_COLORED_MATTE_NONREFLECTIVE_SHADOWLESS_TRANSPARENT:
        return new shader::ShadelessColoredMatteNonreflectiveShadowlessOpaque(sid, this, c);
    case render::shader::SHADELESS_CUBE_MATTE_NONREFLECTIVE_SHADOWLESS_OPAQUE:
        return new shader::ShadelessCubeMatteNonreflectiveShadowlessOpaque(sid, this, c);
    case render::shader::SHADELESS_D2_MATTE_NONREFLECTIVE_CASTER_OPAQUE:
    case render::shader::SHADELESS_D2_MATTE_NONREFLECTIVE_SHADOWLESS_OPAQUE:
    case render::shader::SHADELESS_D2_MATTE_NONREFLECTIVE_SHADOWLESS_TRANSPARENT:
        return new shader::ShadelessD2MatteNonreflectiveShadowlessOpaque(sid, this, c);
    case render::shader::SKYBOX_BASIC:
        return new shader::SkyboxBasic(sid, this, c);
    default:
        UNEXPECTED;
    }
}

gearoenix::render::shader::Resources* gearoenix::gles2::Engine::create_shader_resources(core::Id sid, render::pipeline::Pipeline* p, render::buffer::Uniform* ub, core::sync::EndCaller<core::sync::EndCallerIgnore>)
{
    pipeline::Pipeline* pip = reinterpret_cast<pipeline::Pipeline*>(p);
    buffer::Uniform* u = reinterpret_cast<buffer::Uniform*>(ub);
    switch (sid) {
    case render::shader::DEPTH_POS:
    case render::shader::DEPTH_POS_NRM:
    case render::shader::DEPTH_POS_NRM_UV:
    case render::shader::DEPTH_POS_UV:
        return new shader::Depth::Resources(this, pip, u);
    case render::shader::DIRECTIONAL_COLORED_MATTE_NONREFLECTIVE_SHADOWLESS_OPAQUE:
        return new shader::DirectionalColoredMatteNonreflectiveShadowlessOpaque::Resources(this, pip, u);
    case render::shader::DIRECTIONAL_COLORED_SPECULATED_BAKED_CASTER_OPAQUE:
    case render::shader::DIRECTIONAL_COLORED_SPECULATED_BAKED_SHADOWLESS_OPAQUE:
        return new shader::DirectionalColoredSpeculatedBakedShadowlessOpaque::Resources(this, pip, u);
    case render::shader::DIRECTIONAL_COLORED_SPECULATED_BAKED_FULL_OPAQUE:
        return new shader::DirectionalColoredSpeculatedBakedFullOpaque::Resources(this, pip, u);
    case render::shader::DIRECTIONAL_COLORED_SPECULATED_NONREFLECTIVE_SHADOWLESS_OPAQUE:
        return new shader::DirectionalColoredSpeculatedNonreflectiveShadowlessOpaque::Resources(this, pip, u);
    case render::shader::DIRECTIONAL_D2_SPECULATED_BAKED_FULL_OPAQUE:
        return new shader::DirectionalD2SpeculatedBakedFullOpaque::Resources(this, pip, u);
    case render::shader::DIRECTIONAL_D2_SPECULATED_NONREFLECTIVE_FULL_OPAQUE:
        return new shader::DirectionalD2SpeculatedNonreflectiveFullOpaque::Resources(this, pip, u);
    case render::shader::DIRECTIONAL_D2_SPECULATED_NONREFLECTIVE_SHADOWLESS_OPAQUE:
        return new shader::DirectionalD2SpeculatedNonreflectiveShadowlessOpaque::Resources(this, pip, u);
    case render::shader::FONT_COLORED:
        return new shader::FontColored::Resources(this, pip, u);
    case render::shader::SHADELESS_COLORED_MATTE_NONREFLECTIVE_CASTER_OPAQUE:
    case render::shader::SHADELESS_COLORED_MATTE_NONREFLECTIVE_SHADOWLESS_OPAQUE:
    case render::shader::SHADELESS_COLORED_MATTE_NONREFLECTIVE_SHADOWLESS_TRANSPARENT:
        return new shader::ShadelessColoredMatteNonreflectiveShadowlessOpaque::Resources(this, pip, u);
    case render::shader::SHADELESS_CUBE_MATTE_NONREFLECTIVE_SHADOWLESS_OPAQUE:
        return new shader::ShadelessCubeMatteNonreflectiveShadowlessOpaque::Resources(this, pip, u);
    case render::shader::SHADELESS_D2_MATTE_NONREFLECTIVE_CASTER_OPAQUE:
    case render::shader::SHADELESS_D2_MATTE_NONREFLECTIVE_SHADOWLESS_OPAQUE:
    case render::shader::SHADELESS_D2_MATTE_NONREFLECTIVE_SHADOWLESS_TRANSPARENT:
        return new shader::ShadelessD2MatteNonreflectiveShadowlessOpaque::Resources(this, pip, u);
    case render::shader::SKYBOX_BASIC:
        return new shader::SkyboxBasic::Resources(this, pip, u);
    default:
        UNEXPECTED;
    }
}

gearoenix::render::pipeline::Pipeline* gearoenix::gles2::Engine::create_pipeline(core::Id sid, core::sync::EndCaller<core::sync::EndCallerIgnore> c)
{
    return new pipeline::Pipeline(sid, this, c);
}

void gearoenix::gles2::Engine::on_event(core::event::Event& e)
{
    render::Engine::on_event(e);
    switch (e.get_type()) {
    case core::event::Event::From::SYSTEM: {
        const core::event::system::System& se = e.to_system();
        switch (se.get_action()) {
        case core::event::system::System::Action::UNLOAD: {
            terminate();
            break;
        }
        case core::event::system::System::Action::RELOAD: {
            initialize();
            break;
        }
        default:
            break;
        }
        break;
    }
    case core::event::Event::From::WINDOW_RESIZE: {
        const core::event::WindowResize& event = e.to_window_resize();
        win_width = event.get_current_width();
        win_height = event.get_current_height();
        break;
    }
    default:
        break;
    }
}

#endif
