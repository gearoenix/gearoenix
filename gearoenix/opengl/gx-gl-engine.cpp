#include "gx-gl-engine.hpp"
#ifdef GX_RENDER_OPENGL_ENABLED
#include "../core/ecs/gx-cr-ecs-world.hpp"
#include "../platform/gx-plt-application.hpp"
#include "gx-gl-camera.hpp"
#include "gx-gl-constants.hpp"
#include "gx-gl-loader.hpp"
#include "gx-gl-mesh.hpp"
#include "gx-gl-model.hpp"
#include "gx-gl-reflection.hpp"
#include "gx-gl-skybox.hpp"
#include "gx-gl-submission.hpp"
#include "gx-gl-texture.hpp"
#include <imgui_impl_opengl3.h>

gearoenix::gl::Engine::Engine(platform::Application& platform_application) noexcept
    : render::engine::Engine(render::engine::Type::OpenGL, platform_application)
{
    ImGui_ImplOpenGL3_Init("#version 300 es");
    frames_count = GEAROENIX_GL_FRAMES_COUNT;

    sint max_attach = 0;
    glGetIntegerv(GL_MAX_COLOR_ATTACHMENTS, &max_attach);
    specification.texture_maximum_target_attachments = static_cast<unsigned int>(max_attach);
    specification.is_deferred_supported = max_attach >= GEAROENIX_GL_GBUFFER_FRAMEBUFFER_ATTACHMENTS_COUNT;
    specification.is_raytracing_supported = false;
    specification.is_float_texture_supported = extension_exists("GL_OES_texture_float") && extension_exists("GL_OES_texture_float_linear") && extension_exists("GL_OES_texture_half_float") && extension_exists("GL_OES_texture_half_float_linear");

    // specification.is_float_texture_supported = true;
    // specification.is_deferred_supported = false;

    camera_manager = std::make_unique<CameraManager>(*this);
    mesh_manager = std::make_unique<MeshManager>(*this);
    model_manager = std::make_unique<ModelManager>(*this);
    texture_manager = std::make_unique<TextureManager>(*this);
    submission_manager = std::make_unique<SubmissionManager>(*this);
    skybox_manager = std::make_unique<SkyboxManager>(*this);
    reflection_manager = std::make_unique<ReflectionManager>(*this);
    todos.unload();
}

gearoenix::gl::Engine::~Engine() noexcept
{
    world = nullptr;
    skybox_manager = nullptr;
    submission_manager = nullptr;
    texture_manager = nullptr;
    model_manager = nullptr;
    mesh_manager = nullptr;
    camera_manager = nullptr;
    todos.unload();
    ImGui_ImplOpenGL3_Shutdown();
}

void gearoenix::gl::Engine::start_frame() noexcept
{
    todos.unload();
    render::engine::Engine::start_frame();
    todos.unload();

    ImGui_ImplOpenGL3_NewFrame();
    ImGui::NewFrame();
}

void gearoenix::gl::Engine::end_frame() noexcept
{
    todos.unload();
    render::engine::Engine::end_frame();
    todos.unload();
    submission_manager->update();
    todos.unload();
}

void gearoenix::gl::Engine::window_resized() noexcept
{
}

void gearoenix::gl::Engine::upload_imgui_fonts() noexcept
{
}

bool gearoenix::gl::Engine::is_supported() noexcept
{
    return load_library();
}

std::unique_ptr<gearoenix::gl::Engine> gearoenix::gl::Engine::construct(platform::Application& platform_application) noexcept
{
    if (!is_supported())
        return std::unique_ptr<gearoenix::gl::Engine>();
    return std::make_unique<gearoenix::gl::Engine>(platform_application);
}

#endif
