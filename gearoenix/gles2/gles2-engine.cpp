#include "gles2-engine.hpp"
#ifdef USE_OPENGL_ES2
#include "../core/asset/cr-asset-manager.hpp"
#include "../core/cr-end-caller.hpp"
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
#include "shader/gles2-shd-shadeless-colored-matte-nonreflective-shadowless-opaque.hpp"
#include "shader/gles2-shd-shadeless-cube-matte-nonreflective-shadowless-opaque.hpp"
#include "shader/gles2-shd-shadeless-d2-matte-nonreflective-shadowless-opaque.hpp"
#include "texture/gles2-txt-2d.hpp"
#include "texture/gles2-txt-cube.hpp"

#define SHADOW_MAP
//#define INTA_TEST001

#ifdef INTA_TEST001
static GLuint vbo, ibo;
static std::shared_ptr<gearoenix::gles2::shader::DirectionalTexturedSpeculatedNocubeNoshadowOpaque> shd;
static std::shared_ptr<gearoenix::render::camera::Camera> cam;
static std::shared_ptr<gearoenix::gles2::texture::Texture2D> txt;
#endif

gearoenix::gles2::Engine::Engine(system::Application* sysapp)
    : render::Engine(sysapp)
{
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    win_width = sysapp->get_width();
    win_height = sysapp->get_height();
    glGetIntegerv(GL_FRAMEBUFFER_BINDING, (GLint*)&render_framebuffer);
    glGetIntegerv(GL_RENDERBUFFER_BINDING, (GLint*)&render_depth);
#ifdef SHADOW_MAP
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
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        UNEXPECTED;
    }
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_SCISSOR_TEST);
    glEnable(GL_STENCIL_TEST);
    glViewport(0, 0, (GLfloat)shadow_map_aspect, (GLfloat)shadow_map_aspect);
    glScissor(0, 0, (GLfloat)shadow_map_aspect, (GLfloat)shadow_map_aspect);
    glBindRenderbuffer(GL_RENDERBUFFER, render_depth);
    glBindFramebuffer(GL_FRAMEBUFFER, render_framebuffer);
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        UNEXPECTED;
    }
    shadow_map_texture = new texture::Texture2D(shadow_map_color);
#endif
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_SCISSOR_TEST);
    glEnable(GL_STENCIL_TEST);
    glViewport(0, 0, win_width, win_height);
    glScissor(0, 0, win_width, win_height);
    pipmgr = new render::pipeline::Manager(this);
#ifdef INTA_TEST001
    const GLfloat vertices[] = {
        0.0f, 1.0f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 2.0f,
        1.0f, 0.0f, -0.5f, 0.0f, 0.0f, 1.0f, 2.0f, 0.0f,
        -1.0f, 0.0f, -0.5f, 0.0f, 0.0f, 1.0f, -2.0f, 0.0f,
    };
    const GLushort indices[] = {
        0, 2, 1,
    };
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glGenBuffers(1, &ibo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
#endif
#ifdef GLES2_PROFILING
    prof_last_time_draw = std::chrono::high_resolution_clock::now();
#endif
}

gearoenix::gles2::Engine::~Engine()
{
    TODO;
}

void gearoenix::gles2::Engine::window_changed()
{
    TODO;
}

void gearoenix::gles2::Engine::update()
{
#ifdef INTA_TEST001
    static int first_happen = 0;
    if (first_happen == 0) {
        ++first_happen;
        cam = sysapp->get_asset_manager()->get_camera(0);
        shd = std::static_pointer_cast<shader::DirectionalTexturedSpeculatedNocubeNoshadowOpaque>(
            sysapp->get_asset_manager()->get_shader(
                render::shader::DIRECTIONAL_TEXTURED_SPECULATED_NOCUBE_FULLSHADOW_OPAQUE,
                core::EndCaller::create([&] { ++first_happen; })));
        txt = std::static_pointer_cast<texture::Texture2D>(
            sysapp->get_asset_manager()->get_texture(
                0, core::EndCaller::create([&] { ++first_happen; })));
    }
#endif
    glClear(GL_COLOR_BUFFER_BIT);
    ///////////////////////////////////////////////////////////////////////////////////////////////
    std::vector<std::function<void()>> temp_functions;
    load_functions_mutex.lock();
    std::move(load_functions.begin(), load_functions.end(), std::back_inserter(temp_functions));
    load_functions.clear();
    load_functions_mutex.unlock();
    for (std::function<void()>& f : temp_functions) {
        f();
    }
    temp_functions.clear();
#ifdef INTA_TEST001
    if (first_happen == 3) {
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
        shd->use();
        shd->set_mvp(cam->get_view_projection().data());
        //        shd->set_mvp(math::Mat4x4().data());
        txt->bind(GL_TEXTURE0);
        glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_SHORT, 0);
    }
#else
    //    for (std::shared_ptr<render::scene::Scene>& scene : loaded_scenes) {
    //        scene->commit();
    //    }
    physics_engine->wait();

    for (std::shared_ptr<render::scene::Scene>& scene : loaded_scenes) {
        glBindRenderbuffer(GL_RENDERBUFFER, shadow_map_depth);
        glBindFramebuffer(GL_FRAMEBUFFER, shadow_map_framebuffer);
        glViewport(0, 0, (GLfloat)shadow_map_aspect, (GLfloat)shadow_map_aspect);
        glScissor(0, 0, (GLfloat)shadow_map_aspect, (GLfloat)shadow_map_aspect);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

        scene->cast_shadow();

        glBindRenderbuffer(GL_RENDERBUFFER, render_depth);
        glBindFramebuffer(GL_FRAMEBUFFER, render_framebuffer);
        glViewport(0, 0, win_width, win_height);
        glScissor(0, 0, win_width, win_height);
        glClear(GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

        scene->draw(shadow_map_texture);
    }
    physics_engine->update();
#endif
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
    TODO;
}

