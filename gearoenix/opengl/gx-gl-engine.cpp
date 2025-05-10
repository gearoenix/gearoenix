#include "gx-gl-engine.hpp"
#ifdef GX_RENDER_OPENGL_ENABLED
#include "../core/ecs/gx-cr-ecs-world.hpp"
#include "../physics/gx-phs-engine.hpp"
#include "../platform/gx-plt-application.hpp"
#include "gx-gl-camera.hpp"
#include "gx-gl-check.hpp"
#include "gx-gl-constants.hpp"
#include "gx-gl-light.hpp"
#include "gx-gl-loader.hpp"
#include "gx-gl-mesh.hpp"
#include "gx-gl-model.hpp"
#include "gx-gl-reflection.hpp"
#include "gx-gl-scene.hpp"
#include "gx-gl-skybox.hpp"
#include "gx-gl-submission-manager.hpp"
#include "gx-gl-texture.hpp"
#include "material/gx-gl-mat-manager.hpp"
#include "shader/gx-gl-shd-manager.hpp"
#include <imgui/backends/imgui_impl_opengl3.h>
#include <imgui/imgui.h>

gearoenix::gl::Engine::Engine()
    : render::engine::Engine(render::engine::Type::OpenGL)
    , Singleton<Engine>(this)
    , shader_manager(new shader::Manager())
{
    while (GL_NO_ERROR != glGetError()) { /*ignoring some error that caused by others*/
    }
    GX_GL_CHECK_D;
    ImGui_ImplOpenGL3_Init("#version 300 es");
    glGetError();
    GX_GL_CHECK_D;

    frames_count = GEAROENIX_GL_FRAMES_COUNT;

    sint max_texture_size = 0;
    glGetIntegerv(GL_MAX_TEXTURE_SIZE, &max_texture_size);
    constexpr sint min_supported_texture_size = 2048;
    specification.texture_maximum_aspect = static_cast<unsigned int>(
        max_texture_size > min_supported_texture_size ? max_texture_size : min_supported_texture_size);
    sint max_attach = 0;
    glGetIntegerv(GL_MAX_COLOR_ATTACHMENTS, &max_attach);
    specification.texture_maximum_target_attachments = static_cast<unsigned int>(max_attach);
    specification.is_deferred_supported = max_attach >= GEAROENIX_GL_GBUFFERS_FRAMEBUFFER_ATTACHMENTS_COUNT;
    specification.is_raytracing_supported = false;
    specification.is_float_texture_supported = extension_exists("GL_OES_texture_float") && extension_exists("GL_OES_texture_float_linear") && extension_exists("GL_OES_texture_half_float") && extension_exists("GL_OES_texture_half_float_linear");
    specification.is_deferred_supported = false;

    scene_manager = std::make_unique<SceneManager>();
    GX_GL_CHECK_D;
    camera_manager = std::make_unique<CameraManager>();
    GX_GL_CHECK_D;
    mesh_manager = std::make_unique<MeshManager>();
    GX_GL_CHECK_D;
    model_manager = std::make_unique<ModelManager>();
    GX_GL_CHECK_D;
    material_manager = std::make_unique<material::Manager>();
    GX_GL_CHECK_D;
    texture_manager = std::make_unique<TextureManager>();
    GX_GL_CHECK_D;
    submission_manager = std::make_unique<submission::Manager>();
    skybox_manager = std::make_unique<SkyboxManager>();
    reflection_manager = std::make_unique<ReflectionManager>();
    light_manager = std::make_unique<LightManager>();
    core::job::execute_current_thread_jobs();
}

gearoenix::gl::Engine::~Engine()
{
    world = nullptr;
    skybox_manager = nullptr;
    submission_manager = nullptr;
    texture_manager = nullptr;
    model_manager = nullptr;
    mesh_manager = nullptr;
    camera_manager = nullptr;
    physics_engine = nullptr;
    material_manager = nullptr;
    reflection_manager = nullptr;
    core::job::execute_current_thread_jobs();
    ImGui_ImplOpenGL3_DestroyFontsTexture();
    ImGui_ImplOpenGL3_Shutdown();
}

void gearoenix::gl::Engine::start_frame()
{
    core::job::execute_current_thread_jobs();
    render::engine::Engine::start_frame();
    core::job::execute_current_thread_jobs();

    ImGui_ImplOpenGL3_NewFrame();
    ImGui::NewFrame();
}

void gearoenix::gl::Engine::end_frame()
{
    core::job::execute_current_thread_jobs();
    render::engine::Engine::end_frame();
    core::job::execute_current_thread_jobs();
    submission_manager->update();
    core::job::execute_current_thread_jobs();
}

void gearoenix::gl::Engine::window_resized()
{
    render::engine::Engine::window_resized();
    submission_manager->window_resized();
}

void gearoenix::gl::Engine::upload_imgui_fonts()
{
    ImGui_ImplOpenGL3_CreateFontsTexture();
}

bool gearoenix::gl::Engine::is_supported()
{
    return load_library();
}

std::unique_ptr<gearoenix::gl::Engine> gearoenix::gl::Engine::construct()
{
    if (!is_supported()) {
        return {};
    }
    return std::make_unique<Engine>();
}

#endif
