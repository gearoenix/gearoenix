#include "gles2-engine.hpp"
#ifdef USE_OPENGL_ES2
#include "../core/asset/cr-asset-manager.hpp"
#include "../render/camera/rnd-cmr-camera.hpp"
#include "../system/sys-app.hpp"
#include "../system/sys-log.hpp"
#include "shader/gles2-shd-directional-colored-speculated-nocube-noshadow-opaque.hpp"

gearoenix::gles2::Engine::Engine(system::Application* sysapp)
    : render::Engine(sysapp)
{
    glClearColor(0.3f, 0.3f, 0.3f, 1.f);
    //    glEnable(GL_CULL_FACE);
    //    glCullFace(GL_BACK);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_SCISSOR_TEST);
    glEnable(GL_STENCIL_TEST);
    float width = sysapp->get_width();
    float height = sysapp->get_height();
    glViewport(0, 0, width, height);
    glScissor(0, 0, width, height);
    glGenFramebuffers(1, &shadow_map_framebuffer);
    GLuint shadow_map_render_buffer;
    glGenRenderbuffers(1, &shadow_map_render_buffer);
    glBindRenderbuffer(GL_RENDERBUFFER, shadow_map_render_buffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, 1024, 1024);
    glBindFramebuffer(GL_FRAMEBUFFER, shadow_map_framebuffer);
    glGenTextures(1, &shadow_map_texture);
    glBindTexture(GL_TEXTURE_2D, shadow_map_texture);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, shadow_map_render_buffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1024, 1024, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, shadow_map_texture, 0);
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        UNEXPECTED;
    }
    //    glEnable(GL_CULL_FACE);
    //    glCullFace(GL_BACK);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_SCISSOR_TEST);
    glEnable(GL_STENCIL_TEST);
    glViewport(0, 0, 1024, 1024);
    glScissor(0, 0, 1024, 1024);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, width, height);
    glScissor(0, 0, width, height);

    const GLfloat vertices[] = {
        0.0f, 1.0f, -0.5f, 0.0f, 0.0f, 1.0f,
        1.0f, 0.0f, -0.5f, 0.0f, 0.0f, 1.0f,
        -1.0f, 0.0f, -0.5f, 0.0f, 0.0f, 1.0f,
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
    shd = new shader::DirectionalColoredSpeculatedNocubeNoshadowOpaque();
    vp = math::Mat4x4::look_at(
        math::Vec3(0.0f, 0.0f, 0.0f),
        math::Vec3(0.0f, 0.0f, -1.0f),
        math::Vec3(0.0f, 1.0f, 0.0f));
    vp = math::Mat4x4::perspective(1.7f, 1.0f, 0.1f, 100.0f) * vp;
    vp *= math::Mat4x4::translator(math::Vec3(0.0f, 0.0f, 0.0f));
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
    static bool first_happen = true;
    if (first_happen) {
        first_happen = false;
        cam = sysapp->get_asset_manager()->get_camera(0);
    }
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
    shd->use();
    //    shd->set_mvp(vp.get_data());
    shd->set_mvp(cam->get_view_projection().get_data());
    glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_SHORT, 0);
}

void gearoenix::gles2::Engine::terminate()
{
    TODO;
}

#endif