gearoenix::render::texture::Texture2D* gearoenix::gles2::Engine::create_texture_2d(system::File* file, std::shared_ptr<core::EndCaller> c)
{
    return new texture::Texture2D(file, this, c);
}

gearoenix::render::texture::Cube* gearoenix::gles2::Engine::create_texture_cube(system::File* file, std::shared_ptr<core::EndCaller> c)
{
    return new texture::Cube(file, this, c);
}

gearoenix::render::buffer::Mesh* gearoenix::gles2::Engine::create_mesh(unsigned int vec, system::File* file, std::shared_ptr<core::EndCaller> c)
{
    return new buffer::Mesh(vec, file, this, c);
}

gearoenix::render::buffer::Uniform* gearoenix::gles2::Engine::create_uniform(unsigned int s, std::shared_ptr<core::EndCaller>)
{
    return new buffer::Uniform(s, this);
}

gearoenix::render::shader::Shader* gearoenix::gles2::Engine::create_shader(core::Id sid, system::File*, std::shared_ptr<core::EndCaller> c)
{
    render::shader::Id shader_id = static_cast<render::shader::Id>(sid);
    switch (shader_id) {
    case render::shader::DEPTH_POS:
    case render::shader::DEPTH_POS_NRM:
    case render::shader::DEPTH_POS_NRM_UV:
    case render::shader::DEPTH_POS_UV:
        return new shader::Depth(shader_id, this, c);
    case render::shader::DIRECTIONAL_COLORED_MATTE_NONREFLECTIVE_SHADOWLESS_OPAQUE:
        return new shader::DirectionalColoredMatteNonreflectiveShadowlessOpaque(this, c);
    case render::shader::DIRECTIONAL_COLORED_SPECULATED_BAKED_CASTER_OPAQUE:
    case render::shader::DIRECTIONAL_COLORED_SPECULATED_BAKED_SHADOWLESS_OPAQUE:
        return new shader::DirectionalColoredSpeculatedBakedShadowlessOpaque(this, c);
    case render::shader::DIRECTIONAL_COLORED_SPECULATED_BAKED_FULL_OPAQUE:
        return new shader::DirectionalColoredSpeculatedBakedFullOpaque(this, c);
    case render::shader::DIRECTIONAL_COLORED_SPECULATED_NONREFLECTIVE_SHADOWLESS_OPAQUE:
        return new shader::DirectionalColoredSpeculatedNonreflectiveShadowlessOpaque(this, c);
    case render::shader::DIRECTIONAL_D2_SPECULATED_BAKED_FULL_OPAQUE:
        return new shader::DirectionalD2SpeculatedBakedFullOpaque(this, c);
    case render::shader::DIRECTIONAL_D2_SPECULATED_NONREFLECTIVE_FULL_OPAQUE:
        return new shader::DirectionalD2SpeculatedNonreflectiveFullOpaque(this, c);
    case render::shader::DIRECTIONAL_D2_SPECULATED_NONREFLECTIVE_SHADOWLESS_OPAQUE:
        return new shader::DirectionalD2SpeculatedNonreflectiveShadowlessOpaque(this, c);
    case render::shader::SHADELESS_COLORED_MATTE_NONREFLECTIVE_SHADOWLESS_OPAQUE:
        return new shader::ShadelessColoredMatteNonreflectiveShadowlessOpaque(this, c);
    case render::shader::SHADELESS_CUBE_MATTE_NONREFLECTIVE_SHADOWLESS_OPAQUE:
        return new shader::ShadelessCubeMatteNonreflectiveShadowlessOpaque(this, c);
    case render::shader::SHADELESS_D2_MATTE_NONREFLECTIVE_CASTER_OPAQUE:
    case render::shader::SHADELESS_D2_MATTE_NONREFLECTIVE_SHADOWLESS_OPAQUE:
        return new shader::ShadelessD2MatteNonreflectiveShadowlessOpaque(this, c);
    default:
        UNEXPECTED;
    }
}

gearoenix::render::shader::Resources* gearoenix::gles2::Engine::create_shader_resources(core::Id sid, render::pipeline::Pipeline* p, render::buffer::Uniform* ub, std::shared_ptr<core::EndCaller>)
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
    case render::shader::SHADELESS_COLORED_MATTE_NONREFLECTIVE_SHADOWLESS_OPAQUE:
        return new shader::ShadelessColoredMatteNonreflectiveShadowlessOpaque::Resources(this, pip, u);
    case render::shader::SHADELESS_CUBE_MATTE_NONREFLECTIVE_SHADOWLESS_OPAQUE:
        return new shader::ShadelessCubeMatteNonreflectiveShadowlessOpaque::Resources(this, pip, u);
    case render::shader::SHADELESS_D2_MATTE_NONREFLECTIVE_CASTER_OPAQUE:
    case render::shader::SHADELESS_D2_MATTE_NONREFLECTIVE_SHADOWLESS_OPAQUE:
        return new shader::ShadelessD2MatteNonreflectiveShadowlessOpaque::Resources(this, pip, u);
    default:
        UNEXPECTED;
    }
}

gearoenix::render::pipeline::Pipeline* gearoenix::gles2::Engine::create_pipeline(core::Id sid, std::shared_ptr<core::EndCaller> c)
{
    return new pipeline::Pipeline(sid, this, c);
}

#endif
